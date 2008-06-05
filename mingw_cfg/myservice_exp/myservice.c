/* Unit test to test a dummy win32 service
 * - if 
 * 
 * To compile it:
 * cc -o myservice.exe myservice
 */
#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#if defined(__MINGW32__)
#include <windows.h>
#	define sleep(x) Sleep((x)*1000)
#endif

static int	exit_now = 0;

/** \brief Dump the char * str at the end of a file
 */
static void	log_dump(char *str)
{
	FILE *	fOut	= fopen("c:\\myservice.log", "at");
	assert(fOut);
	fprintf(fOut, "%s\n", str);
	fclose(fOut);
}

static void handler_sigint()
{
	log_dump("SIGINT received");
	exit_now	= 1;
}

static void handler_sigterm()
{
	log_dump("SIGTERM received");
	exit_now	= 1;
}

int main_internal(int argc, char *argv[])
{
	char	str[1024];
	// init the signal
	signal(SIGINT,handler_sigint);
	signal(SIGTERM,handler_sigterm);
	// log the start
	log_dump("main starting");
	// log the argc/argv
	sprintf(str, "argc=%d", argc);	log_dump(str);
	int	i;
	for(i = 0; i < argc; i++){
		sprintf(str, "argv[%d]=%s", i, argv[i]);
		log_dump(str);	
	}
	
	// start looping
	while( !exit_now ){
		sprintf(str, "running at %ld", time(NULL));
		log_dump(str);
		sleep(1);
	}
	// log the exit
	log_dump("main exiting");
	return 0;
}
