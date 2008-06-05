/*
 * utils.cc: Various utility functions for cygrunsrv.
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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>

#include <windows.h>

#include "cygrunsrv.h"
#include "utils.h"

char *reason_list[] = {
  "",
  "Exactly one of --install, --remove, --start, --stop, --query, or --list is required",
  "--path is required with --install",
  "Given path doesn't point to a valid executable",
  "--path is only allowed with --install",
  "Only one --path is allowed",
  "--args is only allowed with --install",
  "Only one --args is allowed",
  "--chdir is only allowed with --install",
  "Only one --chdir is allowed",
  "--env is only allowed with --install",
  "--disp is only allowed with --install",
  "Only one --disp is allowed",
  "--desc is only allowed with --install",
  "Only one --desc is allowed",
  "--user is only allowed with --install",
  "Only one --user is allowed",
  "--pass is only allowed with --install",
  "Only one --pass is allowed",
  "--type is only allowed with --install",
  "Only one --type is allowed",
  "Invalid type, only `a[uto]' or `m[anual]' are vaild.",
  "--termsig is only allowed with --install",
  "Only one --termsig is allowed",
  "Invalid signal; must be number or name like INT, QUIT, TERM, etc.",
  "--dep is only allowed with --install",
  "--std{in,out,err},--pidfile are only allowed with --install",
  "Each of --std{in,out,err},--pidfile is allowed only once",
  "--neverexits is only allowed with --install",
  "Only one --neverexits is allowed",
  "--shutdown is only allowed with --install",
  "Only one --shutdown is allowed",
  "--interactive is only allowed with --install",
  "Only one --interactive is allowed",
  "--interactive not allowed with --user",
  "--nohide is only allowed with --install",
  "Only one --nohide is allowed",
  "Trailing commandline arguments not allowed",
  "You must specify one of the `-IRSE' options",
  "Error installing a service",
  "Error removing a service",
  "Error starting a service",
  "Error stopping a service",
  "Error querying a service",
  "Error enumerating services",
  NULL
};

int
error (reason_t reason, const char *func, DWORD win_err)
{
  if (reason > UnrecognizedOption && reason < MaxReason)
    {
      fprintf (stderr, "%s: %s", appname, reason_list[reason]);
      if (reason > StartAsSvcErr)
        {
	  if (func)
	    fprintf (stderr, ": %s", func);
	  if (win_err)
	    fprintf (stderr, ":  Win32 error %lu:\n%s",
	    	     win_err, winerror (win_err));
	}
      if (reason <= StartAsSvcErr)
        fprintf (stderr, "\nTry `%s --help' for more information.", appname);
      fputc ('\n', stderr);
    }
  return 1;
}

char *
winerror (DWORD win_err)
{
  static char errbuf[1024]; /* That's really sufficient. */

  errbuf[0] = '\0';
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
  		NULL, win_err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		errbuf, 1024, NULL);
  return errbuf;
}

void
syslog_starterr (const char *func, DWORD win_err, int posix_err)
{
  syslog (LOG_ERR,
  	  "starting service `%s' failed: %s: %d, %s",
  	  svcname,
	  func,
	  win_err ?: posix_err,
	  win_err ? winerror (win_err) : strerror (posix_err));
}

#define uprint(fmt)	{fprintf(stderr,fmt,appname);fputc('\n',stderr);}

