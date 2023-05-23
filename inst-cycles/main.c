#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define CT_REP 10

#define LOGFILE "timings.csv"

#define REP16(x) x x x x x x x x x x x x x x x x
#define REP256(x)                                                              \
  REP16(x)                                                                     \
  REP16(x) REP16(x) REP16(x) REP16(x) REP16(x) REP16(x) REP16(x) REP16(x)      \
      REP16(x) REP16(x) REP16(x) REP16(x) REP16(x) REP16(x) REP16(x)
#define REP1K(x) REP256(x) REP256(x) REP256(x) REP256(x)

#define MERGE_(a, b) a##b
#define LABEL_(a) SCT_MERGE_(measure_, a)
#define UNIQUE_NAME LABEL_(__LINE__)

uint64_t overhead = 0;

#if defined(__x86_64__)
static inline uint64_t rdcycle() {
  uint64_t a, d;
  asm volatile("mfence");
  asm volatile("rdtsc" : "=a"(a), "=d"(d));
  a = (d << 32) | a;
  asm volatile("mfence");
  return a;
}
#else
static inline uint64_t rdcycle() {
  uint64_t val;
  asm volatile("rdcycle %0" : "=r"(val));
  return val;
}
#endif

typedef uint64_t (*measure_func_t)(void);

typedef struct {
  measure_func_t fnc;
  const char *name;
  int cycles;
  int ct;
} function_info_t;

function_info_t functions[1000];
int function_count = 0;

#define MEASURE(instr, init, code, flags)                                      \
  uint64_t MERGE_(measure_, instr)() {                                         \
    uint64_t start = rdcycle();                                                \
    asm volatile(init "; fence; " REP1K(code) "; fence;" flags);               \
    return rdcycle() - start;                                                  \
  }                                                                            \
  void __attribute__((constructor)) MERGE_(ctor_measure_, instr)() {           \
    functions[function_count].fnc = MERGE_(measure_, instr);                   \
    functions[function_count].name = #instr;                                   \
    functions[function_count].cycles = -1;                                     \
    functions[function_count].ct = 1;                                          \
    function_count++;                                                          \
  }

#define BASE(fnc)                                                              \
  void __attribute__((constructor)) base_measurement() {                       \
    MERGE_(measure_, fnc)();                                                   \
    overhead = MERGE_(measure_, fnc)();                                        \
  }

// MEASURE(warmup, "", "", "")
MEASURE(base, "", "", "")

MEASURE(nop, "", "nop;", "")
MEASURE(mv, "", "mv a0, a1;", ::: "a0")
MEASURE(add, "", "add a0, a1, a2;", ::: "a0")
MEASURE(add_s, "", "add a1, a1, a1;", ::: "a1")
MEASURE(sub, "", "sub a0, a1, a2;", ::: "a0")
MEASURE(subw, "", "subw a0, a1, a2;", ::: "a0")
MEASURE(addi, "", "addi a0, a1, 1337;", ::: "a0")
MEASURE(addiw, "", "addiw a0, a1, 1337;", ::: "a0")
MEASURE(addw, "", "addw a0, a1, 1337;", ::: "a0")
MEASURE(slt, "", "slt a0, a1, a2;", ::: "a0")
MEASURE(slti, "", "slti a0, a1, 1337;", ::: "a0")
MEASURE(sltu, "", "sltu a0, a1, a2;", ::: "a0")
MEASURE(sltiu, "", "sltiu a0, a1, 1337;", ::: "a0")
MEASURE(lui, "", "lui a0, 1337;", ::: "a0")
// MEASURE(auip, "", "auip a0, 1337;", ::: "a0")

MEASURE(and, "", "and a0, a1, a2;", ::: "a0")
MEASURE(xor, "", "xor a0, a1, a2;", ::: "a0")
MEASURE(or, "", "or a0, a1, a2;", ::: "a0")
MEASURE(andi, "", "andi a0, a1, 1337;", ::: "a0")
MEASURE(xori, "", "xori a0, a1, 1337;", ::: "a0")
MEASURE(ori, "", "ori a0, a1, 1337;", ::: "a0")
MEASURE(sll, "", "sll a0, a1, a2;", ::: "a0")
MEASURE(srl, "", "srl a0, a1, a2;", ::: "a0")
MEASURE(slli, "", "slli a0, a1, 15;", ::: "a0")
MEASURE(slliw, "", "slliw a0, a1, 15;", ::: "a0")
MEASURE(sllw, "", "sllw a0, a1, 15;", ::: "a0")
MEASURE(srli, "", "srli a0, a1, 15;", ::: "a0")
MEASURE(srliw, "", "srliw a0, a1, 15;", ::: "a0")
MEASURE(srlw, "", "srlw a0, a1, a2;", ::: "a0")
MEASURE(srai, "", "srai a0, a1, 15;", ::: "a0")
MEASURE(sraiw, "", "sraiw a0, a1, 15;", ::: "a0")
MEASURE(sra, "", "sra a0, a1, a2;", ::: "a0")
MEASURE(sraw, "", "sraw a0, a1, a2;", ::: "a0")

