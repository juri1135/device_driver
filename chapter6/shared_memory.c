#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

//공유 메모리 이름
#define SHM_NAME "/comento_mem"
//공유 메모리 사이즈 
#define SHM_SIZE 4096

int main(int argc, char *const argv[]){
    int needs_init=0,fd;
    char *ptr;

    if(argc==2&&!strcmp(argv[1],"-d")){
        //삭제 입력하면 unlink로 공유 메모리 삭제 
        printf("Delete the shared memory\n");
        shm_unlink(SHM_NAME);
        return 0;
    }
    else if(argc==2&&!strcmp(argv[1],"-l")){
        // /dev/shm 디렉토리의 파일 목록을 표시하기 위해 execve로 /bin/ls /dev/shm 실행 
        static char *const ls_argv[]={
            "/bin/ls","/dev/shm",NULL
        };
        printf("List the shared memory:\n");
        execve(ls_argv[0],ls_argv,NULL);
        fprintf(stderr,"Failed to run ls command");
        return 5;
    }
    else if(argc!=1){
        fprintf(stderr,"Usage: %s <-d | -l>\n",argv[0]);
        return 4;
    }
    //공유 메모리 열기. 기존 공유 메모리를 읽기/쓰기 모드로 엶. 권한은 소유자만 읽기/쓰기 
    fd=shm_open(SHM_NAME,O_RDWR,0600);
    if(fd==-1){
        //파일이 없으면 새로 생성
        if(errno==ENOENT){
            printf("Create new shared memory\n");
            fd=shm_open(SHM_NAME,O_CREAT|O_RDWR,0600);
            printf("Set the size of shared memory\n");
            //크기 설정 
            if(ftruncate(fd,SHM_NAME)==-1){
                fprintf(stderr,"Failed to ftruncate\n");
                return 2;
            }
            needs_init=1;
        }
        //존재하는데 열리지 않은 거면 오류
        else{
            fprintf(stderr,"Failed to shm_open\n");
            return 1;
        }
    }
    printf("Mapping the shared memory\n");
    //시작 주소 자동 지정, 크기 지정, 읽기/쓰기, 여러 프로세스와 공유 
    ptr=(char*) mmap(NULL,SHM_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    
    if(ptr==MAP_FAILED){
        fprintf(stderr,"Failed to mmap\n");
        return 3;
    }
    //새로 생성되지 않았다면 기존 데이터 출력 
    if(!needs_init){
        printf("before: %.4095s\n",ptr);
    }
    scanf("%4095s",ptr);
    munmap(ptr,SHM_SIZE);
    close(fd);
    return 0;
    
}