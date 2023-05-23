#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <sched.h>



void victim() {
    asm volatile ("j l1\n\t"
                  "nop\n\t"
                  "nop\n\t"
                  "nop\n\t"
                  "nop\n\t"
                  "nop\n\t"
                  "nop\n\t"
                  "ret\n\t"
                  "ret\n\t"
                  "ret\n\t"
                  "nop\n\t"
                  "l1:nop\n\t");
}

size_t start, end;
size_t start_instr, end_instr, delta_instr;
size_t delta;

static jmp_buf trycatch_buf;
void unblock_signal(int signum __attribute__((__unused__))) {
  sigset_t sigs;
  sigemptyset(&sigs);
  sigaddset(&sigs, signum);
  sigprocmask(SIG_UNBLOCK, &sigs, NULL);
}

static inline uint64_t rdtsc() {
  uint64_t val;
  asm volatile ("rdcycle %0" : "=r"(val));
  //asm volatile("nop");
  return val;
}

static inline size_t rdinstret() {
  size_t val;
  asm volatile("rdinstret %0" : "=r"(val));
  return val;
}

void trycatch_segfault_handler(int signum) {
  (void)signum;
  end = rdtsc();
  end_instr = rdinstret();
  if (start) {
    delta = end - start;
    delta_instr = end_instr - start_instr;
  }
  unblock_signal(SIGSEGV);
  unblock_signal(SIGILL);
  longjmp(trycatch_buf, 1);
}

int flush_reload_t(void *ptr) {
  uint64_t s1 = 0, s2 = 0;

  s1 = rdtsc();
  ((void(*)())ptr)();
  s2 = rdtsc();
  return (int)(s2 - s1);
}


static inline void flush(void* addr) {

  asm volatile("xor a7, a7, a7\n"
               "add a7, a7, %0\n"
               "fence.i\n\t"
  : : "r"(addr) : "a7","memory");
}


void dummy() {
  asm volatile("nop");
}

typedef void (*fnc)();
int main(int argc, char* argv[]) {
  start = 0;
  end = 0;
  char* ptr = (char*)((size_t)victim + 14);

  FILE* fd = fopen("./flush-ret.csv", "w");
  for (size_t j = 0; j < 1000; j++) {
    for (size_t i = 0; i < 100; i++) {
      int cached = i % 2 == 0;
      fnc target = (fnc) (cached * (size_t) victim + (1 - cached) * (size_t) dummy);
      target();

      size_t value = flush_reload_t(ptr);
      if (j > 20) {
        fprintf(fd, "%zu,%d,%zu\n", i, cached, value);
      }
      flush(ptr);
    }
  }

  fclose(fd);
  return 0;
}
