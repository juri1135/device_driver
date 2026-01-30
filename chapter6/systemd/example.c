#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>

#define SOCKET_NAME "/run/comento.sock"

#define MSG_TYPE_ENCRYPT 0
#define MSG_TYPE_DECRYPT 1

int request(char type, const char *in, size_t in_len,char *out,size_t *out_len){
    size_t pos, tmp_out_len;
    int sockfd;
    struct sockaddr_un addr;
    //소켓 생성 후 초기화, 경로 설정, 연결 후 연결 잘 되면 type, 길이, data 전송 
    sockfd=socket(AF_UNIX,SOCK_STREAM,0);
    if(sockfd<0){
        return -errno;
    }

    memset(&addr,0,sizeof(addr));
    addr.sun_family=AF_UNIX;
    snprintf(addr.sun_path,sizeof(addr.sun_path)-1,SOCKET_NAME);

    if(connect(sockfd,(const struct sockaddr*)&addr,sizeof(addr))<0){
        return -errno;
    }

    write(sockfd,&type,sizeof(type));
    write(sockfd,&in_len,sizeof(in_len));
    for(pos=0; pos<in_len;){
        pos+=write(sockfd,in+pos,in_len-pos);
    }
    //출력 길이 수신 후 출력 버퍼 크기 조정 후 데이터 수신 
    read(sockfd,&tmp_out_len,sizeof(tmp_out_len));
    if(*out_len>tmp_out_len){
        *out_len=tmp_out_len;
    }
    for(pos=0; pos<*out_len;){
        pos+=read(sockfd,out+pos,*out_len-pos);
    }
    close(sockfd);
    return 0;
}

int encrypt(const char *in,size_t in_len, char *out, size_t *out_len){
    return request(MSG_TYPE_ENCRYPT,in,in_len,out,out_len);
}


int decrypt(const char *in, size_t in_len, char *out, size_t *out_len){
    return request(MSG_TYPE_DECRYPT,in,in_len,out,out_len);
}

char msg_buf[4096];

int main(){
    pid_t pid;
    char msg_type;
    int sockfd;
    size_t msg_len,out_len;

    printf("Input >");
    //사용자 입력 수신 후 길이 계산 
    msg_buf[0]='\0';
    scanf("%2048s",msg_buf);
    msg_len=strlen(msg_buf);
    //암호화 요청 
    out_len=4096;
    printf("Req ENCRYPT %d %.2048s\n",msg_len,msg_buf);
    encrypt(msg_buf,msg_len,msg_buf,&out_len);
    printf("Rsp %d\n",msg_len);
    //길이를 출력 길이로 업데이트한 후 복호화 요청 
    msg_len=out_len;
    out_len=4096;
    printf("Req DECRPYT %d bytes data\n",msg_len);
    decrypt(msg_buf,msg_len,msg_buf,&out_len);
    printf("Rsp %d %.*s\n",msg_len,out_len,msg_buf);

    return 0;
}