#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <systemd/sd-daemon.h>

//암호화/복호화 키, 키 저장 파일, 복호화/암호화 결과 파일
#define KEY "This is a password!"
#define TMP_KEY_FILE "/tmp/comento.key"
#define TMP_DECRYPT_FILE "/tmp/comento.decrypt"
#define TMP_ENCRYPT_FILE "/tmp/comento.encrypt"
//메세지 타입 상수
#define MSG_TYPE_ENCRYPT 0
#define MSG_TYPE_DECRYPT 1

//openssl 실행 파일, enc는 openssl의 서브커맨드로 암호화/복호화 작업 수행하는 모듈 호출,
//enc 뒤 서브 옵션이나 모드는 일단 NULL, -aes는 암호화 알고리즘, -kfile은 키를 파일에서 읽어오라는 옵션
//TMP_KEY_FILE이 그 파일 경로, -in, -out 뒤에 오는 게 지금은 NULL인데 이게 입력, 출력 파일 경로임 추후 설정 
//마지막은 배열 종료 마커
char *cmd_argv[]={
    "/usr/bin/openssl",
    "enc",
    NULL,
    "-aes-256-cbc",
    "-kfile",
    TMP_KEY_FILE,
    "-in",
    NULL,
    "-out",
    NULL,
    NULL,
};

int main(int argc, char* argv[]){
    int num_fds,recv_bytes,fd,conn_fd,file_fd;
    char msg_type=-1,*msg_buf;
    unsigned long long msg_len=0, pos;
    pid_t pid;
    //systemd가 제공하는 소켓 파일 디스크립터 수
    num_fds =sd_listen_fds(0);
    //데몬 모드로 실행. 세션 생성 안함+표준 입출력 유지
    daemon(0,1);
    
    if(num_fds<0){
        fprintf(stderr,"Failed to get listen_fds\n");
        fflush(stdout);
        return 1;
    }
    //임시 키 파일 읽기/쓰기 전용 생성
    file_fd=open(TMP_KEY_FILE,O_CREAT|O_TRUNC|O_WRONLY,0400);
    //파일에 키 작성 후 파일 닫기
    write(file_fd,KEY,strlen(KEY));
    close(file_fd);
    //systemd 제공 소켓 순회하면서 연결 시도
    for(fd=SD_LISTEN_FDS_START; fd<SD_LISTEN_FDS_START+num_fds; fd++){
        conn_fd=accept(fd,NULL,NULL);
        if(conn_fd<=0){
            fprintf(stderr,"Failed to accept");
            goto err;
        }
    
        //연결된 소켓의 메시지 타입, 길이 읽고 버퍼 할당 후 데이터 읽기
        read(conn_fd,&msg_type,sizeof(msg_type));
        read(conn_fd,&msg_len,sizeof(msg_len));
        msg_buf=malloc(msg_len);
        for(pos=0; pos<msg_len;){
            pos+=read(conn_fd,msg_buf+pos,msg_len-pos);
        }
        printf("Req type %d, len %d\n",msg_type,msg_len);
        fflush(stdout);
        switch(msg_type){
            //암호화모드면 e 설정 후 입력 파일로 해독, 출력 파일로 암호
            case MSG_TYPE_ENCRYPT:
            cmd_argv[2]="-e";
            cmd_argv[7]=TMP_DECRYPT_FILE;
            cmd_argv[9]=TMP_ENCRYPT_FILE;
            break;
            case MSG_TYPE_DECRYPT:
                cmd_argv[2]="-d";
                cmd_argv[7]=TMP_ENCRYPT_FILE;
                cmd_argv[9]=TMP_DECRYPT_FILE;
                break;
            default:
                fprintf(stderr, "No proper msg type");
                goto err;
        }
        //입력 파일 생성 후 읽기/쓰기 전용 열기해서 버퍼에 저장된 거 써서 닫고 버퍼 초기화
        file_fd=open(cmd_argv[7],O_CREAT|O_TRUNC|O_WRONLY,0600);
        write(file_fd,msg_buf,msg_len);
        close(file_fd);
        free(msg_buf);
        //부모는 기다리고 자식은 OpenSSL 실행 
        pid=fork();
        if(pid!=0){
            wait(NULL);
        }
        else{
            execve(cmd_argv[0],cmd_argv,NULL);
        }
        //출력 파일 일여서 파일 크기 및 위치 초기화 후 결과 읽어서 전달
        file_fd=open(cmd_argv[9],O_CREAT|O_RDONLY,0600);
        msg_len=lseek(file_fd,0,SEEK_END);
        lseek(file_fd,0,SEEK_SET);
        msg_buf=malloc(msg_len);
        read(file_fd,msg_buf,msg_len);
        close(file_fd);
        write(conn_fd,&msg_len,sizeof(msg_len));
        for(pos=0; pos<msg_len;){
            pos+=write(conn_fd,msg_buf+pos,msg_len-pos);
        };
        free(msg_buf);

        printf("Rsp len %d\n",msg_len);
        fflush(stdout);
err:
    close(conn_fd);
    close(fd);
    }
    return 0;
}