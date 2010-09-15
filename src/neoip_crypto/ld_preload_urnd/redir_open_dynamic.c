/*
 * File: redir_open.c
 * Redirect all open("/dev/random"); to /dev/urandom
 * - work around slowness bugs in linux /dev/random
 *
 * Compile:
 * gcc -fPIC -c -o redir_open.o redir_open.c
 * gcc -shared -o redir_open.so redir_open.o -ldl
 *
 * Use:
 * LD_PRELOAD="./redir_open.so" command
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#define _FCNTL_H
#include <bits/fcntl.h>

extern int errorno;

int (*_open)(const char * pathname, int flags, ...);
int (*_open64)(const char * pathname, int flags, ...);

int open(const char * pathname, int flags, mode_t mode)
{
	// path conversion
	if( !strcmp(pathname, "/dev/random") )	pathname	= "/dev/urandom";
	// forward to normal open
	_open = (int (*)(const char * pathname, int flags, ...)) dlsym(RTLD_NEXT, "open");
	if(!(flags & O_CREAT))	mode = 0;
	return _open(pathname, flags, mode);
}

int open64(const char * pathname, int flags, mode_t mode)
{
	// path conversion
	if( !strcmp(pathname, "/dev/random") )	pathname	= "/dev/urandom";
	// forward to normal open
	_open64 = (int (*)(const char * pathname, int flags, ...)) dlsym(RTLD_NEXT, "open64");
	if(!(flags & O_CREAT))	mode = 0;
	return _open64(pathname, flags, mode);
}