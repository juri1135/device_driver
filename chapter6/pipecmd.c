#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//execve 호출 시 현재 프로세스의 환경 변수 전달
extern char **environ;

//첫 번째 프로세스가 실행할 명령어 배열
//cat이니까 /etc/passwd 파일 내용을 출력함. 
char *const front_argv[]={
    "/bin/cat",
    "/etc/passwd",
    NULL
};
//두 번째 프로세스가 실행할 명령어 배열
// /bin/cat, /etc/passwd 명령의 출력을 /bin/wc -l 명령에 전달한다. 
char *const back_argv[]={
    "/bin/wc",
    "-l",
    NULL
};
int main(){
    pid_t pid;
    int pipefd[2];
    //첫 번째 프로세스가 pipefd[1]에 데이터를 쓰고, 두 번째 프로세스가 pipefd[0]에서 데이터를 읽음
    pipe(pipefd);

    pid=fork();
    if(pid==0){
        //we don't need read fd
        close(pipefd[0]);
        //all standard output send to write fd
        dup2(pipefd[1],STDOUT_FILENO);
        close(pipefd[1]);
        // /bin/cat /etc/passwd 실행. 
        execve(front_argv[0],front_argv,environ);
    }
    pid=fork();
    if(pid==0){
        close(pipefd[1]);
        dup2(pipefd[0],STDIN_FILENO);
        close(pipefd[0]);
        execve(back_argv[0],back_argv,environ);
    }
    wait(NULL);
    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    return 0;
    
}