#include <stdio.h>

int mylib2_fct2()
{
	printf("enter %s\n", __func__);
	mylib1_fct2();
}
