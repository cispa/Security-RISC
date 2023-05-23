
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ARR_LEN 3000
// Use tow thresholds to filter the right interrupt band
#define THRESH_LO 1000

static inline uint64_t rdcycle() {
  uint64_t val;
  asm volatile("rdcycle %0" : "=r"(val));
  return val;
}

uint64_t event[ARR_LEN];
uint64_t delta_event[ARR_LEN];

void main() {
  uint64_t start, end, delta;
  uint64_t idx = 1;
  event[0] = rdcycle();
  printf("[+] Starting measurement\n");
  while (idx < ARR_LEN) {
    start = rdcycle();
    end = rdcycle();
    delta = end - start;
    if (delta > THRESH_LO) {
      event[idx] = start;
      delta_event[idx] = event[idx] - event[idx - 1];
      idx++;
    }
  }
  printf("[+] Measurement ended, dumping to file\n");
  FILE *logfile = fopen("log", "w");
  fprintf(logfile, "event,delta\n");
  for (size_t i = 0; i < ARR_LEN; i++) {
    fprintf(logfile, "%ld,%ld\n", event[i], delta_event[i]);
  }
  printf("[+] Results dumped\n");
}
