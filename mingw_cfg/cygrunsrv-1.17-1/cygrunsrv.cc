/*
 * cygrunsrv.cc: implementation of a Cygwin service starter application
 *               similar to SRVANY or Invoker but straight implemented
 *		 to support Cygwin applications.
 *
 * Copyright 2001, 2002, 2003, 2004, 2005, 2006  Corinna Vinschen,
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
#include <getopt.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <libgen.h>
#include <mntent.h>
#include <sys/strace.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <windows.h>
#include <wininet.h>
#include <lmcons.h>

#include <exceptions.h>
#include <sys/cygwin.h>

#include "cygrunsrv.h"
#include "crit.h"
#include "utils.h"
#include "waitanypid.h"

static char *SCCSid = "@(#)cygrunsrv V1.17, " __DATE__ "\n";

struct option longopts[] = {
  { "install", required_argument, NULL, 'I' },
  { "remove", required_argument, NULL, 'R' },
  { "start", required_argument, NULL, 'S' },
  { "stop", required_argument, NULL, 'E' },
  { "query", required_argument, NULL, 'Q' },
  { "list", no_argument, NULL, 'L' },
  { "path", required_argument, NULL, 'p' },
  { "args", required_argument, NULL, 'a' },
  { "chdir", required_argument, NULL, 'c' },
  { "env", required_argument, NULL, 'e' },
  { "disp", required_argument, NULL, 'd' },
  { "desc", required_argument, NULL, 'f' },
  { "user", required_argument, NULL, 'u' },
  { "passwd", required_argument, NULL, 'w' },
  { "type", required_argument, NULL, 't' },
  { "termsig", required_argument, NULL, 's' },
  { "dep", required_argument, NULL, 'y' },
  { "stdin", required_argument, NULL, '0' },
  { "stdout", required_argument, NULL, '1' },
  { "stderr", required_argument, NULL, '2' },
  { "pidfile", required_argument, NULL, 'x' },
  { "neverexits", no_argument, NULL, 'n' },
  { "shutdown", no_argument, NULL, 'o' },
  { "interactive", no_argument, NULL, 'i' },
  { "nohide", no_argument, NULL, 'j' },
  { "verbose", no_argument, NULL, 'V' },
  { "help", no_argument, NULL, 'h' },
  { "version", no_argument, NULL, 'v' },
  { 0, no_argument, NULL, 0 }
};

char *opts = "I:"
  "R:"
  "S:"
  "E:"
  "Q:"
  "L"
  "p:"
  "a:"
  "c:"
  "e:"
  "d:"
  "f:"
  "o"
  "u:"
  "w:"
  "t:"
  "s:"
  "y:"
  "0:"
  "1:"
  "2:"
  "x:"
  "n"
  "i"
  "j"
  "V"
  "h"
  "v";

char *appname;
char *svcname;
DWORD termsig;
bool termsig_sent;
DWORD neverexits;
DWORD shutdown;
DWORD interactive;
DWORD showcons;

DWORD shutting_down = 0;
static char service_main_exitval = 1;

enum action_t {
  Undefined,
  Install,
  Remove,
  Start,
  Stop,
  Query,
  List
};

enum type_t {
  NoType,
  Auto,
  Manual
};

struct env_t
{
  char *name;
  char *val;
};

int
version ()
{
  printf ("%s\n", SCCSid + 4);
  printf ("Copyright 2001, 2002, 2003, 2004, 2005, 2006 Corinna Vinschen\n");
  printf ("This program comes with NO WARRANTY, to the extent permitted by law.\n");
  printf ("You may redistribute it under the terms of the GNU General Public License;\n");
  printf ("see the file named COPYING for details.\n");
  printf ("Written by Corinna Vinschen and Fred Yankowski.\n");
  return 0;
}

static inline DWORD
eval_wait_time (register DWORD wait)
{
  if ((wait /= 10) < 1000)
    wait = 1000L;
  else if (wait > 10000L)
    wait = 10000L;
  return wait;
}

#define err_out(name)	{err_func = #name; err = GetLastError (); goto out;}
#define err_out_set_error(name, error) \
            {err_func = #name; err = error; SetLastError (error); goto out;}

/* Installs the subkeys of the service registry entry so that cygrunsrv
   can determine what application to start on service startup. */
int
install_registry_keys (const char *name, const char *desc, const char *path,
		       char *args, char *dir, env_t *env, DWORD termsig,
		       const char *in_stdin, const char *in_stdout,
		       const char *in_stderr, const char *in_pidfile,
		       DWORD neverexits, DWORD shutdown, DWORD interactive,
		       DWORD showcons)
{
  HKEY srv_key = NULL;
  HKEY env_key = NULL;
  HKEY desc_key = NULL;
  DWORD disp;
  char reg_key[MAX_PATH];
  char *err_func;
  DWORD err = 0;

  strcat (strcpy (reg_key, SRV_KEY), name);
  if (desc)
    {
      if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, reg_key, 0,
                        KEY_ALL_ACCESS, &desc_key) != ERROR_SUCCESS)
        err_out (RegOpenKeyEx);
      if (RegSetValueEx (desc_key, DESC, 0, REG_SZ,
                         (const BYTE *) desc, strlen (desc) + 1) != ERROR_SUCCESS)
        err_out (RegSetValueEx);
      RegFlushKey (desc_key);
    }
  
  strcat (reg_key, PARAM_KEY);
  if (RegCreateKeyEx (HKEY_LOCAL_MACHINE, reg_key, 0, "",
  		      REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
		      NULL, &srv_key, &disp) != ERROR_SUCCESS)
    err_out (RegCreateKeyEx);
  if (RegSetValueEx (srv_key, PARAM_PATH, 0, REG_SZ,
  		     (const BYTE *) path, strlen (path) + 1) != ERROR_SUCCESS)
    err_out (RegSetValueEx);
  if (args &&
      RegSetValueEx (srv_key, PARAM_ARGS, 0, REG_SZ,
  		     (const BYTE *) args, strlen (args) + 1) != ERROR_SUCCESS)
    err_out (RegSetValueEx);
  if (dir &&
      RegSetValueEx (srv_key, PARAM_DIR, 0, REG_SZ,
  		     (const BYTE *) dir, strlen (dir) + 1) != ERROR_SUCCESS)
    err_out (RegSetValueEx);
  if (env)
    {
      if (RegCreateKeyEx (srv_key, PARAM_ENVIRON, 0, "",
      			  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
			  NULL, &env_key, &disp) != ERROR_SUCCESS)
	err_out (RegCreateKeyEx);
      for (int i = 0; i <= MAX_ENV && env[i].name; ++i)
	if (RegSetValueEx (env_key, env[i].name, 0, REG_SZ,
			   (const BYTE *) env[i].val,
			   strlen (env[i].val) + 1) != ERROR_SUCCESS)
	  err_out (RegSetValueEx);
    }
  if (termsig)
    if (RegSetValueEx (srv_key, PARAM_TERMSIG, 0, REG_DWORD,
		       (const BYTE *) &termsig,
		       sizeof(DWORD)) != ERROR_SUCCESS)
      err_out (RegSetValueEx);
  if (in_stdin)
    if (RegSetValueEx (srv_key, PARAM_STDIN, 0, REG_SZ,
		       (const BYTE *) in_stdin,
		       strlen (in_stdin) + 1) != ERROR_SUCCESS)
      err_out (RegSetValueEx);
  if (in_stdout)
    if (RegSetValueEx (srv_key, PARAM_STDOUT, 0, REG_SZ,
		       (const BYTE *) in_stdout,
		       strlen (in_stdout) + 1) != ERROR_SUCCESS)
      err_out (RegSetValueEx);
  if (in_stderr)
    if (RegSetValueEx (srv_key, PARAM_STDERR, 0, REG_SZ,
		       (const BYTE *) in_stderr,
		       strlen (in_stderr) + 1) != ERROR_SUCCESS)
      err_out (RegSetValueEx);
  if (in_pidfile)
    if (RegSetValueEx (srv_key, PARAM_PIDFILE, 0, REG_SZ,
		       (const BYTE *) in_pidfile,
		       strlen (in_pidfile) + 1) != ERROR_SUCCESS)
      err_out (RegSetValueEx);
  if (neverexits)
    if (RegSetValueEx (srv_key, PARAM_NEVEREXITS, 0, REG_DWORD,
		       (const BYTE *) &neverexits,
		       sizeof(DWORD)) != ERROR_SUCCESS)
      err_out (RegSetValueEx);
  if (shutdown)
    if (RegSetValueEx (srv_key, PARAM_SHUTDOWN, 0, REG_DWORD,
		       (const BYTE *) &shutdown,
		       sizeof(DWORD)) != ERROR_SUCCESS)
      err_out (RegSetValueEx);
  if (interactive)
    if (RegSetValueEx (srv_key, PARAM_INTERACT, 0, REG_DWORD,
		       (const BYTE *) &interactive,
		       sizeof(DWORD)) != ERROR_SUCCESS)
      err_out (RegSetValueEx);
  if (showcons)
    if (RegSetValueEx (srv_key, PARAM_SHOWCONS, 0, REG_DWORD,
		       (const BYTE *) &showcons,
		       sizeof(DWORD)) != ERROR_SUCCESS)
      err_out (RegSetValueEx);
  RegFlushKey (srv_key);

