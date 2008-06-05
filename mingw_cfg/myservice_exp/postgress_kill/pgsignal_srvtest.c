#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "pgsignal_w32.h"

void hup_handler(int s) {
	printf("SIGHUP handler called!\n");
}

int main (int argc, char *argv[]) {

	int i;

	printf("Server starting, pid %i\n",GetCurrentProcessId());

	pg_signal_initialize();

	signal(SIGHUP,hup_handler);
	for (i = 100; i; i--) {
		printf("Loop %i\n",i);
		SleepEx(5000,TRUE);
		PG_POLL_SIGNALS();
	}

	return 0;
}