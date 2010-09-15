#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include <syscall.h>

#include <sys/types.h>
#include <sys/stat.h>
//#include <fcntl.h>
#define O_CREAT		   0100	/* not fcntl */

int open(const char * pathname, int flags, mode_t mode)
{
	//printf("pathname=%s\n", pathname);
	// path conversion
	if( !strcmp(pathname, "/dev/random") )	pathname	= "/dev/urandom";
	if(!(flags & O_CREAT))	mode = 0;
	return syscall(__NR_open, pathname, flags, mode); 
}