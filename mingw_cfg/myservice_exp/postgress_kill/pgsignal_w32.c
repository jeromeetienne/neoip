#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <aclapi.h>

/* Only for temp error handling. Replaced with postgresql error functions later */
#include <stdio.h>
#include <stdlib.h>


#include "pgsignal_w32.h"

/* Global variable used to check if there are any signals queued */
volatile int __pg_signal_count;



static CRITICAL_SECTION pg_signal_crit_sec;
#define PG_SIGNAL_COUNT 32
static pg_signal_function pg_signal_array[PG_SIGNAL_COUNT];
static BYTE pg_signal_queue[PG_SIGNAL_COUNT];
static int pg_signal_mask;
static HANDLE pg_main_thread_handle;
static HANDLE pg_signal_thread_handle;

/* Forward function declarations */
static DWORD WINAPI pg_signal_thread_proc(LPVOID param);

/* Initialization */
void pg_signal_initialize(void) {
	InitializeCriticalSection(&pg_signal_crit_sec);

	memset(&pg_signal_array,0,sizeof(pg_signal_array));
	memset(&pg_signal_queue,0,sizeof(pg_signal_queue));
	pg_signal_mask = 0;
	__pg_signal_count = 0;

	/* Get handle to main thread so we can post calls to it later */
	if (!DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),&pg_main_thread_handle,0,FALSE,DUPLICATE_SAME_ACCESS)) {
		printf("Failed to get thread handle!\n");
		exit(1);
	}

	/* Create thread for handling signals */
	pg_signal_thread_handle = CreateThread(NULL,0,pg_signal_thread_proc,NULL,0,NULL);
	if (pg_signal_thread_handle == NULL) {
		printf("Failed to create handler thread!\n");
		exit(1);
	}
}

/* signal masking */
int sigsetmask(int mask) {
	int prevmask;
	EnterCriticalSection(&pg_signal_crit_sec);
	prevmask = pg_signal_mask;
	pg_signal_mask = mask;
	LeaveCriticalSection(&pg_signal_crit_sec);
	return prevmask;
}

int siggetmask(void) {
	int mask;
	EnterCriticalSection(&pg_signal_crit_sec);
	mask = pg_signal_mask;
	LeaveCriticalSection(&pg_signal_crit_sec);
	return mask;
}

int sigblock(int mask) {
	int prevmask;
	EnterCriticalSection(&pg_signal_crit_sec);
	prevmask = pg_signal_mask;
	pg_signal_mask |= mask;
	LeaveCriticalSection(&pg_signal_crit_sec);
	return prevmask;
}

/* signal manipulation */
pg_signal_function signal(int signum, pg_signal_function handler) {
	pg_signal_function prevfunc;
	if (signum > PG_SIGNAL_COUNT)
		return SIG_ERR;
	EnterCriticalSection(&pg_signal_crit_sec);
	prevfunc = pg_signal_array[signum];
	pg_signal_array[signum] = handler;
	LeaveCriticalSection(&pg_signal_crit_sec);
	return prevfunc;
}

/* signal sending */
int kill(int pid, int sig) {
	char pipename[128];
	BYTE sigData = sig;
	BYTE sigRet = 0;
	DWORD bytes;

	if (sig > PG_SIGNAL_COUNT || sig <= 0) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return -1;
	}
	if (pid <= 0) {
		/* No support for process groups */
		SetLastError(ERROR_INVALID_PARAMETER);
		return -1;
	}
	wsprintf(pipename,"\\\\.\\pipe\\pgsignal_%i",pid);
	if (!CallNamedPipe(pipename,&sigData,1,&sigRet,1,&bytes,1000)) {
		return -1;
	}
	if (bytes != 1 || sigRet != sig)
		return -1;

	return 0;
}

int raise(int sig) {
	return kill(GetCurrentProcessId(),sig);
}

/* signal polling */
void __pg_poll_signals(void) {
	pg_signal_function signals_to_call[PG_SIGNAL_COUNT];
	int i = -1;

	memset(&signals_to_call,0,sizeof(signals_to_call));

	/* Copy any signals we need to call, so we can start accepting
	   new ones. */
	EnterCriticalSection(&pg_signal_crit_sec);
	if (__pg_signal_count > 0) {
		for (i = 0; i < PG_SIGNAL_COUNT; i++) {
			if (pg_signal_queue[i]) {
				if ((pg_signal_mask & sigmask(i)) == 0) {
					if (pg_signal_array[i] != SIG_ERR &&
						pg_signal_array[i] != SIG_DFL &&
						pg_signal_array[i] != SIG_IGN) {
							signals_to_call[i] = pg_signal_array[i];
					}
					pg_signal_queue[i] = 0;
				}
			}
		}

		/* Reset signal count */
		InterlockedExchange(&__pg_signal_count,0);
	}
	LeaveCriticalSection(&pg_signal_crit_sec);

	/* Now call any signals */
	if (i != -1) {
		/* __pg_signal_count > 0 */
		for (i = 0; i < PG_SIGNAL_COUNT; i++) {
			if (signals_to_call[i] != NULL) {
				signals_to_call[i](i);
			}
		}
	}
}

/* APC callback scheduled on main thread when signals are fired */
static void CALLBACK pg_signal_apc(ULONG_PTR param) {
	__pg_poll_signals();
}


/*
 * All functions below execute on the signal handler thread
 * and must be synchronized as such!
 */
static void pg_queue_signal(int signum) {
	if (signum > PG_SIGNAL_COUNT)
		return;

	EnterCriticalSection(&pg_signal_crit_sec);
	pg_signal_queue[signum] = 1;
	InterlockedIncrement(&__pg_signal_count);
	LeaveCriticalSection(&pg_signal_crit_sec);

	QueueUserAPC(pg_signal_apc,pg_main_thread_handle,(ULONG_PTR)NULL);
}

/* Signal handling thread */
static DWORD WINAPI pg_signal_thread_proc(LPVOID param) {
	char pipename[128];
	HANDLE pipe = INVALID_HANDLE_VALUE;

	wsprintf(pipename,"\\\\.\\pipe\\pgsignal_%i",GetCurrentProcessId());

	for (;;) {
		BYTE sigNum = 255;
		DWORD bytes;

		if (pipe != INVALID_HANDLE_VALUE)
			CloseHandle(pipe);

		pipe = CreateNamedPipe(pipename,PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,PIPE_UNLIMITED_INSTANCES,16,16,1000,NULL);
		if (pipe == INVALID_HANDLE_VALUE) {
			printf("Failed to create signal listener pipe: %i. Retrying.\n",GetLastError());
			SleepEx(500,TRUE);
			continue;
		}
		if (!ConnectNamedPipe(pipe,NULL)) {
			if (GetLastError() != ERROR_PIPE_CONNECTED) {
				printf("Failed to connect to signal listener pipe: %i. Retrying.\n",GetLastError());
				continue;
			}
		}
		if (!ReadFile(pipe,&sigNum,1,&bytes,NULL)) {
			/* Normal error - client died before sending */
			continue;
		}
		if (bytes != 1) {
			printf("Received %i bytes over signal pipe (should be 1).\n",bytes);
			continue;
		}

		pg_queue_signal(sigNum);

		WriteFile(pipe,&sigNum,1,&bytes,NULL);

		/* Pipe is closed at the top of the loop */
	}
	return 0;
}