out:
  if (env_key)
    RegCloseKey (env_key);
  if (desc_key)
    RegCloseKey (desc_key);
  if (srv_key)
    RegCloseKey (srv_key);
  return err == 0 ? 0 : error (InstallErr, err_func, err);
}

/* Get optional string values from registry. */
int
get_opt_string_entry (HKEY key, const char *entry, char*&value)
{
  int ret;
  DWORD type, size = 0;

  /* Not finding the key is no error. */
  if ((ret = RegQueryValueEx (key, entry, 0, &type, NULL, &size)))
    return ERROR_SUCCESS;
  if (!(value = (char *) malloc (size)))
    return ERROR_OUTOFMEMORY;
  if ((ret = RegQueryValueEx (key, entry, 0, &type, (BYTE *) value, &size)))
    {
      free (value);
      value = NULL;
    }
  return ret;
}

int
reeval_io_path (int fd, char *&io_path, const char *svc_name)
{
  char buf[MAX_PATH + 1];

  if (!io_path)
    if (fd == STDIN_FILENO)
      {
	if (!(io_path = strdup (DEF_STDIN_PATH)))
	  return ERROR_OUTOFMEMORY;
      }
    else
      {
	strcat (strcat (strcpy (buf, DEF_LOG_PATH), svc_name), ".log");
	if (!(io_path = (char *) malloc (strlen (buf) + 1)))
	  return ERROR_OUTOFMEMORY;
        strcpy (io_path, buf);
      }
  /* convert relative paths to absolute ones, relative to DEF_LOG_PATH. */
  if (io_path[0] != '/' && io_path[1] != ':')
    {
      if (strlen (DEF_LOG_PATH) + strlen (io_path) > MAX_PATH)
        return ERROR_INVALID_PARAMETER;
      strcat (strcpy (buf, DEF_LOG_PATH), io_path);
      free (io_path);
      if (!(io_path = (char *) malloc (strlen (buf) + 1)))
	return ERROR_OUTOFMEMORY;
      strcpy (io_path, buf);
    }
  return ERROR_SUCCESS;
}

/* Retrieves the subkeys of the service registry key to determine what
   application to start from `service_main'. */
int
get_reg_entries (const char *name, char *&path, char *&args, char *&dir,
		 env_t *&env, DWORD *termsig_p,
		 char *&stdin_path, char *&stdout_path, char *&stderr_path,
		 char *&pidfile_path, DWORD *neverexits_p,
		 DWORD *shutdown_p, DWORD *interactive_p, DWORD *showcons_p)
{
  HKEY srv_key = NULL;
  HKEY env_key = NULL;
  DWORD type, size;
  char param_key[MAX_PATH];
  int ret;

  strcat (strcat (strcpy (param_key, SRV_KEY), name), PARAM_KEY);
  if ((ret = RegOpenKeyEx (HKEY_LOCAL_MACHINE, param_key, 0,
  			   KEY_READ, &srv_key)) != ERROR_SUCCESS)
    goto out;
  /* Get path. */
  if ((ret = RegQueryValueEx (srv_key, PARAM_PATH, 0, &type,
                              NULL, (size = 0, &size))) != ERROR_SUCCESS)
    goto out;
  if (!(path = (char *) malloc (size)))
    {
      ret = ERROR_OUTOFMEMORY;
      goto out;
    }
  if ((ret = RegQueryValueEx (srv_key, PARAM_PATH, 0, &type,
                              (BYTE *) path, &size)) != ERROR_SUCCESS)
    goto out;
  /* Get (optional) args. */
  if ((ret = get_opt_string_entry (srv_key, PARAM_ARGS, args)))
    goto out;
  /* Get (optional) dir. */
  if ((ret = get_opt_string_entry (srv_key, PARAM_DIR, dir)))
    goto out;
  /* Get (optional) pid file. */
  if ((ret = get_opt_string_entry (srv_key, PARAM_PIDFILE, pidfile_path)))
    goto out;
  /* Get (optional) termination signal. */
  if (RegQueryValueEx (srv_key, PARAM_TERMSIG, 0, &type,
		       (BYTE *) termsig_p,
		       (size = sizeof(*termsig_p), &size)) != ERROR_SUCCESS)
    *termsig_p = SIGTERM;  // the default
  /* Get (optional) neverexits flag. */
  if (RegQueryValueEx (srv_key, PARAM_NEVEREXITS, 0, &type,
		       (BYTE *) neverexits_p,
		       (size = sizeof(*neverexits_p), &size)) != ERROR_SUCCESS)
    *neverexits_p = 0;  // the default
  /* Get (optional) shutdown flag. */
  if (RegQueryValueEx (srv_key, PARAM_SHUTDOWN, 0, &type,
		       (BYTE *) shutdown_p,
		       (size = sizeof(*shutdown_p), &size)) != ERROR_SUCCESS)
    *shutdown_p = 0;  // the default
  /* Get (optional) interactive flag. */
  if (RegQueryValueEx (srv_key, PARAM_INTERACT, 0, &type,
		       (BYTE *) interactive_p,
		       (size = sizeof(*interactive_p), &size)) != ERROR_SUCCESS)
    *interactive_p = 0;  // the default
  /* Get (optional) show console flag. */
  if (RegQueryValueEx (srv_key, PARAM_SHOWCONS, 0, &type,
		       (BYTE *) showcons_p,
		       (size = sizeof(*showcons_p), &size)) != ERROR_SUCCESS)
    *showcons_p = 0;  // the default
  /* Get (optional) stdin/stdout/stderr redirection files. */
  if ((ret = get_opt_string_entry (srv_key, PARAM_STDIN, stdin_path)))
    goto out;
  else if (reeval_io_path (STDIN_FILENO, stdin_path, name))
    goto out;
  if ((ret = get_opt_string_entry (srv_key, PARAM_STDOUT, stdout_path)))
    goto out;
  else if (reeval_io_path (STDOUT_FILENO, stdout_path, name))
    goto out;
  if ((ret = get_opt_string_entry (srv_key, PARAM_STDERR, stderr_path)))
    goto out;
  else if (reeval_io_path (STDERR_FILENO, stderr_path, name))
    goto out;
  /* Get (optional) environment strings. */
  strcat (strcat (param_key, "\\"), PARAM_ENVIRON);
  if ((ret = RegOpenKeyEx (HKEY_LOCAL_MACHINE, param_key, 0,
  			   KEY_READ, &env_key)) == ERROR_SUCCESS)
    {
      /* Max. size of value names is 16K since NT 5.1 (XP), 260 otherwise. */
      char kname[16384];
      char *kvalue;
      DWORD type, ksize;
      int idx;

      if (!(env = (env_t *) calloc (MAX_ENV + 1,  sizeof (env_t))))
	{
	  ret = ERROR_OUTOFMEMORY;
	  goto out;
	}
      idx = 0;
      for (int i = 0; i <= MAX_ENV + 2; ++i)
	{
	  ret = RegEnumValue (env_key, i, kname, (size = 16384, &size), 0,
			      &type, NULL, (ksize = 0, &ksize));
	  if (ret == ERROR_NO_MORE_ITEMS)
	    break;
	  if (ret != ERROR_SUCCESS && ret != ERROR_MORE_DATA)
	    goto out;
	  if (type != REG_SZ)
	    continue;
	  if (!(kvalue = (char *) malloc (ksize)))
	    {
	      ret = ERROR_OUTOFMEMORY;
	      goto out;
	    }
	  ret = RegEnumValue (env_key, i, kname, (size = 16384, &size), 0,
			      NULL, (BYTE *) kvalue, &ksize);
	  if (ret != ERROR_SUCCESS)
	    goto out;
	  if (!(env[idx].name = strdup (kname)))
	    {
	      ret = ERROR_OUTOFMEMORY;
	      goto out;
	    }
	  env[idx].val = kvalue;
	  ++idx;
	}
      if (!idx)
	{
	  free (env);
	  env = NULL;
	}
    }
  if (args && !strlen (args))
    {
      free (args);
      args = NULL;
    }
  ret = 0;

out:
  if (ret)
    {
      path = args = NULL;
      env = NULL;
    }
  if (env_key)
    RegCloseKey (env_key);
  if (srv_key)
    RegCloseKey (srv_key);
  return ret;
}

