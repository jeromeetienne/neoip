/*
 * utils.h: Utility function declarations.
 *
 * Copyright 2001, 2002, 2003, 2004, 2005  Corinna Vinschen,
 * <corinna@vinschen.de>
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

#ifndef _UTILS_H
#define _UTILS_H

enum reason_t {
  UnrecognizedOption,
  ReqAction,
  ReqPath,
  InvalidPath,
  PathNotAllowed,
  OnlyOnePath,
  ArgsNotAllowed,
  OnlyOneArgs,
  ChdirNotAllowed,
  OnlyOneChdir,
  EnvNotAllowed,
  DispNotAllowed,
  OnlyOneDisp,
  DescNotAllowed,
  OnlyOneDesc,
  UserNotAllowed,
  OnlyOneUser,
  PassNotAllowed,
  OnlyOnePass,
  TypeNotAllowed,
  OnlyOneType,
  InvalidType,
  SigNotAllowed,
  OnlyOneSig,
  InvalidSig,
  DepNotAllowed,
  IONotAllowed,
  OnlyOneIO,
  NeverExitsNotAllowed,
  OnlyOneNeverExits,
  ShutdownNotAllowed,
  OnlyOneShutdown,
  InteractiveNotAllowed,
  OnlyOneInteractive,
  NoInteractiveWithUser,
  ShowconsNotAllowed,
  OnlyOneShowcons,
  TrailingArgs,
  StartAsSvcErr,
  InstallErr,
  RemoveErr,
  StartErr,
  StopErr,
  QueryErr,
  ListErr,
  MaxReason		/* Always the last element */
};

extern char *winerror (DWORD win_err);
extern int error (reason_t reason, const char *func = NULL, DWORD win_err = 0);
extern void syslog_starterr(const char *func, DWORD win_err, int posix_err = 0);
extern int usage ();
BOOL is_executable (const char *path);
int redirect_fd (int fd, const char *path, BOOL output);
int redirect_io (const char *stdin_path, const char *stdout_path,
		 const char *stderr_path);
int create_parent_directory (const char *path);

#endif /* _UTILS_H */
