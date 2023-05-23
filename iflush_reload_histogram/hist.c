#include "../rlibsc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define HISTOGRAM_ENTRIES 200
#define HISTOGRAM_SCALE 1
#define MEASUREMENTS 1000

#define LOGFILE "histogram.csv"

void __attribute__((aligned(4096), naked)) victim() {
  REP4K(asm volatile("ret");)
}

typedef void (*victim_t)(void);

size_t measure_access_time(void *address) {
  victim_t v = (victim_t)address;
  uint64_t x = rdcycle();
  v();
  uint64_t y = rdcycle();
  return y - x;
}

void measure_hits(void *address, size_t *histogram,
                  size_t number_of_measurements) {
  for (size_t i = 0; i < number_of_measurements; i++) {
    size_t hit = measure_access_time(address);
    if (hit < HISTOGRAM_ENTRIES)
      histogram[hit]++;
  }
}
‚
void measure_misses(void *address, size_t *histogram,
                    size_t number_of_measurements) {
  for (size_t i = 0; i < number_of_measurements; i++) {
    iflush(address);
    size_t miss = measure_access_time(address);
    if (miss < HISTOGRAM_ENTRIES)
      histogram[miss]++;
  }
}

size_t hit_histogram[HISTOGRAM_ENTRIES], miss_histogram[HISTOGRAM_ENTRIES];

int main(int argc, char *argv[]) {
  FILE *logfile = fopen(LOGFILE, "w+");
  if (logfile == NULL) {
    fprintf(stderr, "Error: Could not open logfile: %s\n", LOGFILE);
    return -1;
  }

  fprintf(logfile, "Time,Hit,Miss\n");

  victim();

  measure_hits(victim, hit_histogram, MEASUREMENTS);
  measure_misses(victim, miss_histogram, MEASUREMENTS);

  for (size_t i = 0; i < HISTOGRAM_ENTRIES; i += HISTOGRAM_SCALE) {
    size_t hit = 0, miss = 0;
    for (size_t scale = 0; scale < HISTOGRAM_SCALE; scale++) {
      hit += hit_histogram[i + scale];
      miss += miss_histogram[i + scale];
    }
    fprintf(stdout, "%4zu: %10zu %10zu\n", i, hit, miss);
    if (logfile != NULL) {
      fprintf(logfile, "%zu,%zu,%zu\n", i, hit, miss);
    }
  }

  fclose(logfile);

  return 0;
}
