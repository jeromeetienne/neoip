/*
 * waitanypid.cc: Function to wait for any Cygwin process
 *
 * Copyright 2005 Christian Franke <franke@computer.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <windows.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "waitanypid.h"


/*
 * Convert Cygwin pid into Windows pid
 * Returns windows pid, or -1 on error
 */

static int
pid_to_winpid (pid_t pid)
{
  char path[100];
  sprintf (path, "/proc/%d/winpid", pid);
  FILE * f = fopen (path, "r");
  if (!f)
    return -1;
  int winpid = -1;
  fscanf (f, "%d", &winpid);
  fclose (f);
  return winpid;
}


/*
 * #include "waitanypid.h"
 * 
 * int waitanypid(pid_t pid, int * status,
 *                int options = 0, long * handle = 0)
 *
 * Waitanypid waits for any Process and returns its exit status.
 *
 * Unlike waitpid(2), waitanypid can wait for non-child processes,
 * but it cannot wait for process groups.
 *
 * Options can be WNOHANG, which means return immediately
 * if process is still running.
 *
 * The optional handle parameter is the address of a variable
 * where the windows process handle is stored during first call
 * and used in subsequent calls. The variable itself must be set
 * to zero before first call.
 * The handle should be given if WNOHANG is set, otherwise
 * waitanypid cannot retrieve the status after the process has
 * terminated. An error ESRCH will be returned in this case.
 *
 * If WNOHANG is not specified, handle can be zero.
 * 
 * Return value:
 *   pid (> 0) on success,
 *   -1 on error,
 *   0 if WNOHANG is specified and process is still running.
 *
 * Errors:
 *   ESRCH: No such process (or process terminated before handle
 *          could be acquired)
 *
 *   EPERM: Permission denied (or other unexpected error ;-).
 */

extern "C" int
waitanypid(pid_t pid, int * status, int options, long * handle)
{
  HANDLE h = (HANDLE) (handle ? *handle : 0);
  if (!h)
    {
      int winpid = pid_to_winpid (pid);
      if (winpid == -1) {
	errno = ESRCH;
	return -1;
      }
      h = OpenProcess (PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, winpid);
      if (!h)
        {
	  errno = (GetLastError () == ERROR_INVALID_PARAMETER ? ESRCH : EPERM);
	  return -1;
	}
      if (handle)
	*handle = (long) h;
    }

  int wr = WaitForSingleObject (h, (options & WNOHANG ? 0 : INFINITE));
  if (wr == WAIT_TIMEOUT && (options & WNOHANG))
    {
      if (!handle)
	CloseHandle (h);
      return 0;
    }

  if (wr == WAIT_OBJECT_0)
    {
      DWORD code = 0xff00;
      GetExitCodeProcess (h, &code);
      *status = code;
    }
  else
    {
      errno = EPERM;
      pid = -1;
  }

  CloseHandle (h);
  if (handle)
    *handle = 0;
  return pid;
}


#ifdef TEST

#include <stdio.h>

int main(int argc, char ** argv)
{
  int options = 0;
  int i = 1;
  if (argc > 1 && !strcmp(argv[1],"-n"))
    options = WNOHANG, i++;
  long handle = 0;
  for ( ; i < argc; i++) {
    int pid = atoi(argv[i]);
    for (;;) {
      int status = 42;
      printf("waitanypid(%d,.,0x%02x,.)=", pid, options); fflush(stdout);
      int rc = waitanypid(pid, &status, options, &handle);
      printf("%d, status=%d(%d), handle=%ld, errno=%d\n", rc,
        status, WEXITSTATUS(status), handle, errno);
      if (rc) 
        break;
      sleep(1);
    }
  }
  return 0;
}

#endif