/* Retrieves the description of the service.  Note: it would be so much
   cleaner to do this by a simple call to QueryServiceConfig2(), but alas this
   does not exist in NT4.  *sigh*  */
int
get_description (const char *name, char *&descr)
{
  HKEY desc_key = NULL;
  char desc_key_path[MAX_PATH];
  int ret;

  strcat (strcpy (desc_key_path, SRV_KEY), name);
  if ((ret = RegOpenKeyEx (HKEY_LOCAL_MACHINE, desc_key_path, 0,
  			   KEY_READ, &desc_key)) != ERROR_SUCCESS)
    goto out;
  
  if ((ret = get_opt_string_entry (desc_key, DESC, descr)))
    goto out;

  ret = 0;

out:
  if (ret)
    descr = NULL;
  if (desc_key)
    RegCloseKey (desc_key);
  return ret;
}



int
add_env_var (char *envstr, env_t *&env)
{
  if (!envstr)
    return error (InstallErr, "NULL environment string not allowed");
  if (!env && !(env = (env_t *) calloc (MAX_ENV + 1,  sizeof (env_t))))
    return error (InstallErr, "Out of memory");
  char *name = envstr;
  char *value = strchr (envstr, '=');
  if (!value)
    return error (InstallErr, "Malformed environment string");
  *value++ = '\0';
  for (int i = 0; i <= MAX_ENV; ++i)
    if (!env[i].name)
      {
        env[i].name = name;
	env[i].val = value;
	return 0;
      }
    else if (!strcmp (env[i].name, name))
      return 0;
  return error (InstallErr, "Only " MAX_ENV_STR
  			    " environment variables allowed");
}

int
add_dep(char *service_name, char **&deps)
{
  if (!service_name)
    return error(InstallErr, "NULL dependency name not allowed");
  if (!deps && !(deps = (char **) calloc(MAX_DEPS + 1, sizeof(char *))))
    return error(InstallErr, "Out of memory");
  for (int i = 0; i <= MAX_DEPS; ++i)
    if (! deps[i])
      {
	if (! (deps[i] = strdup(service_name)))
	  return error(InstallErr, "Out of memory");
	return 0;
      }
  return error(InstallErr, "Only " MAX_DEPS_STR " dependency values allowed");
}

/* The strings `path' and `args' are evaluated to an arglist which
   can be used as `argv' argument to execv. It's splitted at the
   spaces. One level of " or ' quotes are supported as well. */

char *arglistbuf[1024];

int
eval_arglist (const char *path, char *args, char **&arglist)
{
  char **tmp, stopchar;
  int count = 2;

  arglist = (char **) malloc (count * sizeof (char *));
  if (!arglist)
    return ERROR_OUTOFMEMORY;
  arglist[0] = (char *) path;
  arglist[1] = NULL;
  if (!args)
    return 0;
  for (char *c = args; *c; ++c)
    if (*c != ' ')
      {
	if (*c == '\'' || *c == '"')
	  {
	    stopchar = *c;
	    ++c;
	  }
	else
	  stopchar = ' ';
	if (!(tmp = (char **) realloc (arglist, ++count * sizeof (char *))))
	  return ERROR_OUTOFMEMORY;
	arglist = tmp;
	arglist[count - 2] = c;
	arglist[count - 1] = NULL;
	while (*c && *c != stopchar)
	  ++c;
	if (*c)
	  *c = '\0';
	else
	  --c;
      }
  return 0;
}

/* Checks the mount table for /usr/bin mounted in user mode and warns if found. */
void
check_system_mounts ()
{
  FILE *m = setmntent ("/-not-used-", "r");
  struct mntent *p;

  while ((p = getmntent (m)) != NULL)
    {
      if (stricmp (p->mnt_dir, "/usr/bin") == 0
          && p->mnt_type[0] == 'u')
        {
          fprintf (stderr, "%s",
      "*******************************************************************\n"
      "* Warning: User mode mounts detected.  This will likely cause     *\n"
      "* the service to fail to start.  You must have system mode mounts *\n"
      "* in order to run services.  Re-run setup and choose 'All Users'  *\n"
      "* or consult the FAQ for more information.                        *\n"
      "*******************************************************************\n");
          break;
        }
    }
  endmntent (m);
}


/* Installs cygrunsrv as service `name' with display name `disp'. */
int
install_service (const char *name, const char *disp, type_t type,
		 char *user, char *pass, char **deps, int interactive)
{
  char mypath[MAX_PATH];
  SC_HANDLE sm = (SC_HANDLE) 0;
  SC_HANDLE sh = (SC_HANDLE) 0;
  char userbuf[INTERNET_MAX_HOST_NAME_LENGTH + UNLEN + 2];
  char *username = NULL;
  char *dependencies = NULL;
  char *err_func;
  DWORD err = 0;

  check_system_mounts ();

  /* Get own full path. */
  if (!GetModuleFileName (NULL, mypath, MAX_PATH))
    err_out (GetModuleFileName);
  /* Open service manager database. */
  if (!(sm = OpenSCManager (NULL, NULL,
			    SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE)))
    err_out (OpenSCManager);
  /* Open service, go ahead if service doesn't exists. */
  if (!(sh = OpenService (sm, name, SERVICE_ALL_ACCESS)) &&
      GetLastError() != ERROR_SERVICE_DOES_NOT_EXIST)
    err_out (OpenService);
  if (sh)
    err_out_set_error (OpenService, ERROR_SERVICE_EXISTS);
  /* Set optional dependencies. */
  if (deps)
    {
      int concat_length = 0;
      for (int i = 0; i < MAX_DEPS && deps[i]; i++)
	concat_length += (strlen(deps[i]) + 1);
      concat_length++;
      if (! (dependencies = (char *) malloc(concat_length)))
	err_out_set_error (malloc, ERROR_OUTOFMEMORY);
      char *p = dependencies;
      for (int i = 0; i < MAX_DEPS && deps[i]; i++)
	{
	  strcpy(p, deps[i]);
	  p += (strlen(deps[i]) + 1);
	}
      *p = '\0';
      /* dependencies now holds the concatenation of all the
         dependent service names, each terminated by a null and the
         whole thing terminated by a final null. */
    }
  /* Check username. */
  if (user)
    {
      /* Check if user is in /etc/passwd. */
      struct passwd *pw = getpwnam (user);

      /* If yes, try to extract an U- entry, if any.
         Otherwise treat user as the windows user name. */
      if (pw && pw->pw_gecos)
        {
	  char *c;

	  if ((c = strstr (pw->pw_gecos, "U-")) != NULL &&
	      (c == pw->pw_gecos || c[-1] == ','))
	    {
	      userbuf[0] = '\0';
	      strncat (userbuf, c + 2,
	      	       INTERNET_MAX_HOST_NAME_LENGTH + UNLEN + 1);
	      if ((c = strchr (userbuf, ',')) != NULL)
	        *c = '\0';
	      user = userbuf;
	    }
	}
      if (!(username = (char *) malloc (strlen (user) + 3)))
        err_out_set_error (malloc, ERROR_OUTOFMEMORY);
      /* If no "\" is part of the name, prepend ".\" */
      if (!strchr (user, '\\'))
        strcat (strcpy (username, ".\\"), user);
      else
        strcpy (username, user);
    }
  /* If a username other than .\System is given but no password, ask for it. */
  char buf[128];
  if (username && strcasecmp (username, ".\\System") && !pass)
    {
      while (!pass)
        {
	  char *p;
	  sprintf (buf, "Enter password of user `%s': ", username);
	  p = getpass (buf);
	  strcpy (buf, p);
	  p = getpass ("Reenter, please: ");
	  if (!strcmp (buf, p))
	    pass = buf;
	  else
	    printf ("Sorry, passwords do not match.\n");
	  while (p && *p)
	    *p++ = '\0';
	}
    }
  /* Try to create service. */
  if (!(sh = CreateService (sm, name, disp, SERVICE_ALL_ACCESS,
			    SERVICE_WIN32_OWN_PROCESS |
			    (interactive ? SERVICE_INTERACTIVE_PROCESS : 0),
			    type == Auto ? SERVICE_AUTO_START
			    		 : SERVICE_DEMAND_START,
			    SERVICE_ERROR_NORMAL, mypath, NULL, NULL,
			    dependencies,
			    username, username ? pass ?: "" : NULL)))
    err_out (CreateService);

out:
  if (pass == buf)
    while (*pass)
      *pass++ = '\0';
  if (sh)
    CloseServiceHandle (sh);
  if (sm)
    CloseServiceHandle (sm);
  return err == 0 ? 0 : error (InstallErr, err_func, err);
}

