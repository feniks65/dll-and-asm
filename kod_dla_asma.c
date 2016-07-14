#include <stdio.h>

int get()
{
	__asm(".globl	_getPiec \n"
	"_getPiec: \n"
	"pushl	%ebp \n"
	"movl	%esp, %ebp \n"
	"movl	$5, %eax \n"
	"popl	%ebp \n"
	"ret");
}

int main()
{
	int ret;
	ret = getPiec();
	
	printf("getPiec=%d", ret);
	
	return 0;
}