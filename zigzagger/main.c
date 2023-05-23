#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../rlibsc.h"

#define HISTOGRAM_SCALE 1
#define HISTOGRAM_ENTRIES 50
#define HISTS 3

#define LOGFILE "histogram.csv"

size_t hists[HISTS][HISTOGRAM_ENTRIES];

// see asm.S
extern void zigzag_bench(uint64_t nb, uint64_t a, uint64_t b);

uint64_t test_zigzagger(uint64_t a, uint64_t b, uint64_t iterations) {
  uint64_t inst_before, inst_after;
  inst_before = rdinstret();
  zigzag_bench(iterations, a, b);
  inst_after = rdinstret();
  return inst_after - inst_before;
}

void measure(uint64_t a, uint64_t b, uint64_t number_measurements,
             size_t *hist) {
  for (size_t i = 0; i < number_measurements; i++) {
    uint64_t zigzagger_instr = test_zigzagger(a, b, 1);
    if (zigzagger_instr < HISTOGRAM_ENTRIES)
      hist[zigzagger_instr]++;
  }
}

void main() {
  measure(0, 0, 100000, hists[0]);
  measure(0, 1, 100000, hists[1]);
  measure(1, 0, 100000, hists[2]);

  FILE *logfile = fopen(LOGFILE, "w+");
  if (logfile == NULL) {
    fprintf(stderr, "Error: Could not open logfile: %s\n", LOGFILE);
    return;
  }

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
}
