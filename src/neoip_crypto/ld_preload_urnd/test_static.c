#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define DISPLAY 80
#define PAGE_LENGTH 20

int main(int argc, char *argv[])
{
	char filename[80]="/dev/random";
	
	int fd	= open(filename, 0);
	printf("fd=%d\n", fd); 
	return 0;
}