/* Remove service `name'. */
int
remove_service (const char *name)
{
  SC_HANDLE sm = (SC_HANDLE) 0;
  SC_HANDLE sh = (SC_HANDLE) 0;
  SERVICE_STATUS ss;
  int cnt = 0;
  char *err_func;
  DWORD err = 0;

  /* Open service manager database. */
  if (!(sm = OpenSCManager (NULL, NULL,
  			    SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE)))
    err_out (OpenSCManager);
  /* Check whether service exists. */
  if (!(sh = OpenService (sm, name, SERVICE_ALL_ACCESS)))
    err_out (OpenService);
  /* Try stopping service.
     If CANNOT_ACCEPT_CTRL wait and try again up to 30secs. */
  while (!ControlService (sh, SERVICE_CONTROL_STOP, &ss) &&
         (err = GetLastError ()) == ERROR_SERVICE_CANNOT_ACCEPT_CTRL &&
	 ++cnt < 30)
    sleep (1);
  /* Any error besides ERROR_SERVICE_NOT_ACTIVE is treated as an error. */
  if (err && err != ERROR_SERVICE_NOT_ACTIVE)
    err_out (ControlService);
  /* If service is active, wait until it's stopped. */
  if (!err)
    {
      if (!QueryServiceStatus (sh, &ss))
	err_out (QueryServiceStatus);

      DWORD last_check = ss.dwCheckPoint;
      DWORD last_tick = GetTickCount ();

      while (ss.dwCurrentState == SERVICE_STOP_PENDING)
        {
	  Sleep (eval_wait_time (ss.dwWaitHint));
	  if (!QueryServiceStatus (sh, &ss))
	    err_out (QueryServiceStatus);
	  if (ss.dwCurrentState == SERVICE_STOP_PENDING)
	    {
	      if (ss.dwCheckPoint > last_check)
		{
		  last_check = ss.dwCheckPoint;
		  last_tick = GetTickCount ();
		}
	      else if (GetTickCount() - last_tick > ss.dwWaitHint)
		{
		  SetLastError (ERROR_SERVICE_REQUEST_TIMEOUT);
		  err_out (QueryServiceStatus);
		}
	    }
        }
    }
  err = 0;
  /* Now try to remove service. */
  if (!DeleteService (sh))
    err_out (DeleteService);

out:
  if (sh)
    CloseServiceHandle (sh);
  if (sm)
    CloseServiceHandle (sm);
  return err == 0 ? 0 : error (RemoveErr, err_func, err);
}

/* Start service `name'. */
int
start_service (const char *name)
{
  SC_HANDLE sm = (SC_HANDLE) 0;
  SC_HANDLE sh = (SC_HANDLE) 0;
  SERVICE_STATUS ss;
  int cnt = 0;
  DWORD last_check;
  DWORD last_tick;
  char *err_func;
  DWORD err = 0;

  /* Open service manager database. */
  if (!(sm = OpenSCManager (NULL, NULL, SC_MANAGER_CONNECT)))
    err_out (OpenSCManager);
  /* Check whether service exists. */
  if (!(sh = OpenService (sm, name, SERVICE_START | SERVICE_QUERY_STATUS)))
    err_out (OpenService);
  /* Start service. */
  if (!StartService (sh, 0, NULL) &&
      GetLastError () != ERROR_SERVICE_ALREADY_RUNNING)
    err_out (StartService);
  /* Wait until start was successful. */
  if (!QueryServiceStatus (sh, &ss))
    err_out (QueryServiceStatus);

  last_check = ss.dwCheckPoint;
  last_tick = GetTickCount ();

  while (ss.dwCurrentState == SERVICE_START_PENDING)
    {
      Sleep (eval_wait_time (ss.dwWaitHint));
      if (!QueryServiceStatus (sh, &ss))
	err_out (QueryServiceStatus);
      if (ss.dwCurrentState == SERVICE_START_PENDING)
	{
	  if (ss.dwCheckPoint > last_check)
	    {
	      last_check = ss.dwCheckPoint;
	      last_tick = GetTickCount ();
	    }
	  else if (GetTickCount() - last_tick > ss.dwWaitHint)
	    err_out_set_error (QueryServiceStatus, ERROR_SERVICE_REQUEST_TIMEOUT);
	}
    }

  if (ss.dwCurrentState != SERVICE_RUNNING)
    err_out_set_error (QueryServiceStatus, ERROR_SERVICE_NOT_ACTIVE);

out:
  if (sh)
    CloseServiceHandle (sh);
  if (sm)
    CloseServiceHandle (sm);
  return err == 0 ? 0 : error (StartErr, err_func, err);
}

/* Stop service `name'. */
int
stop_service (const char *name)
{
  SC_HANDLE sm = (SC_HANDLE) 0;
  SC_HANDLE sh = (SC_HANDLE) 0;
  SERVICE_STATUS ss;
  int cnt = 0;
  char *err_func;
  DWORD err = 0;

  /* Open service manager database. */
  if (!(sm = OpenSCManager (NULL, NULL, SC_MANAGER_CONNECT)))
    err_out (OpenSCManager);
  /* Check whether service exists. */
  if (!(sh = OpenService (sm, name, SERVICE_STOP | SERVICE_QUERY_STATUS)))
    err_out (OpenService);
  /* Try stopping service.
     If CANNOT_ACCEPT_CTRL wait and try again up to 30secs. */
  while (!ControlService (sh, SERVICE_CONTROL_STOP, &ss) &&
         (err = GetLastError ()) == ERROR_SERVICE_CANNOT_ACCEPT_CTRL &&
	 ++cnt < 30)
    sleep (1);
  /* Any error besides ERROR_SERVICE_NOT_ACTIVE is treated as an error. */
  if (err && err != ERROR_SERVICE_NOT_ACTIVE)
    err_out (ControlService);
  /* If service is active, wait until it's stopped. */
  if (!err)
    {
      if (!QueryServiceStatus (sh, &ss))
	err_out (QueryServiceStatus);

      DWORD last_check = ss.dwCheckPoint;
      DWORD last_tick = GetTickCount ();

      while (ss.dwCurrentState == SERVICE_STOP_PENDING)
        {
	  Sleep (eval_wait_time (ss.dwWaitHint));
	  if (!QueryServiceStatus (sh, &ss))
	    err_out (QueryServiceStatus);
	  if (ss.dwCurrentState == SERVICE_STOP_PENDING)
	    {
	      if (ss.dwCheckPoint > last_check)
		{
		  last_check = ss.dwCheckPoint;
		  last_tick = GetTickCount ();
		}
	      else if (GetTickCount() - last_tick > ss.dwWaitHint)
	        err_out_set_error (QueryServiceStatus, ERROR_SERVICE_REQUEST_TIMEOUT);
	    }
        }
    }
  err = 0;

out:
  if (sh)
    CloseServiceHandle (sh);
  if (sm)
    CloseServiceHandle (sm);
  return err == 0 ? 0 : error (StopErr, err_func, err);
}

