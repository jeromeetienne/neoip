/*
 * crit.h: Access helper function declarations.
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

#ifndef _CRIT_H
#define _CRIT_H

#include <windows.h>

extern void init_service_status (SERVICE_STATUS_HANDLE nssh);
extern void report_service_status (bool incr = true);
extern void set_service_status (DWORD state,
				DWORD check_point = 0,
		                DWORD wait_hint = 0,
				DWORD exit_code = NO_ERROR);
extern void set_service_controls_accepted(bool shutdown, bool interactive);

#endif /* _CRIT_H */
