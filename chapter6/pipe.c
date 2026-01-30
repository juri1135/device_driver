#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    pid_t pid;
    int pipe_from_parent_to_child[2];
    int pipe_from_child_to_parent[2];
    char buf[256];

    pipe(pipe_from_parent_to_child);
    pipe(pipe_from_child_to_parent);

    pid=fork();
    if(pid==0){
        //close fd we don't use
        close(pipe_from_child_to_parent[0]);
        close(pipe_from_parent_to_child[1]);

        const char *msg="I'm child~!!!\n";
        printf("[Child] send msg: %s\n",msg);

        write(pipe_from_child_to_parent[1],msg,strlen(msg)+1);
        sleep(1);

        read(pipe_from_parent_to_child[0],buf,256);
        printf("[Child] recv msg: %s\n",buf);

        close(pipe_from_child_to_parent[1]);
        close(pipe_from_parent_to_child[0]);
    }
    else{
        close(pipe_from_parent_to_child[0]);
        close(pipe_from_child_to_parent[1]);

        const char *msg="I'm parent\n";
        printf("[Parent] send msg: %s\n",msg);
        //부모가 pipe에 write하고 대기
        write(pipe_from_parent_to_child[1],msg,strlen(msg)+1);
        sleep(1);
        //부모가 pipe에 있는 message read하기
        read(pipe_from_child_to_parent[0],buf,256);
        printf("[Parent] recv msg: %s\n",buf);

        close(pipe_from_parent_to_child[1]);
        close(pipe_from_child_to_parent[0]);

        waitpid(pid,NULL,0);
    }
    return 0;
}