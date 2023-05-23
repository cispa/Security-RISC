#include "../rlibsc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define HISTOGRAM_ENTRIES 1500
#define HISTOGRAM_SCALE 10
#define MEASUREMENTS 1000
#define HISTS 5

#define LOGFILE "histogram.csv"

char __attribute__((aligned(4096))) buffer[4096 * 32];

size_t hists[HISTS][HISTOGRAM_ENTRIES];

typedef void (*prime_t)(void *);

#define prime_generic                                                          \
  size_t pset = (((size_t)addr) >> 6) & 63;                                    \
  int pn = ((size_t)addr) & 4096;                                              \
  int bufpn = ((size_t)buffer) & 4096;                                         \
  int i = 1;

static inline void prime1(void *addr) {
  prime_generic REP1(maccess(buffer + i++ * 2 * 4096 + (pset << 6));)
}

static inline void prime2(void *addr) {
  prime_generic REP2(maccess(buffer + i++ * 2 * 4096 + (pset << 6));)
}

static inline void prime3(void *addr) {
  prime_generic REP3(maccess(buffer + i++ * 2 * 4096 + (pset << 6));)
}

static inline void prime4(void *addr) {
  prime_generic REP4(maccess(buffer + i++ * 2 * 4096 + (pset << 6));)
}

static inline void prime5(void *addr) {
  prime_generic REP5(maccess(buffer + i++ * 2 * 4096 + (pset << 6));)
}

static inline void prime6(void *addr) {
  prime_generic REP6(maccess(buffer + i++ * 2 * 4096 + (pset << 6));)
}

static inline void rprime4(void *addr) {
  prime_generic i += 4;
  REP4(maccess(buffer + i-- * 2 * 4096 + (pset << 6));)
}

size_t measure_access_time(void *address) {
  uint64_t x = rdcycle();
  rprime4(address);
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
                    size_t number_of_measurements, prime_t primer) {
  for (size_t i = 0; i < number_of_measurements; i++) {
    primer(address);
    size_t miss = measure_access_time(address);
    if (miss < HISTOGRAM_ENTRIES)
      histogram[miss]++;
  }
}

char __attribute__((aligned(4096))) address[4096];

int main(int argc, char *argv[]) {
  FILE *logfile = fopen(LOGFILE, "w+");
  if (logfile == NULL) {
    fprintf(stderr, "Error: Could not open logfile: %s\n", LOGFILE);
    return -1;
  }

  fprintf(logfile, "Time");
  for (int h = 0; h < HISTS; h++) {
    fprintf(logfile, ",hist%d", h);
  }
  fprintf(logfile, "\n");

  memset(address, 1, 4096);
  memset(buffer, 2, sizeof(buffer));

  measure_hits(address, hists[0], MEASUREMENTS);
  measure_misses(address, hists[1], MEASUREMENTS, prime1);
  measure_misses(address, hists[2], MEASUREMENTS, prime2);
  measure_misses(address, hists[3], MEASUREMENTS, prime3);
  measure_misses(address, hists[4], MEASUREMENTS, prime4);

  for (size_t i = 0; i < HISTOGRAM_ENTRIES; i += HISTOGRAM_SCALE) {
    size_t sums[HISTS];
    memset(sums, 0, sizeof(sums));
    for (size_t scale = 0; scale < HISTOGRAM_SCALE; scale++) {
      for (int h = 0; h < HISTS; h++) {
        sums[h] += hists[h][i + scale];
      }
    }
    fprintf(stdout, "%4zu: ", i);
    for (int h = 0; h < HISTS; h++) {
      fprintf(stdout, "%10zu ", sums[h]);
    }
    printf("\n");
    if (logfile != NULL) {
      fprintf(logfile, "%zu", i);
      for (int h = 0; h < HISTS; h++) {
        fprintf(logfile, ",%zu", sums[h]);
      }
      fprintf(logfile, "\n");
    }
  }

  fclose(logfile);

  return 0;
}
