#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>


#define DRIVER_NAME "/dev/vicharak"
#define POP_DATA _IOR('a', 'c', struct data * )

struct data {
    int length;
    char *data;
};


int main(void) {

    int fd = open(DRIVER_NAME, O_RDWR);
    struct data d;
    d.length = 10;
    d.data = malloc(10);
    int ret = ioctl(fd, POP_DATA, &d);
    printf("Data popped : %s\n", d.data);
    close(fd);
    free(d.data);
    return ret;
}
