#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>

//세마포어 이름
#define SEM_NAME "comento"



//부모와 자식 프로세스가 공유 메모리의 정수 값을 각각 증가/감소시키면서 동기화
//익명 공유 메모리는 파일 기반이 아닌 프로세스 간의 직접 공유 
int main(int argc, char *argv[]){
    int *ptr,i,sem_init=1;
    sem_t *sem;
    pid_t pid;

    //이 실험의 경우는 2개의 프로세스를 동시 실행시키는 방식으로 테스트할 것. 
    //따라서 sem을 2로 초기화하면 사실상 세마포어 없이 동시에 둘 다 접근 가능한 것 
    if(argc==2&&!strcmp(argv[1],"-no-sem")){
       sem_init=2;
    }
    //기존에 갖고 있었을 수 있어서 해제해주기
    sem_unlink(SEM_NAME);
    //세마포어 생성하고 excl로 중복 방지 
    sem=sem_open(SEM_NAME,O_CREAT | O_EXCL,0600, sem_init);
    if(sem==SEM_FAILED){
        fprintf(stderr,"Failed to create semaphore\n");
        return 1;
    }
    printf("Mapping an anonymous share memory\n");
    //익명 공유 메모리 매핑 (이건 부모/자식간만 사용 가능 )
    ptr=mmap(NULL,sizeof(*ptr),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    pid=fork();
    
    if(pid==0){
        for(i=0; i<10000; i++){
            //세마포어 획득
            sem_wait(sem);
            //공유 메모리 값 증가
            (*ptr)++;
            //세마포어 해제
            sem_post(sem);
            if(i%1000==0){
                printf("[Child] %d\n",*ptr);
            }
        }
    }
    else{
        for(i=0; i<10000; i++){
            sem_wait(sem);
            (*ptr)--;
            sem_post(sem);
            if(i%1000==0){
                printf("[Parent] %d\n",*ptr);
            }
        }
        wait(NULL);
        //동기화가 잘 된다면 충돌 없이 총 결과 0이 나옴
        //만약 동기화가 잘 안 됐다면 child가 1 load하고 parent로 넘어가서 1을 load
        // child가 1 더해서 2 저장, parent가 1 빼서 0 저장. 원래 1이 되어야 하는데 충돌해서 0이 저장됨 
        printf("Final value: %d\n",*ptr);
    }
    return 0;
    
}