#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "pgsignal_w32.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
	int pid;
	int i;

	if (argc != 2) {
		printf("Usage: pgkill <pid>\n\n");
		return 1;
	}

	pid = atoi(argv[1]);
	if (pid <= 0) {
		printf("Invalid pid %i\n",pid);
		return 2;
	}
	
	i = kill(pid, SIGHUP);

	printf("Result: %i\n",i);
	printf("Lasterror: %i\n",GetLastError());
	return 0;
}