int
usage ()
{

  uprint ("\nUsage: %s [OPTION]...");
  uprint ("\nMain options: Exactly one is required.");
  uprint ("  -I, --install <svc_name>  Installes a new service named <svc_name>.");
  uprint ("  -R, --remove <svc_name>   Removes a service named <svc_name>.");
  uprint ("  -S, --start <svc_name>    Starts a service named <svc_name>.");
  uprint ("  -E, --stop <svc_name>     Stops a service named <svc_name>.");
  uprint ("  -Q, --query <svc_name>    Queries a service named <svc_name>.");
  uprint ("  -L, --list                Lists services that have been installed");
  uprint ("                            with cygrunsrv.");
  uprint ("\nRequired install options:");
  uprint ("  -p, --path <app_path>     Application path which is run as a service.");
  uprint ("\nMiscellaneous install options:");
  uprint ("  -a, --args <args>         Optional string with command line options which");
  uprint ("                            is given to the service application on startup.");
  uprint ("  -c, --chdir <directory>   Optional directory which will be used as working");
  uprint ("                            directory for the application.");
  uprint ("  -e, --env <VAR=VALUE>     Optional environment strings which are added");
  uprint ("                            to the environment when service is started.");
  uprint ("                            You can add up to " MAX_ENV_STR " environment strings using");
  uprint ("                            the `--env' option.");
  uprint ("                            Note: /bin is always added to $PATH to allow all");
  uprint ("                            started applications to find at least cygwin1.dll.");
  uprint ("  -d, --disp <display name> Optional string which contains the display name");
  uprint ("                            of the service. Defaults to service name.");
  uprint ("  -f, --desc <description>  Optional string which contains the service");
  uprint ("                            description.");
  uprint ("  -t, --type [auto|manual]  Optional start type of service. Defaults to `auto'.");
  uprint ("  -u, --user <user name>    Optional user name to start service as.");
  uprint ("                            Defaults to SYSTEM account.");
  uprint ("  -w, --passwd <password>   Optional password for user. Only needed");
  uprint ("                            if a user is given. If a user has an empty");
  uprint ("                            password, enter `-w ""'. If a user is given but");
  uprint ("                            no password, cygrunsrv will ask for a password");
  uprint ("                            interactively.");
  uprint ("  -s, --termsig <signal>    Optional signal to send to service application");
  uprint ("                            when service is stopped.  <signal> can be a number");
  uprint ("                            or a signal name such as HUP, INT, QUIT, etc.");
  uprint ("                            Default is TERM.");
  uprint ("  -y, --dep <svc_name2>     Optional name of service that must be started");
  uprint ("                            before this new service.  The --dep option may");
  uprint ("                            be given up to " MAX_DEPS_STR " times, listing another dependent");
  uprint ("                            service each time.");
  uprint ("  -0, --stdin <file>        Optional input file used for stdin redirection.");
  uprint ("                            Default is /dev/null.");
  uprint ("  -1, --stdout <file>       Optional output file used for stdout redirection.");
  uprint ("                            Default is /var/log/<svc_name>.log.");
  uprint ("  -2, --stderr <file>       Optional output file used for stderr redirection.");
  uprint ("                            Default is /var/log/<svc_name>.log.");
  uprint ("  -x, --pidfile <file>      Optional path for .pid file written by application");
  uprint ("                            after fork().");
  uprint ("                            Default is that application must not fork().");
  uprint ("  -n, --neverexits          Service should never exit by itself.");
  uprint ("  -o, --shutdown            Stop service application during system shutdown.");
  uprint ("  -i, --interactive         Allow service to interact with the desktop.");
  uprint ("  -j, --nohide              Don't hide console window when service interacts");
  uprint ("                            with desktop.");
  uprint ("\nInformative output:");
  uprint ("  -V, --verbose             When used with --query or --list, causes extra");
  uprint ("                            information to be printed.");
  uprint ("  -h, --help                print this help, then exit.");
  uprint ("  -v, --version             print cygrunsrv program version number, then exit.");
  uprint ("\nReport bugs to <cygwin@cygwin.com>.");

  return 1;
}

BOOL
is_executable (const char *path)
{
  struct stat st;

  if (stat (path, &st))
    return 0;
  if (!S_ISREG (st.st_mode))
    return 0;
  return !access (path, X_OK);
}

int
create_parent_directory (const char *path)
{
  char buf[MAX_PATH + 1], *c;
  struct stat st;

  strcpy (buf, path);
  /* If not in the root dir. */
  if ((c = strrchr (buf, '/')) && c > buf + 1 && c[-1] != ':')
    {
      *c = '\0';
      /* Parent path already exists ... */
      if (!stat (buf, &st))
        {
	  /* ...but is not a directory. */
	  if (!S_ISDIR (st.st_mode))
	    return -1;
	  /* ...but isn't accessible for me. */
	  if (access (buf, W_OK) &&
	      chmod (buf, S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO))
	    return -1;
	  return 0;
	}
      /* If we can't create the dir, try to create the parent dir (recursive).
         If that fails or another try to create the dir, give up. */
      if (mkdir (buf, S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO) &&
          (create_parent_directory (buf) ||
	   mkdir (buf, S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO)))
        return -1;
    }
  return 0;
}

int
redirect_fd (int fd, const char *path, BOOL output)
{
  char funcbuf[MAX_PATH + 64];
  int nfd = -1;

  close (fd);
  if (!output)
    nfd = open (path, O_RDWR);
  else if (!create_parent_directory (path))
    nfd = open (path, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (nfd < 0)
    {
      sprintf (funcbuf, "redirect_fd: open (%d, %s)", fd, path);
      syslog_starterr (funcbuf, 0, errno);
      return -1;
    }
  if (nfd != fd)
    {
      if (dup2 (nfd, fd))
        {
	  sprintf (funcbuf, "redirect_fd: dup2 (%d, %s)", fd, path);
	  syslog_starterr (funcbuf, 0, errno);
	  return -1;
	}
      close (nfd);
    }
  return 0;
}

int
redirect_io (const char *stdin_path, const char *stdout_path,
	     const char *stderr_path)
{
  if (redirect_fd (STDIN_FILENO, stdin_path, FALSE) ||
      redirect_fd (STDOUT_FILENO, stdout_path, TRUE) ||
      redirect_fd (STDERR_FILENO, stderr_path, TRUE))
    return -1;
  return 0;
}
