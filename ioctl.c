#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define COMENTO_IOCTL_CLEAR _IO('c',0)

int main(){
    int fd=open("/dev/comento",O_RDWR);
    ioctl(fd,COMENTO_IOCTL_CLEAR,0);

    return 0;
}