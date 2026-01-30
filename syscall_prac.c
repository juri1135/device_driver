#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define __NR_push_data 452
#define __NR_pop_data 453

int main(int argc, char* argv[]){
    long data, result;
	if(!strcmp(argv[1],"push")&&argc==3){
        data=atoi(argv[2]);
        result=syscall(__NR_push_data,data);
        if(result!=0){fprintf(stderr,"Failed to push\n");}
    }
    else if(!strcmp(argv[1],"pop")&&argc==2){
        result=syscall(__NR_pop_data);
        if(result==-1) {fprintf(stderr,"Failed to pop\n");}
        else {fprintf(stdout,"Value: %d\n",result);}
    }
    else{
        fprintf(stderr,"Invalid input\n");
        fprintf(stdout,"Usage: %s <push | pop> [data|NULL]\n",argv[0]);
        return -1;
    }
    return 0;
}
