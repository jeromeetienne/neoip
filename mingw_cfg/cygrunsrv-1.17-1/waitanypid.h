/*
 * waitanypid.h: Function to wait for any Cygwin process
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

#ifndef _WAITANYPID_H
#define _WAITANYPID_H

#include <sys/wait.h> // WNOHANG

#ifdef __cplusplus
extern "C" {
#endif

int waitanypid(pid_t pid, int * status, int options, long * handle);

#ifdef __cplusplus
}

inline int waitanypid(pid_t pid, int * status, int options = 0)
  { return waitanypid(pid, status, options, (long*)0); }

#endif

#endif

