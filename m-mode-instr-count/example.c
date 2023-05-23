#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "module/sample.h"

static uint64_t rdcycle(void) {
  uint64_t val;
//   asm volatile ("rdcycle %0" : "=r"(val));
    asm volatile ("rdinstret %0" : "=r"(val));
//        asm volatile ("rdtime %0" : "=r"(val));
return val;
}

int main() {
    // open the module
    int fd = open(MODULE_DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error: Could not open module: %s\n", MODULE_DEVICE_PATH);
        return -1;
    }
    
    size_t sum1 = 0, sum2 = 0, sum1u = 0, sum2u = 0;
    size_t val1 = 0, val2 = 0;
    for(int i = 0; i < 1000; i++) {
        size_t start = rdcycle();
        ioctl(fd, MODULE_IOCTL_CMD_INC, (unsigned long)&val1);
        sum1u += rdcycle() - start;
        start = rdcycle();
        ioctl(fd, MODULE_IOCTL_CMD_INC2, (unsigned long)&val2);
        sum2u += rdcycle() - start;
        sum1 += val1;
        sum2 += val2;
    }
    printf("1: %10zd | %10zd\n", sum1 / 1000, sum1u / 1000);
    printf("2: %10zd | %10zd\n", sum2 / 1000, sum2u / 1000);

    close(fd);
}
