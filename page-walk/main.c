#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "../rlibsc.h"

#define REP5(x) x x x x x
#define REP10(x) REP5(x) REP5(x)

char __attribute__((aligned(4096))) buffer[4096 * 32];
static inline void evict(void *addr) {
  int i = 1;
  REP10(maccess(buffer + i++ * 4096);)
}

char __attribute__((aligned(4096))) target[4096];

int main() {
  memset(target, 1, sizeof(target));
  memset(buffer, 2, sizeof(buffer));
  for (int i = 0; i < 2; i++) {
    if (i == 0)
      printf("[ No Walk ]\n");
    else
      printf("[ Walk ]\n");
    size_t sum = 0, min = -1, sumt = 0, mint = -1;
    for (int avg = 0; avg < 10; avg++) {
      if (i == 0)
        maccess(target + 256);
      else
        evict(target);
      size_t start = rdcycle();
      size_t before = rdinstret();
      maccess(target);
      size_t after = rdinstret();
      size_t end = rdcycle();
      size_t delta = after - before;
      sum += delta;
      if (delta < min)
        min = delta;
      delta = end - start;
      sumt += delta;
      if (delta < mint)
        mint = delta;
    }
    printf("Retired: %zd (avg: %zd) | Time: %zd (avg: %zd)\n", min, sum / 10,
           mint, sumt / 10);
  }
}
