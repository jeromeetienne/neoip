/*
 * cygrunsrv.h: Defining stuff for cygrunsrv.
 *
 * Copyright 2001, 2002, 2003, 2004  Corinna Vinschen, <corinna@vinschen.de>
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

#ifndef _CYGRUNSRV_H
#define _CYGRUNSRV_H

#define SRV_KEY	        "SYSTEM\\CurrentControlSet\\Services\\"
#define DESC            "Description"
#define PARAM_KEY	"\\Parameters"
#define PARAM_PATH	"AppPath"
#define PARAM_ARGS	"AppArgs"
#define PARAM_DIR 	"WorkingDirectory"
#define PARAM_STDIN	"StdIn"
#define PARAM_STDOUT	"StdOut"
#define PARAM_STDERR	"StdErr"
#define PARAM_PIDFILE	"PidFile"
#define PARAM_ENVIRON	"Environment"
#define PARAM_TERMSIG	"TermSig"
#define PARAM_NEVEREXITS "NeverExits"
#define PARAM_SHUTDOWN	"Shutdown"
#define PARAM_INTERACT	"Interactive"
#define PARAM_SHOWCONS	"ShowConsole"

#define DEF_STDIN_PATH	"/dev/null"
#define DEF_LOG_PATH	"/var/log/"

#define STRINGIFY_(X)   #X
#define STRINGIFY(X)    STRINGIFY_(X)

#define MAX_ENV		255
#define MAX_ENV_STR	STRINGIFY(MAX_ENV)

#define MAX_DEPS	16
#define MAX_DEPS_STR	STRINGIFY(MAX_DEPS)

extern char *appname;
extern char *svcname;

#endif /* _CYGRUNSRV_H */