// MEASURE(csrrci, "", "csrrci a0, 0, 0;", ::: "a0")
MEASURE(fence, "", "fence iorw,iorw;", ::: "a0")
MEASURE(fencei, "", "fence.i;", ::: "a0")

// MEASURE(wfi, "", "wfi;", ::: "a0")

MEASURE(div, "", "div a0, a1, a2;", ::: "a0")
MEASURE(divu, "", "divu a0, a1, a2;", ::: "a0")
MEASURE(divuw, "", "divuw a0, a1, a2;", ::: "a0")
MEASURE(divw, "", "divw a0, a1, a2;", ::: "a0")

MEASURE(mul, "", "mul a0, a1, a2;", ::: "a0")
MEASURE(mulh, "", "mulh a0, a1, a2;", ::: "a0")
MEASURE(mulhsu, "", "mulhsu a0, a1, a2;", ::: "a0")
MEASURE(mulhu, "", "mulhu a0, a1, a2;", ::: "a0")
MEASURE(mulw, "", "mulw a0, a1, a2;", ::: "a0")

MEASURE(rem, "", "rem a0, a1, a2;", ::: "a0")
MEASURE(remu, "", "remu a0, a1, a2;", ::: "a0")
MEASURE(remuw, "", "remuw a0, a1, a2;", ::: "a0")
MEASURE(remw, "", "remw a0, a1, a2;", ::: "a0")

MEASURE(fadds, "", "fadd.s f0, f1, f2;", ::: "f0")
MEASURE(fclasss, "", "fclass.s a0, f0;", ::: "f0")
MEASURE(fdivs, "", "fdiv.s f0, f1, f2;", ::: "f0")
MEASURE(feqs, "", "feq.s a0, f1, f2;", ::: "a0")

MEASURE(cadd, "", "c.add a0, a1;", ::: "a0")
MEASURE(caddi, "", "c.addi a0, 15;", ::: "a0")
MEASURE(caddiw, "", "c.addiw a0, 15;", ::: "a0")
MEASURE(caddw, "", "c.addw a0, a1;", ::: "a0")
MEASURE(cand, "", "c.and a0, a1;", ::: "a0")
MEASURE(candi, "", "c.andi a0, 15;", ::: "a0")
MEASURE(cli, "", "c.li a0, 15;", ::: "a0")
MEASURE(clui, "", "c.lui a0, 15;", ::: "a0")
MEASURE(cmv, "", "c.mv a0, a1;", ::: "a0")
MEASURE(cnop, "", "c.nop;", "")
MEASURE(cor, "", "c.or a0, a1;", ::: "a0")
MEASURE(cslli, "", "c.slli a0, 3;", ::: "a0")
MEASURE(csrai, "", "c.srai a0, 3;", ::: "a0")
MEASURE(csrli, "", "c.srli a0, 3;", ::: "a0")
MEASURE(csub, "", "c.sub a0, a1;", ::: "a0")
MEASURE(csubw, "", "c.subw a0, a1;", ::: "a0")
MEASURE(cxor, "", "c.xor a0, a1;", ::: "a0")

BASE(base)

int main(int argc, char *argv[]) {
  FILE *logfile = fopen(LOGFILE, "w+");
  if (logfile == NULL) {
    fprintf(stderr, "Error: Could not open logfile: %s\n", LOGFILE);
    return -1;
  }

  fprintf(logfile, "Instr,Time\n");
  srand(time(NULL));

  for (int rep = 0; rep < CT_REP; rep++) {
    for (int i = 0; i < function_count; i++) {
      functions[i].fnc();
      asm volatile("mv a1, %0" : : "r"(rand()));
      asm volatile("mv a2, %0" : : "r"(rand()));

      uint64_t cycles = ((functions[i].fnc() - overhead) + 1023) / 1024;
      if (functions[i].cycles != -1 && functions[i].cycles != cycles) {
        functions[i].ct = 0;
        //             printf("%s: %d != %zd\n", functions[i].name,
        //             functions[i].cycles, cycles);
      }
      functions[i].cycles = cycles;
    }
  }

  for (int i = 0; i < function_count; i++) {
    printf("%-8s: %3zd %s\n", functions[i].name, functions[i].cycles,
           functions[i].ct ? "" : "! not CT !");
    fprintf(logfile, "%s,%zd\n", functions[i].name, functions[i].cycles);
  }

  fclose(logfile);

  return 0;
}