/* these are used to turn DWORDs into desciptive text */
static struct desc_type { bool bitwise; DWORD flag; const char *meaning; } 

/* Note: service installed with --interactive will have
   (SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS) so this
   needs to have bitmask = true.  */
ServiceType_desc[] =
{
  {true, 0,                                    "(Unknown)"},
  {true, SERVICE_WIN32_OWN_PROCESS,            "Own Process, "},
  {true, SERVICE_WIN32_SHARE_PROCESS,          "Shared Process, "},
  {true, SERVICE_KERNEL_DRIVER,                "Kernel Driver, "},
  {true, SERVICE_FILE_SYSTEM_DRIVER,           "Filesystem Driver, "},
  {true, SERVICE_INTERACTIVE_PROCESS,          "Interactive, "},
  {true, 0, NULL}
},

StartType_desc[] =
{
  {false, 0,                                   "(Unknown)"},
  {false, SERVICE_BOOT_START,                  "Boot"},
  {false, SERVICE_SYSTEM_START,                "System"},
  {false, SERVICE_AUTO_START,                  "Automatic"},
  {false, SERVICE_DISABLED,                    "Disabled"},
  {false, SERVICE_DEMAND_START,                "Manual"},
  {false, 0, NULL}
},

CurrentState_desc[] =
{
  {false, 0,                                   "(Unknown)"},
  {false, SERVICE_STOPPED,                     "Stopped"},
  {false, SERVICE_START_PENDING,               "Start Pending"},
  {false, SERVICE_STOP_PENDING,                "Stop Pending"}, 
  {false, SERVICE_RUNNING,                     "Running"},  
  {false, SERVICE_CONTINUE_PENDING,            "Continue Pending"},
  {false, SERVICE_PAUSE_PENDING,               "Pause Pending"},
  {false, SERVICE_PAUSED,                      "Paused"}, 
  {false, 0, NULL}
},

ControlsAccepted_desc[] = 
{
  {true, 0,                                    "(none)"},
  {true, SERVICE_ACCEPT_STOP,                  "Stop, "},
  {true, SERVICE_ACCEPT_SHUTDOWN,              "Shutdown, "},
  {true, SERVICE_ACCEPT_PAUSE_CONTINUE,        "Pause, Continue, "},
  {true, SERVICE_ACCEPT_PARAMCHANGE,           "Change Parameters, "},
  {true, SERVICE_ACCEPT_NETBINDCHANGE,         "Change Network Binding, "},
  {true, SERVICE_ACCEPT_HARDWAREPROFILECHANGE, "Hardware Profile Change Notify, "},
  {true, SERVICE_ACCEPT_POWEREVENT,            "Power Status Change Notify, "},
  {true, SERVICE_ACCEPT_SESSIONCHANGE,         "Session Status Change Notify, "},
  {true, 0, NULL }
};


/* Passed one of the above static arrays and a DWORD, this returns a 
   pointer to a descriptive string: either a concatenation of matching
   items (if bitwise is true) otherwise just the matching item.  */
const char *
make_desc(struct desc_type *desc, DWORD thing)
{
  static char buf[256];

  if (desc[0].bitwise)
    {
      buf[0] = '\0';
      for (int i = 1; desc[i].meaning; i++)
        if (thing & desc[i].flag)
          strcat (buf, desc[i].meaning);
  
      char *ptr = strchr (buf, '\0');
      if (ptr - buf > 2 && ptr[-1] == ' ' && ptr[-2] == ',')
        {
          ptr[-2] = '\0';               /* remove tailing ", " */
          return (buf);
        }
    }
  else
    for (int i = 1; desc[i].meaning; i++)
      if (thing == desc[i].flag)
        return (desc[i].meaning);

  return desc[0].meaning;               /* default value */        
}

/* Passed a pointer to a double-NULL terminated list of strings, this 
   returns a formatted list of those items, each delimited by `delim'.  */
char *
parsedoublenull (const char *input, const char *delim)
{
  char *base, *end;
  static char buf[256];
  int used = 0, dsiz = strlen (delim);
  
#define parsedoublenull_done (*end == 0 && *base == 0)

  for (buf[0] = 0, base = end = (char *) input; !parsedoublenull_done; end++)
    if (*end == 0)
      {
        if ((used += ((end - base) + dsiz)) >= sizeof(buf))
          break;     /* don't overflow */
        strcat (buf, base);
        base = end + 1;
        if (!parsedoublenull_done)
          strcat (buf, delim);
      }
  return buf;
}

/* Passed the name, opened handle, and status information about a service, 
   this formats all the information and outputs it to stdout.  */
void
print_service (const char *name, SC_HANDLE &sh, SERVICE_STATUS &ss, 
               QUERY_SERVICE_CONFIG *qsc, bool verbose)
{
  char *descrip = NULL, *path = NULL, *args = NULL, *dir = NULL, 
       *stdin_path = NULL, *stdout_path = NULL, *stderr_path = NULL,
       *pidfile_path = NULL;
  DWORD termsignal, neverex, shutd, interact, showc;
  env_t *env = NULL;

#define P(x, y) printf ("%-20s: %s\n", x, y)

  P("Service", name);
  if (strcmp (name, qsc->lpDisplayName))
    P("Display name", qsc->lpDisplayName);
  if (!get_description (name, descrip) && descrip && strlen (descrip))
    P("Description", descrip);
    
  P("Current State", make_desc(CurrentState_desc, ss.dwCurrentState));
  if (ss.dwControlsAccepted)
    P("Controls Accepted", make_desc(ControlsAccepted_desc, ss.dwControlsAccepted));

  /* Get the cygrunsrv-specific things from the registry. */
  if (get_reg_entries (name, path, args, dir, env, &termsignal,
		            stdin_path, stdout_path, stderr_path, pidfile_path,
			    &neverex, &shutd, &interact, &showc))
    return;  /* bail on error */

  printf ("%-20s: %s", "Command", path);
  if (args)
    printf (" %s\n", args);
  else
    fputc ('\n', stdout);

  if (verbose)
    {      
      if (dir)
        P("Working Dir", dir);
      if (stdin_path)
        P("stdin path", stdin_path);
      if (stdout_path)
        P("stdout path", stdout_path);
      if (stderr_path)
        P("stderr path", stderr_path);
      if (pidfile_path)
        P("pidfile path", pidfile_path);
        
      char tmp[128] = {0};
      if (neverex)
        strcat (tmp, "--neverexits ");
      if (shutd)
        strcat (tmp, "--shutdown ");
      if (interact)
        strcat (tmp, "--interactive ");
      if (showc)
        strcat (tmp, "--nohide ");
      if (strlen(tmp))
        P("Special flags", tmp);
      
      if (termsignal != SIGTERM)
        P("Termination Signal", strsignal (termsignal));

      if (env)
        {
          printf ("%-20s: ", "Environment");
          for (int i = 0; i <= MAX_ENV && env[i].name; ++i)
            printf ("%s=\"%s\" ", env[i].name, env[i].val);
          fputc ('\n', stdout);
        }
      
      P("Process Type", make_desc(ServiceType_desc, ss.dwServiceType));
      P("Startup", make_desc(StartType_desc, qsc->dwStartType)); 
      if (qsc->lpDependencies && strlen (qsc->lpDependencies))
        P("Dependencies", parsedoublenull(qsc->lpDependencies, ", "));
      if (qsc->lpServiceStartName)
        P("Account", qsc->lpServiceStartName);
    }

#undef P
  fputc ('\n', stdout);
}

/* According to the platform SDK, the maximum size that a QUERY_SERVICE_CONFIG
   buffer need be is 8kb.  */
#define QSC_BUF_SIZE 8192

