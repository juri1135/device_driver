#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void sigint_handler(int signum){
    printf("Ctrl+C pressed!\n");
    exit(0);
}

void sigusr1_handler(int signum){
    printf("SIGUSR1 occured!\n");
}

int main(){
    pid_t pid;
    pid=fork();
    //자식 process는 부모 process를 bg로 돌리고 기다렸다가 다시 continue해서 bg에서 돌아가게 시작 
    //백그라운드에서 signal 보내게 하기 위해 돌린 것
    if(pid==0){
        kill(getppid(),SIGSTOP);
        sleep(1);
        kill(getppid(),SIGCONT);
        sleep(1);
    }
    //부모는 back groud에서 signal handler 등록
    //SIGINT는 Ctrl+C 
    //SIGUSR1은 별개 signal 
    else{
        if(signal(SIGINT,sigint_handler)<0){
            fprintf(stderr,"Failed to register handler\n");
            return 1;
        }
        if(signal(SIGUSR1,sigusr1_handler)<0){
            fprintf(stderr,"Failed to register handler\n");
            return 1;
        }
        wait(NULL);
        while(1){
            sleep(1);
        }
    }
    return 0;
}