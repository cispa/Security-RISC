#include "../rlibsc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NUM_MEASURE 10000
#define CTR_THREAD_SPINS 100000

inline uint64_t as_nanoseconds(struct timespec *ts) {
  return ts->tv_sec * (uint64_t)1000000000L + ts->tv_nsec;
}

static inline void payload() { REP1K(asm volatile("nop");) }

void main() {
  uint32_t acc[] = {0, 0, 0, 0, 0, 0, 0};
  struct timespec start_c, end_c;
  uint64_t start, end;
  uint64_t cthread_spins = 100000;

  for (size_t i = 0; i < NUM_MEASURE; i++) {

    fencei(); // remove cached instructions to not skew timing
    clock_gettime(CLOCK_REALTIME, &start_c);
    payload();
    clock_gettime(CLOCK_REALTIME, &end_c);
    acc[0] += as_nanoseconds(&end_c) - as_nanoseconds(&start_c);

    fencei(); // remove cached instructions to not skew timing
    start = rdcycle();
    payload();
    end = rdcycle();
    acc[1] += end - start;

    fencei(); // remove cached instructions to not skew timing
    start = rdtime();
    payload();
    end = rdtime();
    acc[2] += end - start;

    fencei(); // remove cached instructions to not skew timing
    start = get_time_perf();
    payload();
    end = get_time_perf();
    acc[3] += end - start;

    fencei(); // remove cached instructions to not skew timing
    start = get_cycle_perf();
    payload();
    end = get_cycle_perf();
    acc[4] += end - start;

    fencei(); // remove cached instructions to not skew timing
    start = get_retire_perf();
    payload();
    end = get_retire_perf();
    acc[5] += end - start;

    fencei(); // remove cached instructions to not skew timing
    clock_gettime(CLOCK_REALTIME, &start_c);
    count_thread(cthread_spins);
    clock_gettime(CLOCK_REALTIME, &end_c);
    acc[6] += as_nanoseconds(&end_c) - as_nanoseconds(&start_c);
  }

  printf("[+] clock_gettime: %dns\n", acc[0] / NUM_MEASURE);
  printf("[+] rdcycle: %d ticks\n", acc[1] / NUM_MEASURE);
  printf("[+] rdtime: %d ticks\n", acc[2] / NUM_MEASURE);
  printf("[+] get_time_perf: %d ticks\n", acc[3] / NUM_MEASURE);
  printf("[+] get_cycle_perf: %d ticks\n", acc[4] / NUM_MEASURE);
  printf("[+] get_retire_perf: %d retired in interval with 1000 nops\n",
         acc[5] / NUM_MEASURE);
  printf("[+] count_thread %d spins in %dns\n", CTR_THREAD_SPINS,
         acc[6] / NUM_MEASURE);

  uint64_t baseline = acc[0] / NUM_MEASURE;

  printf("\n\n");
  printf("[+] Results:\n");
  printf("|timer            | resulution|\n");
  printf("|-----------------|-----------|\n");
  printf("|clock_gettime    | %4dns    |\n", baseline / baseline);
  printf("|rdcycle          | %4dns    |\n", baseline / (acc[1] / NUM_MEASURE));
  printf("|get_cycle_perf   | %4dns    |\n", baseline / (acc[4] / NUM_MEASURE));
  printf("|rdtime           | %4dns    |\n", baseline / (acc[2] / NUM_MEASURE));
  printf("|get_time_perf    | %4dns    |\n", baseline / (acc[3] / NUM_MEASURE));
  printf("|get_retire_perf  | %4dns    |\n", baseline / (acc[5] / NUM_MEASURE));
  printf("|count_thread     | %4dns    |\n",
         (acc[6] / NUM_MEASURE) / CTR_THREAD_SPINS);
}