/* Query service `name'. */
int
query_service (const char *name, bool verbose)
{
  SC_HANDLE sm = (SC_HANDLE) 0;
  SC_HANDLE sh = (SC_HANDLE) 0;
  SERVICE_STATUS ss;
  QUERY_SERVICE_CONFIG *qsc_buf = NULL;
  int cnt = 0;
  char *err_func;
  DWORD err = 0, bytes_needed;

  /* Allocate qsc buffer. */
  if ((qsc_buf = (QUERY_SERVICE_CONFIG *) malloc (QSC_BUF_SIZE)) == NULL)
    err_out_set_error (malloc, ERROR_OUTOFMEMORY);

  /* Open service manager database. */
  if (!(sm = OpenSCManager (NULL, NULL, SC_MANAGER_CONNECT)))
    err_out (OpenSCManager);

  /* Check whether service exists. */
  if (!(sh = OpenService (sm, name,  SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG)))
    err_out (OpenService);

  /* Get the current status of the service. */
  if (!QueryServiceStatus(sh, &ss))
    err_out (QueryServiceStatus);

  /* Get configuration info about the service. */
  if (!QueryServiceConfig (sh, qsc_buf, QSC_BUF_SIZE, &bytes_needed))
    err_out (QueryServiceConfig);

  print_service (name, sh, ss, qsc_buf, verbose);
   
  err = 0;

out:
  if (sh)
    CloseServiceHandle (sh);
  if (sm)
    CloseServiceHandle (sm);
  if (qsc_buf)
    free (qsc_buf);
  return err == 0 ? 0 : error (QueryErr, err_func, err);
}

/* Returns true if the two file/path names end in the same filename */
bool
same_filename (const char *a, const char *b)
{
  char a_buf[MAX_PATH + 1], b_buf[MAX_PATH + 1];
  
  strcpy (a_buf, basename ((char *) a));
  strcpy (b_buf, basename ((char *) b));
  return !stricmp (a_buf, b_buf);
}

/* Iterates through all services and reports on 
   those that are cygrunsrv-managed.  */
int
list_services (bool verbose)
{
  char mypath[MAX_PATH];
  SC_HANDLE sm = (SC_HANDLE) 0;
  SC_HANDLE sh = (SC_HANDLE) 0;
  ENUM_SERVICE_STATUS *srv_buf = NULL;
  QUERY_SERVICE_CONFIG *qsc_buf = NULL;
  SERVICE_STATUS ss;
  DWORD bytes_needed, num_services, resume_handle = 0;
  char *err_func;
  DWORD err = 0;

  /* Get our own filename so that we can tell which services are ours.  */
  if (!GetModuleFileName (NULL, mypath, MAX_PATH))
    err_out (GetModuleFileName);
  
  /* This buffer will be used for querying the details of a service.  */
  if ((qsc_buf = (QUERY_SERVICE_CONFIG *) malloc (QSC_BUF_SIZE)) == NULL)
    err_out_set_error (malloc, ERROR_OUTOFMEMORY);
    
  /* Open service manager database.  */
  if (!(sm = OpenSCManager (NULL, NULL, SC_MANAGER_CONNECT | 
                                        SC_MANAGER_ENUMERATE_SERVICE)))
    err_out (OpenSCManager);

  /* First call with lpServices to NULL to get length of needed buffer.  */
  if (EnumServicesStatus (sm, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0,
                          &bytes_needed, &num_services, &resume_handle) != 0)
    err_out (EnumServiceStatus);
    
  if ((srv_buf = (ENUM_SERVICE_STATUS *) malloc (bytes_needed)) == NULL) 
    err_out_set_error (malloc, ERROR_OUTOFMEMORY);

  /* Call the function for real this time with the allocated buffer.
     FIXME: In theory this should be a while loop that checks for ERROR_MORE_DATA
     and continues fetching the remaining records.  However, we'll just trust
     that the value returned above in bytes_needed was sufficient to get all
     records in a single pass.  */
  if (!EnumServicesStatus (sm, SERVICE_WIN32, SERVICE_STATE_ALL, srv_buf, 
                  bytes_needed, &bytes_needed, &num_services, &resume_handle))
    err_out (EnumServiceStatus);

  for (int i = 0; i < num_services; i++)
    {
      /* get details of this service and see if it's one of ours.  */
      if (!(sh = OpenService (sm, srv_buf[i].lpServiceName, GENERIC_READ)))
        err_out (OpenService);
      
      if (!QueryServiceConfig (sh, qsc_buf, QSC_BUF_SIZE, &bytes_needed))
        err_out (QueryServiceConfig);
      
      /* is this us? */
      if (same_filename (qsc_buf->lpBinaryPathName, mypath))
        {          
          if (!verbose)
            printf ("%s\n", srv_buf[i].lpServiceName);
          else
            {
              if (!QueryServiceStatus(sh, &ss))
                err_out (QueryServiceStatus);

              print_service (srv_buf[i].lpServiceName, sh, ss, qsc_buf, verbose);
            }
        }
      CloseServiceHandle (sh);
    }

  //fputc ('\n', stdout);  
  err = 0;

out:
  if (qsc_buf)
    free (qsc_buf);
  if (srv_buf)
    free (srv_buf);
  if (sh)
    CloseServiceHandle (sh);
  if (sm)
    CloseServiceHandle (sm);
  return err == 0 ? 0 : error (ListErr, err_func, err);
}

#undef err_out
#undef err_out_set_error

int server_pid;

/* What really is to do when service should be stopped. */
int
terminate_child ()
{
  set_service_status (SERVICE_STOP_PENDING, 1, 21000L);
  for (int i = 0; i < 20; ++i)
    if (!server_pid)
      sleep (1);
    else
      {
	termsig_sent = true;
	kill (server_pid, termsig);
	report_service_status ();
	return 0;
      }
  set_service_status (SERVICE_RUNNING);
  return -1;
}

void
sigterm_handler (int sig)
{
  shutting_down = 1;
  syslog (LOG_INFO, "Received signal `%d', terminate child...", sig);
  terminate_child ();
}

/* Service handler routine which is called from SCM to check status,
   stop the service, etc. */
VOID WINAPI
service_handler (DWORD ctrl)
{
  switch (ctrl)
    {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
      /* Since the service_handler doesn't run in the same thread as
         the service_main routine, it has to setup exception handling. */
      exception_list except_list;
      cygwin_internal (CW_INIT_EXCEPTIONS, &except_list);

      terminate_child ();
      break;
    case SERVICE_CONTROL_INTERROGATE:
      break;
    }
  report_service_status (false);
}

/* Used by the service_main thread. */
exception_list except_list;

/* Various initialization task. */
int
prepare_daemon ()
{
  /* Initialize exception handling. */
  cygwin_internal (CW_INIT_EXCEPTIONS, &except_list);
  /* Set up signal handler. */
  signal (SIGQUIT, sigterm_handler);
  signal (SIGINT, sigterm_handler);
  signal (SIGHUP, sigterm_handler);
  signal (SIGTERM, sigterm_handler);

  return 0;
}

int
read_pidfile (const char * path, time_t forktime)
{
  /* ignore old files */
  if (forktime > 0)
    {
      struct stat st;
      if (!stat(path, &st) && st.st_mtime <= forktime)
        return -1;
    }
  FILE * f = fopen(path, "r");
  if (!f)
    return -1;
  int pid = -1;
  fscanf(f, "%d", &pid);
  fclose(f);
  return pid;
}


/* Service main routine. Actual worker. Starts and controls application 
   given in the service registry subkeys . */
