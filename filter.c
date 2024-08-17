#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

#define DRIVER_NAME "/dev/vicharak"
#define PUSH_DATA _IOW('a', 'b', struct data * )

struct data {
    int length;
    char *data;
};

int main(void) {
    int fd = open(DRIVER_NAME, O_RDWR);
    struct data d;
    d.length = 3;
    d.data = malloc(3);
    memcpy(d.data, "xyz", 3);
    int ret = ioctl(fd, PUSH_DATA, &d);
    printf("Data pushed in Queue : %s\n" , d.data);
    close(fd);
    free(d.data);
    return ret;
}
