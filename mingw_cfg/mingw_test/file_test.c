#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{	
	int	fd	= open("slota", O_RDWR| O_BINARY | O_CREAT
						, S_IREAD|S_IWRITE);
	printf("fd=%d\n", fd);
	close(fd);
}