void WINAPI
service_main (DWORD argc, LPSTR *argv)
{
  DWORD err;
  SERVICE_STATUS_HANDLE ssh;
  svcname = argv[0];

  /* Initialize syslog facility. */
  openlog (svcname, LOG_PID | LOG_NOWAIT, LOG_DAEMON);

  /* Tell SCM that we are alive and well. */
  if (!(ssh = RegisterServiceCtrlHandlerA (svcname, service_handler)))
    {
      err = GetLastError ();
      syslog_starterr ("RegSrvCtrlHandler", GetLastError ());
      return;
    }

  init_service_status (ssh);

  report_service_status ();

  if (err = prepare_daemon ())
    {
      syslog_starterr ("prepare_daemon", err);
      set_service_status (SERVICE_STOPPED, 0, 0, err);
      return;
    }

  report_service_status ();

  /* Step 1: Get information from registry. */
  char *path = NULL;
  char *args = NULL;
  char *dir = NULL;
  env_t *env = NULL;
  char *stdin_path = NULL;
  char *stdout_path = NULL;
  char *stderr_path = NULL;
  char *pidfile_path = NULL;
  if (err = get_reg_entries (svcname, path, args, dir, env, &termsig,
			     stdin_path, stdout_path, stderr_path,
			     pidfile_path, &neverexits, &shutdown,
			     &interactive, &showcons))
    {
      syslog_starterr ("get_reg_entries", err);
      set_service_status (SERVICE_STOPPED, 0, 0, err);
      return;
    }
  
  set_service_controls_accepted (shutdown, interactive);
  /* Create console window only in --nohide case.  Free old, invisible
     console at this point. */
  if (showcons)
    {
      /* Try to maintain window focus and Z-order. */
      HWND top = GetTopWindow (NULL);
      HWND focus = GetForegroundWindow ();
      FreeConsole ();
      AllocConsole ();
      SetForegroundWindow (focus);
      BringWindowToTop (top);
      char title[256];
      snprintf (title, 256, "%s (pid %d)", svcname, getpid ());
      SetConsoleTitle (title);
    }

  report_service_status ();

  /* Step 2: Further preparations:
     - Evaluate arguments to a list for execv
     - Add environment strings from registry to environment
     - Add /bin to $PATH so that always cygwin1.dll is in the path. */
  char **arglist;
  if (err = eval_arglist (path, args, arglist))
    {
      syslog_starterr ("eval_arglist", err);
      set_service_status (SERVICE_STOPPED, 0, 0, err);
      return;
    }

  report_service_status ();

  if (env)
    for (int i = 0; i <= MAX_ENV && env[i].name; ++i)
      setenv (env[i].name, env[i].val, 1);

  report_service_status ();

  char *env_path = getenv ("PATH");
  if (!env_path)
    setenv ("PATH", "/bin", 1);
  else
    {
      char env_tmp[strlen (env_path) + 6];
      strcat (strcpy (env_tmp, env_path), ":/bin");
      setenv ("PATH", env_tmp, 1);
    }

  report_service_status ();

  /* Save timestamp if old pid file exists,
     so read_pidfile() can ignore this file. */
  time_t forktime = 0;
  if (pidfile_path && !access(pidfile_path, 0))
    {
      syslog (LOG_INFO, "service `%s': old pid file %s exists",
	svcname, pidfile_path);
      forktime = time(0);
      sleep(2);
    }

  /* Step 3: Fork child */
  int child_pid = fork ();
  switch (child_pid)
    {
    case -1: /* Error */
      syslog_starterr ("fork", 0, errno);
      /* Do not report that the service is stopped so that if recovery options
         are set, Windows will automatically restart the service. */
      exit (errno);
      
    case 0:  /* Child */
      if (redirect_io (stdin_path, stdout_path, stderr_path))
        exit (1);
      /* If chdir fails, report it but ignore it. */
      if (dir && chdir (dir))
	syslog_starterr ("chdir", 0, errno);

      if (!interactive && setpgrp () == -1)
	syslog_starterr ("setpgrp", 0, errno);

      exit (execv (path, arglist));
    
    default: /* Parent */
      report_service_status ();

      /* Free some unused memory. */
      free (path);
      free (arglist);
      if (dir)
        free (dir);
      if (env)
	{
	  for (int i = 0; i <= MAX_ENV && env[i].name; ++i)
	    {
	      free (env[i].name);
	      free (env[i].val);
	    }
	  free (env);
	}
      if (stdin_path)
        free (stdin_path);
      if (stdout_path)
        free (stdout_path);
      if (stderr_path)
        free (stderr_path);

      report_service_status ();

      /* Wait a second here to allow the child process to `execv'. If that
         fails, the child sends itself a signal which we can ask for in
	 `waitpid'. */

      sleep (1);

      report_service_status ();

      int status = 1 << 8; /* WEXITSTATUS() == 1 */
      int ret;
      if (!pidfile_path)
	{
	  /* No pid file, service must not fork(). */
	  /* Terminate pgrp unless interactive. */
	  server_pid = interactive ? child_pid : -child_pid;
	  ret = waitpid (child_pid, &status, WNOHANG);
	  if (!ret)
	    {
	      /* Child has probably `execv'd properly. */
	      set_service_status (SERVICE_RUNNING);
	      syslog (LOG_INFO, "`%s' service started", svcname);
	      /* Keep repeating waitpid() command as long as it returned because
		 of a handled signal sent to this cygrunsrv process */
	      while ((ret = waitpid (child_pid, &status, 0)) == -1 &&
		      errno == EINTR)
		 ;
	    }
	  else
	    /* The ret == -1 case below is only valid for the inner watpid call. */
	    ret = 0;
	}
      else
	{
	  /* Pid file given, first wait for daemon's fork(). */
	  int i;
	  for (i = 1; i <= 30; i++) {
	   if ((ret = waitpid (child_pid, &status, WNOHANG)) != 0)
	     break;
	   syslog (LOG_INFO, "service `%s': waiting for fork of %d (#%d)",
	     svcname, child_pid, i);
	   sleep(1);
	   report_service_status ();
	  }
	  if (ret == child_pid && WIFEXITED(status) && WEXITSTATUS(status) == 0)
	    {
	     /* Daemon has fork()ed successfully, wait for pidfile. */
	     int read_pid;
	     for (i = 1; i <= 30; i++) {
	       if ((read_pid = read_pidfile (pidfile_path, forktime)) != -1)
		 break;
	       syslog (LOG_INFO, "service `%s': waiting for file %s (#%d)",
		 svcname, pidfile_path, i);
	       sleep(1);
	       report_service_status ();
	     }
	     if (read_pid != -1) 
	       {
		 /* Got the real pid, daemon now running. */
		 server_pid = read_pid; /* Terminate pid only, daemon
					   should handle rest of pgrp. */
		 syslog (LOG_INFO, "service `%s' started, pid %d read from %s",
			 svcname, read_pid, pidfile_path);
		 set_service_status (SERVICE_RUNNING);
		 /* Process is no child, so waitpid() won't work here. */
		 ret = waitanypid(read_pid, &status);

		 /* Remove stale pidfile. */
		 if (ret != -1 && !access(pidfile_path, 0)) {
		   syslog (LOG_INFO, "service `%s': removing %s",
			   svcname, pidfile_path);
		   if (unlink(pidfile_path)) 
		     syslog (LOG_INFO, "service `%s': error removing %s (errno=%d)",
		  	   svcname, pidfile_path, errno);
		 }
	       }
	     else
	       syslog (LOG_ERR, "service `%s': no pid file %s found",
		       svcname, pidfile_path);
	    }
	  else if (ret == 0)
	    {
	      /* Daemon did not fork(), try to shutdown pgrp gracefully. */
	      syslog (LOG_ERR, "service `%s': %d did not fork, sending SIGTERM",
		svcname, child_pid);
	      kill (-child_pid, SIGTERM);
	      sleep(3);
	      report_service_status ();
	      sleep(3);
	      report_service_status ();
	      if ((ret = waitpid (child_pid, &status, WNOHANG)) == 0)
		{
		  syslog (LOG_ERR, "service `%s': %d did not terminate, "
		    "sending SIGKILL", svcname, child_pid);
		  kill (-child_pid, SIGKILL);
		  while ((ret = waitpid (child_pid, &status, 0)) == -1 &&
		      errno == EINTR)
		    ;
		}
	    }
	}

      /* If ret is -1, report errno, else process the status */
      if (ret == -1)
	{
	  switch (errno)
	    {
	    case ECHILD:
	    case ESRCH:
	      syslog (LOG_ERR, "service `%s' exited, its status was lost"
		      " errno %s", svcname, (errno==ECHILD?"ECHILD":"ESRCH"));
	      break;
	    default:
	      syslog (LOG_ERR, "service `%s' error: wait%spid() failed: "
		      "errno %d", svcname, (server_pid > 0 ? "any":""),
		      errno);
	    }
	  service_main_exitval = errno;
	  set_service_status (SERVICE_STOPPED);
	}
      else if (WIFEXITED (status))
	{
	  unsigned char s = WEXITSTATUS (status);
	  if (neverexits && !shutting_down)
	    {
	      syslog (LOG_ERR, "`%s' service exited prematurely with "
		      "exit status: %u", svcname, s);
	      /* Do not report that the service is stopped so that if
		 recovery options are set, Windows will automatically
		 restart the service. */
	      service_main_exitval = s;
	      _exit (service_main_exitval);
	    }
	  else
	    {
	      syslog (LOG_INFO, "`%s' service stopped, exit status: %u",
		      svcname, s);
	      service_main_exitval = 0;
	      set_service_status (SERVICE_STOPPED);
	    }
	}
      else if (WIFSIGNALED (status))
	{
	  /* If the signal is the one we've send, everything's ok.
	     Otherwise we log the signal event. */
	  if (!termsig_sent || WTERMSIG (status) != termsig)
	    syslog (LOG_ERR, "service `%s' failed: signal %d raised",
		    svcname, WTERMSIG (status));
	  else
	    syslog (LOG_INFO, "`%s' service stopped, signal %d received",
		    svcname, WTERMSIG (status));
	  service_main_exitval = 0;
	  set_service_status (SERVICE_STOPPED);
	}
      else
	{
	  syslog (LOG_ERR, "`%s' service stopped for an unknown reason",
		  svcname);
	  service_main_exitval = 0;
	  set_service_status (SERVICE_STOPPED);
	}
    }
}

