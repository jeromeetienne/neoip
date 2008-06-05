#include <stdlib.h>
#include <stdio.h>

int main()
{
#ifndef	_WIN32
	char	str[]	= "/tmp/.neoip-oload.XXXXXX";
#else
	char	str[]	= "\\tmp\\.neoip-oload.XXXXXX";
#endif
	int	i;
	printf("Original pattern is %s\n", str);
	
	if( !mktemp(str) )	printf("mktemp failed!!");
	printf("result is %s\n", str);
}
