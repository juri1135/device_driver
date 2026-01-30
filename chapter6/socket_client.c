#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//서버, 클라간 통신에 사용할 UNIX 소켓 파일 경로
#define SOCKET_NAME "/tmp/echo_socket"


//Server와 Client를 각각 빌드하고 각각 다른 터미널에서 실행+ 클라 동시에 여러 개 실행
int main(){
    pid_t pid;
    int sockfd;
    struct sockaddr_un addr;
    int recv_bytes;
    char buf[256];

    //UNIX 도메인 소켓을 스트림 방식으로 생성
    sockfd=socket(AF_UNIX,SOCK_STREAM,0);
    if(sockfd<0){
        fprintf(stderr,"{Client} Failed to create socket\n");
        return 1;
    }
    //소켓 구조체 초기화
    memset(&addr,0,sizeof(addr));
    //UNIX 도메인 소켓임을 지정
    addr.sun_family=AF_UNIX;
    //소켓 파일 경로 설정
    snprintf(addr.sun_path,sizeof(addr.sun_path)-1,SOCKET_NAME);
    
    //서버의 소켓에 접속 시도
    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0){
        fprintf(stderr,"{Client} Failed to connect\n");
        return 2;
    }
    //사용자가 q를 입력하기 전까지 계속 입력 받음
    while(1){
        printf("'q' for exit> ");
        //버퍼 초기화
        buf[0]='\0';
        //최대 255바이트 문자열 수신(공백 제외)
        scanf("%255s",buf);
        if(!strcmp(buf,"q")){
            break;
        }
        //소켓 디스크립터에 버퍼 내용 작성 후 송신
        write(sockfd,buf,strlen(buf));
        //서버측의 답신 듣기
        recv_bytes=read(sockfd,buf,sizeof(buf));
        if(recv_bytes<=0){
            printf("{Client} Server shutdown\n");
            break;
        }
        printf("{Client} Recv: %.*s\n",recv_bytes,buf);
    }
    close(sockfd);
    printf("{Client} exit!\n");
    return 3;
}