int
main (int argc, char **argv)
{
  int c;
  action_t action = Undefined;
  char *in_name = NULL;
  char *in_desc = NULL;
  char *in_path = NULL;
  char *in_args = NULL;
  char *in_dir = NULL;
  env_t *in_env = NULL;
  char *in_disp = NULL;
  type_t in_type = NoType;
  char *in_user = NULL;
  char *in_pass = NULL;
  int in_termsig = 0;
  char **in_deps = 0;
  char *in_stdin = NULL;
  char *in_stdout = NULL;
  char *in_stderr = NULL;
  char *in_pidfile = NULL;
  int in_neverexits = 0;
  int in_shutdown = 0;
  int in_interactive = 0;
  int in_showcons = 0;
  bool verbose = false;

  appname = argv[0];


  if (argc < 2)
    {
      /* Started without parameters: Start the service.
         argv[0] contains the name of the service. */

      SERVICE_TABLE_ENTRYA ste[2];

      ste[0].lpServiceName = appname;
      ste[0].lpServiceProc = service_main;
      ste[1].lpServiceName = NULL;
      ste[1].lpServiceProc = NULL;
      if (!StartServiceCtrlDispatcherA(ste))
	return error (StartAsSvcErr);
      return service_main_exitval;
    }
  /* Started with parameters: Install, deinstall, start or stop a service. */ 
  while ((c = getopt_long(argc, argv, opts, longopts, NULL)) != EOF)
    switch (c)
      {
      case 'I':
	if (action != Undefined)
	  return error (ReqAction);
	action = Install;
	in_name = optarg;
	break;
      case 'R':
	if (action != Undefined)
	  return error (ReqAction);
	action = Remove;
	in_name = optarg;
	break;
      case 'S':
	if (action != Undefined)
	  return error (ReqAction);
	action = Start;
	in_name = optarg;
	break;
      case 'E':
	if (action != Undefined)
	  return error (ReqAction);
	action = Stop;
	in_name = optarg;
	break;
      case 'Q':
	if (action != Undefined) 
	  return error (ReqAction);
	action = Query;
	in_name = optarg;
	break;
      case 'L':
      	if (action != Undefined) 
	  return error (ReqAction);
	action = List;
	break;
      case 'p':
	if (action != Install)
	  return error (PathNotAllowed);
	if (in_path)
	  return error (OnlyOnePath);
	in_path = optarg;
	break;
      case 'a':
	if (action != Install)
	  return error (ArgsNotAllowed);
	if (in_args)
	  return error (OnlyOneArgs);
	in_args = optarg;
	break;
      case 'c':
	if (action != Install)
	  return error (ChdirNotAllowed);
	if (in_dir)
	  return error (OnlyOneChdir);
	in_dir = optarg;
	break;
      case 'e':
	if (action != Install)
	  return error (EnvNotAllowed);
	if (add_env_var (optarg, in_env))
	  return 1;
	break;
      case 'd':
	if (action != Install)
	  return error (DispNotAllowed);
	if (in_disp)
	  return error (OnlyOneDisp);
	in_disp = optarg;
	break;
      case 'f':
	if (action != Install)
          return error (DescNotAllowed);
	if (in_desc)
	  return error (OnlyOneDesc);
	in_desc = optarg;
	break;
      case 'n':
	if (action != Install)
	  return error (NeverExitsNotAllowed);
	if (in_neverexits)
	  return error (OnlyOneNeverExits);
	in_neverexits = 1;
	break;
      case 'o':
	if (action != Install)
	  return error (ShutdownNotAllowed);
	if (in_shutdown)
	  return error (OnlyOneShutdown);
	in_shutdown = 1;
	break;
      case 'i':
	if (action != Install)
	  return error (InteractiveNotAllowed);
	if (in_interactive)
	  return error (OnlyOneInteractive);
	if (in_user)
	  return error (NoInteractiveWithUser);
	in_interactive = 1;
	break;
      case 'j':
	if (action != Install)
	  return error (ShowconsNotAllowed);
	if (in_showcons)
	  return error (OnlyOneShowcons);
	in_showcons = 1;
        break;
      case 's':
	if (action != Install)
	  return error (SigNotAllowed);
	if (in_termsig)
	  return error (OnlyOneSig);
	if (! (in_termsig = atoi(optarg)))
	  {
	    char sig_name[128];
	    sprintf(sig_name, "SIG%s", optarg);
	    if (! (in_termsig = strtosigno(sig_name)))
	      return error (InvalidSig);
	  }
	break;
      case 't':
	if (action != Install)
	  return error (TypeNotAllowed);
	if (in_type != NoType)
	  return error (OnlyOneType);
	if (!strcasecmp (optarg, "a") || !strcasecmp (optarg, "auto"))
	  in_type = Auto;
	else if (!strcasecmp (optarg, "m") || !strcasecmp (optarg, "manual"))
	  in_type = Manual;
	else
	  return error (InvalidType);
	break;
      case 'u':
	if (action != Install)
	  return error (UserNotAllowed);
	if (in_user)
	  return error (OnlyOneUser);
	if (in_interactive)
	  return error (NoInteractiveWithUser);
	in_user = optarg;
	break;
      case 'w':
	if (action != Install)
	  return error (PassNotAllowed);
	if (in_pass)
	  return error (OnlyOnePass);
	in_pass = optarg;
	break;
      case 'y':
	if (action != Install)
	  return error (DepNotAllowed);
	if (add_dep(optarg, in_deps))
	  return 1;
	break;
      case '0':
	if (action != Install)
	  return error (IONotAllowed);
	if (in_stdin)
	  return error (OnlyOneIO);
	in_stdin = optarg;
	break;
      case '1':
	if (action != Install)
	  return error (IONotAllowed);
	if (in_stdout)
	  return error (OnlyOneIO);
	in_stdout = optarg;
	break;
      case '2':
	if (action != Install)
	  return error (IONotAllowed);
	if (in_stderr)
	  return error (OnlyOneIO);
	in_stderr = optarg;
	break;
      case 'x':
	if (action != Install)
	  return error (IONotAllowed);
	if (in_pidfile)
	  return error (OnlyOneIO);
	in_pidfile = optarg;
	break;
      case 'V':
        verbose = true;
        break;
      case 'h':
	return usage ();
      case 'v':
        return version ();
      default:
        return error (UnrecognizedOption);
      }
  if (optind < argc)
    return error (TrailingArgs);
  switch (action)
    {
    case Install:
      int ret;

      if (!in_path)
        return error (ReqPath);
      if (!in_disp)
	in_disp = in_name;
      if (in_type == NoType)
        in_type = Auto;
      if (!is_executable (in_path))
        return error (InvalidPath);
      if (ret = install_service (in_name, in_disp, in_type, in_user, in_pass,
      				 in_deps, in_interactive))
        return ret;
      if (ret = install_registry_keys (in_name, in_desc, in_path, in_args,
				       in_dir, in_env, in_termsig,
				       in_stdin, in_stdout, in_stderr,
				       in_pidfile, in_neverexits, in_shutdown,
				       in_interactive, in_showcons))
        remove_service (in_name);
      return ret;
      break;
    case Remove:
      return remove_service (in_name);
      break;
    case Start:
      return start_service (in_name);
      break;
    case Stop:
      return stop_service (in_name);
      break;
    case Query:
      return query_service (in_name, verbose);
      break;
    case List:
      return list_services (verbose);
      break;
    }
  return error (ReqAction);
}
