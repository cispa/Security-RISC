#include "../rlibsc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

volatile uint64_t foo;

uint64_t __attribute__((noinline))
square_multiply(int bit, uint64_t a, uint64_t b) {
  foo = 1;
  a *= a;
  if (bit) {
    a *= b;
  } else {
    REP3(asm volatile("nop");)
  }
  return a;
}

int main(int argc, char *argv[]) {
  square_multiply(0, 5, 7);
  square_multiply(1, 5, 7);

  uint64_t start = rdcycle();
  square_multiply(0, 5, 7);
  uint64_t end = rdcycle();
  uint64_t b0 = end - start;
  start = rdcycle();
  square_multiply(1, 5, 7);
  end = rdcycle();
  uint64_t b1 = end - start;
  printf("Timing 0: %zd\n", b0);
  printf("Timing 1: %zd\n", b1);

  square_multiply(0, 5, 7);
  square_multiply(1, 5, 7);

  start = rdinstret();
  square_multiply(0, 5, 7);
  end = rdinstret();
  b0 = end - start;
  start = rdinstret();
  square_multiply(1, 5, 7);
  end = rdinstret();
  b1 = end - start;
  printf("Retired instr. 0: %zd\n", b0);
  printf("Retired instr. 1: %zd\n", b1);

  return 0;
}
