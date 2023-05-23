#include "../rlibsc.h"
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define TESTS 1000000

int in_range(size_t val, size_t div) {
  return (val > (TESTS / div) * 0.9 && val < (TESTS / div) * 1.1);
}

inline uint64_t as_nanoseconds(struct timespec *ts) {
  return ts->tv_sec * (uint64_t)1000000000L + ts->tv_nsec;
}

int main() {

  size_t div = 100;
  size_t best_top = 0;
  size_t best_top_at = (size_t)-1;
  struct timespec stamp;

  do {
    size_t hist[div];
    memset(hist, 0, sizeof(hist));

    for (size_t i = 0; i < TESTS; i++) {
      clock_gettime(CLOCK_REALTIME, &stamp);
      size_t val = as_nanoseconds(&stamp);
      hist[val % div]++;
    }

    for (size_t i = 0; i < div; i++) {
      if (hist[i] > best_top) {
        best_top = hist[i];
        best_top_at = div;
      }
    }

    div--;
  } while (div > 1);

  if (best_top < TESTS * 0.9) {
    printf("[+] clock_gettime Estimated increment: %.1f\n",
           (double)(best_top_at * best_top) / TESTS);
  } else {
    printf("[+] clock_gettime Increment: %zd\n", best_top_at);
  }

  // -----------------------------------------------------------------------------------------------------------

  div = 100;
  best_top = 0;
  best_top_at = (size_t)-1;

  do {
    size_t hist[div];
    memset(hist, 0, sizeof(hist));

    for (size_t i = 0; i < TESTS; i++) {
      size_t val = rdcycle();
      hist[val % div]++;
    }

    for (size_t i = 0; i < div; i++) {
      if (hist[i] > best_top) {
        best_top = hist[i];
        best_top_at = div;
      }
    }

    div--;
  } while (div > 1);

  if (best_top < TESTS * 0.9) {
    printf("[+] rdcycle Estimated increment: %.1f\n",
           (double)(best_top_at * best_top) / TESTS);
  } else {
    printf("[+] rdcycle Increment: %zd\n", best_top_at);
  }

  // -----------------------------------------------------------------------------------------------------------

  div = 100;
  best_top = 0;
  best_top_at = (size_t)-1;

  do {
    size_t hist[div];
    memset(hist, 0, sizeof(hist));

    for (size_t i = 0; i < TESTS; i++) {
      size_t val = get_cycle_perf();
      hist[val % div]++;
    }

    for (size_t i = 0; i < div; i++) {
      if (hist[i] > best_top) {
        best_top = hist[i];
        best_top_at = div;
      }
    }

    div--;
  } while (div > 1);

  if (best_top < TESTS * 0.9) {
    printf("[+] get_cycle_perf Estimated increment: %.1f\n",
           (double)(best_top_at * best_top) / TESTS);
  } else {
    printf("[+] get_cycle_perf Increment: %zd\n", best_top_at);
  }

  // -----------------------------------------------------------------------------------------------------------

  div = 100;
  best_top = 0;
  best_top_at = (size_t)-1;

  do {
    size_t hist[div];
    memset(hist, 0, sizeof(hist));

    for (size_t i = 0; i < TESTS; i++) {
      size_t val = rdtime();
      hist[val % div]++;
    }

    for (size_t i = 0; i < div; i++) {
      if (hist[i] > best_top) {
        best_top = hist[i];
        best_top_at = div;
      }
    }

    div--;
  } while (div > 1);

  if (best_top < TESTS * 0.9) {
    printf("[+] rdtime Estimated increment: %.1f\n",
           (double)(best_top_at * best_top) / TESTS);
  } else {
    printf("[+] rdtime Increment: %zd\n", best_top_at);
  }

  // -----------------------------------------------------------------------------------------------------------

  div = 100;
  best_top = 0;
  best_top_at = (size_t)-1;

  do {
    size_t hist[div];
    memset(hist, 0, sizeof(hist));

    for (size_t i = 0; i < TESTS; i++) {
      size_t val = get_time_perf();
      hist[val % div]++;
    }

    for (size_t i = 0; i < div; i++) {
      if (hist[i] > best_top) {
        best_top = hist[i];
        best_top_at = div;
      }
    }

    div--;
  } while (div > 1);

  if (best_top < TESTS * 0.9) {
    printf("[+] get_time_perf Estimated increment: %.1f\n",
           (double)(best_top_at * best_top) / TESTS);
  } else {
    printf("[+] get_time_perf Increment: %zd\n", best_top_at);
  }
}
