/*
 * crit.cc: Access helper functions for SERVICE_STATUS.
 *
 * Copyright 2001, 2003  Corinna Vinschen, <corinna@vinschen.de>
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

#include "crit.h"

static SERVICE_STATUS_HANDLE ssh;
static SERVICE_STATUS ss;
static CRITICAL_SECTION ssc;

void
init_service_status (SERVICE_STATUS_HANDLE nssh)
{
  InitializeCriticalSection (&ssc);
  EnterCriticalSection (&ssc);
  ssh = nssh;
  ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  ss.dwCurrentState = SERVICE_START_PENDING;
  ss.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  ss.dwWin32ExitCode = NO_ERROR;
  ss.dwServiceSpecificExitCode = NO_ERROR;
  ss.dwCheckPoint = 1;
  ss.dwWaitHint = 5000L;
  LeaveCriticalSection (&ssc);
}

void
report_service_status (bool incr)
{
  EnterCriticalSection (&ssc);
  if (ssh)
    {
      if (incr)
	++ss.dwCheckPoint;
      SetServiceStatus(ssh, &ss);
    }
  LeaveCriticalSection (&ssc);
}

void
set_service_status (DWORD state, DWORD check_point,
		    DWORD wait_hint, DWORD exit_code)
{
  EnterCriticalSection (&ssc);
  if (ssh)
    {
      ss.dwCurrentState = state;
      ss.dwWin32ExitCode = exit_code;
      ss.dwCheckPoint = check_point;
      ss.dwWaitHint = wait_hint;
      SetServiceStatus(ssh, &ss);
    }
  LeaveCriticalSection (&ssc);
}

void
set_service_controls_accepted (bool accept_shutdown, bool interactive_process)
{
  EnterCriticalSection (&ssc);
  if (ssh)
    {
      ss.dwControlsAccepted = SERVICE_ACCEPT_STOP;
      if (accept_shutdown)
	ss.dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
      if (interactive_process)
	ss.dwServiceType |= SERVICE_INTERACTIVE_PROCESS;
      SetServiceStatus(ssh, &ss);
    }
  LeaveCriticalSection (&ssc);
}
