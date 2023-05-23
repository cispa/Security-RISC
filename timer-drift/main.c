#include "../rlibsc.h"
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
// time interval for observing number of wakeups (in microseconds)
#define TIMER 1000 * 1000

#define TIMER_S (TIMER / (1000000))
#define TIMER_US (TIMER % (1000000))

struct itimerval timer = {
    .it_interval = {.tv_sec = TIMER_S, .tv_usec = TIMER_US},
    .it_value = {.tv_sec = TIMER_S, .tv_usec = TIMER_US}};

uint64_t lret = 0, lt = 0, lc = 0, cret = 0, ct = 0, cc = 0;
#define SW_L 5
size_t sw[SW_L];
int swp = 0;

void sigalarm(int num) {
  cc = rdcycle();
  ct = rdtime();
  cret = rdinstret();
  //     printf("i: %zd - t: %zd - c: %zd\n", cret - lret, ct - lt, cc - lc);
  //     printf("  c/i: %.2f - t/i: %.2f\n", (float)(cc - lc) / (cret - lret),
  //     (float)(100000000) / (cret - lret));
  // printf("  c/i: %.2f | c: %zd\n", (float)(cc - lc) / (cret - lret), cc -
  // lc);
  sw[swp] = cret - lret;
  swp = (swp + 1) % SW_L;
  ssize_t avg = 0;
  for (int i = 0; i < SW_L; i++) {
    avg += sw[i];
  }
  avg /= SW_L;

  printf("ret: %zd\n", (cret - lret) - avg);
  //   if((cret - lret) - avg < (ssize_t)-6000000) printf("irq!\n");
  lret = cret;
  lt = ct;
  lc = cc;
}

int main(int argc, char *argv[]) {
  signal(SIGALRM, sigalarm);
  setitimer(ITIMER_REAL, &timer, NULL);

  while (1) {
    sched_yield();
  }
  /*
    cc = rdcycle();
    ct = rdtime();
    cret = rdinstret();
//     printf("i: %zd - t: %zd - c: %zd\n", cret - lret, ct - lt, cc - lc);
//     printf("  c/i: %.2f - t/i: %.2f\n", (float)(cc - lc) / (cret - lret),
(float)(100000000) / (cret - lret)); printf("  c/i: %.2f\n", (float)(cc - lc) /
(cret - lret));

    lret = cret;
    lt = ct;
    lc = cc;
    usleep(100000);
  }
    */

  return 0;
}
