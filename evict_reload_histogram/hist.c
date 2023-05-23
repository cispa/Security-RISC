#include "../rlibsc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define HISTOGRAM_ENTRIES 150
#define HISTOGRAM_SCALE 1
#define MEASUREMENTS 1000

#define LOGFILE "histogram.csv"

char __attribute__((aligned(4096))) buffer[4 * 1024 * 1024];

#ifdef C906
static inline void evict(void *addr) {
  size_t pset = (((size_t)addr) >> 6) & 63;
  int pn = (((size_t)addr) >> 6) & 64;
  int bufpn = (((size_t)buffer) >> 6) & 64;
  int i = 1;
  int j = (pn & 64 == bufpn) ? 0 : 1;
  REP4(maccess(buffer + i++ * 2 * 4096 + j * 4096 + (pset << 6));)
}
#elif defined(U74)

#define C_VAL 2
#define D_VAL 1
#define S_VAL 10

static inline void evict(void *addr) {
  size_t start =
      (((size_t)(buffer) >> 14) << 14) + 0x4000 + ((size_t)addr & 0x3fff);
#define S 14
  for (int s = 0; s < S_VAL; s += 1) {
    for (int d = 0; d < D_VAL; d++) {
      for (int c = 0; c < C_VAL; c++) {
        maccess((void *)(start + ((s + c) << S)));
      }
    }
  }
}
#endif

size_t measure_access_time(void *address) {
  uint64_t x = rdcycle();
  maccess(address);
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

void measure_misses(void *address, size_t *histogram,
                    size_t number_of_measurements) {
  for (size_t i = 0; i < number_of_measurements; i++) {
    evict(address);
    size_t miss = measure_access_time(address);
    if (miss < HISTOGRAM_ENTRIES)
      histogram[miss]++;
  }
}

size_t hit_histogram[HISTOGRAM_ENTRIES], miss_histogram[HISTOGRAM_ENTRIES];
char __attribute__((aligned(4096))) address[4096];

int main(int argc, char *argv[]) {
  FILE *logfile = fopen(LOGFILE, "w+");
  if (logfile == NULL) {
    fprintf(stderr, "Error: Could not open logfile: %s\n", LOGFILE);
    return -1;
  }

  fprintf(logfile, "Time,Hit,Miss\n");

  memset(address, 1, 4096);
  memset(buffer, 2, sizeof(buffer));

  measure_hits(address, hit_histogram, MEASUREMENTS);
  measure_misses(address, miss_histogram, MEASUREMENTS);

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
