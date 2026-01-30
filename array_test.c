#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define __NR_set_array 454
#define __NR_get_array 455

int main(int argc, char* argv[]){
    int data, result,idx;
	if(!strcmp(argv[1],"set")&&argc==4){
        idx=atoi(argv[2]);
        data=atoi(argv[3]);
        result=syscall(__NR_set_array,idx,data);
        if(result!=0){fprintf(stderr,"Failed to set\n");}
    }
    else if(!strcmp(argv[1],"get")&&argc==3){
        idx=atoi(argv[2]);
        result=syscall(__NR_get_array,idx);
        if(result==-1) {fprintf(stderr,"Failed to get\n");}
        else {fprintf(stdout,"Value: %d\n",result);}
    }
    else{
        fprintf(stderr,"Invalid input\n");
        fprintf(stdout,"Usage: %s <set | get> [index|index] [data|NULL]\n",argv[0]);
        fprintf(stdout,"Valid idx>=0, Valid data>0\n");
        return -1;
    }
    return 0;
}
