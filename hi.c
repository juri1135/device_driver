#include <stdio.h>
#include <unistd.h>

#define __NR_new_syscall 451
int main(){
	int ret=syscall(__NR_new_syscall, 15);
	printf("%d",ret);
	return 0;
}
