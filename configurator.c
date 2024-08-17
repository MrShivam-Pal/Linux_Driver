#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


#define DRIVER_NAME "/dev/vicharak"
#define SET_SIZE_OF_QUEUE _IOW('a', 'a', int * )

int main(void) {
    int fd = open(DRIVER_NAME, O_RDWR);
    int size = 100;
    int ret = ioctl(fd, SET_SIZE_OF_QUEUE, &size);
    printf("Queue size is set : %d\n", size);
    close(fd);
    return ret;
}
