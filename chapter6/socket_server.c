#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//서버, 클라간 통신에 사용할 UNIX 소켓 파일 경로
#define SOCKET_NAME "/tmp/echo_socket"

int main(){
    pid_t pid;
    int sockfd, connfd;
    struct sockaddr_un addr;
    int recv_bytes;
    char buf[256];
    //UNIX 도메인 소켓을 스트림 방식으로 생성
    sockfd=socket(AF_UNIX,SOCK_STREAM,0);
    if(sockfd<0){
        fprintf(stderr,"{Server} Failed to create socket\n");
        return 1;
    }
    //소켓 구조체 초기화
    memset(&addr,0,sizeof(addr));
    //UNIX 도메인 소켓임을 지정
    addr.sun_family=AF_UNIX;
    //소켓 파일 경로 설정
    snprintf(addr.sun_path,sizeof(addr.sun_path)-1,SOCKET_NAME);
    //기존 소켓 파일 삭제를 통한 충돌 방지
    unlink(addr.sun_path);
    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0){
        fprintf(stderr,"{Server} Failed to bind\n");
        return 2;
    }

    if(listen(sockfd,0)<0){
        fprintf(stderr,"{Server} Failed to listen\n");
        return 2;
    }
    //정상적으로 바인딩, 들을 준비가 됐다면 끝날 때까지 계속 accept 시도
    while(1){
        connfd=accept(sockfd,NULL,NULL);
        if(connfd<0){
            fprintf(stderr,"{Server} Failed to accept\n");
            break;
        }
        printf("{Server} Client connected!\n");
        
        pid=fork();
        //자식 프로세스는 이번에 연결된 애 처리, 부모는 while문 돌면서 다음 응답 처리
        if(pid==0){
            while(1){
                recv_bytes=read(connfd,buf,sizeof(buf));
                if(recv_bytes<=0){
                    break;
                }
                printf("{Server} Recv: %.*s\n",recv_bytes,buf);
                //수신데이터를 클라이언트에 에코
                write(connfd,buf,recv_bytes);
            }
            close(connfd);
            printf("{Server} Client disconnected\n");
            return 0;
        }
    }
    close(sockfd);
    return 3;
}