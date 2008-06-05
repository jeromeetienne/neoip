/* Unit test to test if it is possible to get SIGINT
 * - some issue appears when using mingw+wine
 * - no idea who is the 'culprit'
 * - i got it run under window and this is wine the culprit
 * 
 * To compile it:
 * cc -o sigint_test sigint_test.c
 */
#include <signal.h>
#include <stdio.h>

void handler()
{
	printf("SIGINT received\n");
	signal(SIGINT,handler);
}

int main()
{
	signal(SIGINT,handler);
	for(;;);
}
