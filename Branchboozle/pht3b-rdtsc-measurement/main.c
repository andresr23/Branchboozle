#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "x86-64.h"

/* ----------------------------- Experiment ----------------------------------*/
#define EXPERIMENT "pht3b-rdtsc-measurement"
/*----------------------------- Output files ---------------------------------*/
#define OUTPUT_FILE_0 EXPERIMENT ".dat"
/* ---------------------------------------------------------------------------*/

/* Micro-architecture specific. */
#if defined(SANDYBRIDGE) || defined(IVYBRIDGE) || defined(HASWELL)
  #define MISS_N_IDX  0
  #define HIT_N_IDX   6
  #define MISS_T_IDX  7
  #define HIT_T_IDX  13
  #define PROBE_CNT  14
#elif defined(ZEN)
  #define MISS_N_IDX  0
  #define HIT_N_IDX   4
  #define MISS_T_IDX  5
  #define HIT_T_IDX   9
  #define PROBE_CNT  10
#else /* Skylake, Kabylake */
  #define MISS_N_IDX  0
  #define HIT_N_IDX   7
  #define MISS_T_IDX  8
  #define HIT_T_IDX  15
  #define PROBE_CNT  16
#endif /* Micro-architecture specific aux macros. */

/* BPU. */
#define HS_S 100
#define PRIME_T (0x0)
#define PRIME_N (0x1)
#define PROBE_T (0x0)
#define PROBE_N (0x1)

/* Repetitions to calculate average. */
#define REPETITIONS 100000

uint32_t buffer[REPETITIONS][PROBE_CNT] = {};

void save_buffer()
{
  FILE *f = fopen(OUTPUT_FILE_0, "w");
  for(int r = 0; r < REPETITIONS; r++) {
    //                    Miss                   Hit
    fprintf(f, "%d %d " , buffer[r][MISS_N_IDX], buffer[r][HIT_N_IDX]); // Probe Non-Taken
    fprintf(f, "%d %d\n", buffer[r][MISS_T_IDX], buffer[r][HIT_T_IDX]); // Probe Taken
  }
  fclose(f);
}

uint32_t pht3b_branch_rdtsc(uint32_t bd)
{
  uint32_t tsc_s, tsc_e;
  // History Saturation
  asm volatile(
    ".Lloop_start:\n\t"
    "loop .Lloop_start"
    :: "c" (HS_S) :
  );
  _RDTSC_32(tsc_s);
  _LFENCE;
  // true (1) -> Non-Taken, false (0) -> Taken
  if (bd)
    _1BYTE_NOP;
  _RDTSCP_32(tsc_e);
  return (tsc_e - tsc_s);
}

#if defined(SANDYBRIDGE) || defined(IVYBRIDGE) || defined(HASWELL)

int main()
{
  uint32_t sum[PROBE_CNT] = {};
  // Bring to Strong Taken
  pht3b_branch_rdtsc(PRIME_T); // 1
  pht3b_branch_rdtsc(PRIME_T); // 2
  pht3b_branch_rdtsc(PRIME_T); // 3
  pht3b_branch_rdtsc(PRIME_T); // 4
  pht3b_branch_rdtsc(PRIME_T); // 5
  pht3b_branch_rdtsc(PRIME_T); // 6
  for (int r = 0; r < REPETITIONS; r++) {
    // From Strong Taken to Strong Non-Taken
    buffer[r][ 0] = pht3b_branch_rdtsc(PROBE_N); // Miss
    buffer[r][ 1] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 2] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 3] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 4] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 5] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 6] = pht3b_branch_rdtsc(PROBE_N); // Hit
    // From Strong Non-Taken to Strong Taken
    buffer[r][ 7] = pht3b_branch_rdtsc(PROBE_T); // Miss
    buffer[r][ 8] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][ 9] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][10] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][11] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][12] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][13] = pht3b_branch_rdtsc(PROBE_T); // Hit
    // Average sums
    for (int i = 0; i < PROBE_CNT; i++)
      sum[i] += buffer[r][i];
  }
  save_buffer();
  // Print report
  printf("State | Branch | Event  | Average TSC Measurement\n");
  printf("-------------------------------------------------\n");
  printf("ST    | N      | Miss   | %1.3f\n", (float) sum[ 0] / REPETITIONS);
  printf("~T    | N      | Miss   | %1.3f\n", (float) sum[ 1] / REPETITIONS);
  printf("~T    | N      | Miss   | %1.3f\n", (float) sum[ 2] / REPETITIONS);
  printf("??    | N      | X      | %1.3f\n", (float) sum[ 3] / REPETITIONS);
  printf("~N    | N      | Hit    | %1.3f\n", (float) sum[ 4] / REPETITIONS);
  printf("~N    | N      | Hit    | %1.3f\n", (float) sum[ 5] / REPETITIONS);
  printf("SN    | N      | Hit    | %1.3f\n", (float) sum[ 6] / REPETITIONS);
  printf("-------------------------------------------------\n");
  printf("SN    | T      | Miss   | %1.3f\n", (float) sum[ 7] / REPETITIONS);
  printf("~N    | T      | Miss   | %1.3f\n", (float) sum[ 8] / REPETITIONS);
  printf("~N    | T      | Miss   | %1.3f\n", (float) sum[ 9] / REPETITIONS);
  printf("??    | T      | X      | %1.3f\n", (float) sum[10] / REPETITIONS);
  printf("~T    | T      | Hit    | %1.3f\n", (float) sum[11] / REPETITIONS);
  printf("~T    | T      | Hit    | %1.3f\n", (float) sum[12] / REPETITIONS);
  printf("ST    | T      | Hit    | %1.3f\n", (float) sum[13] / REPETITIONS);
  printf("-------------------------------------------------\n");
  printf("(%d repetitions)\n", REPETITIONS);
}

#elif defined(ZEN)

int main()
{
  uint32_t sum[PROBE_CNT] = {};
  // Bring to Strong Taken
  pht3b_branch_rdtsc(PRIME_T); // 1
  pht3b_branch_rdtsc(PRIME_T); // 2
  pht3b_branch_rdtsc(PRIME_T); // 3
  pht3b_branch_rdtsc(PRIME_T); // 4
  for (int r = 0; r < REPETITIONS; r++) {
    // From Strong Taken to Strong Non-Taken
    buffer[r][ 0] = pht3b_branch_rdtsc(PROBE_N); // Miss
    buffer[r][ 1] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 2] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 3] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 4] = pht3b_branch_rdtsc(PROBE_N); // Hit
    // From Strong Non-Taken to Strong Taken
    buffer[r][ 5] = pht3b_branch_rdtsc(PROBE_T); // Miss
    buffer[r][ 6] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][ 7] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][ 8] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][ 9] = pht3b_branch_rdtsc(PROBE_T); // Hit
    // Average sums
    for (int i = 0; i < PROBE_CNT; i++)
      sum[i] += buffer[r][i];
  }
  save_buffer();
  // Print report
  printf("State | Branch | Event  | Average TSC Measurement\n");
  printf("-------------------------------------------------\n");
  printf("ST    | N      | Miss   | %1.3f\n", (float) sum[ 0] / REPETITIONS);
  printf("~T    | N      | Miss   | %1.3f\n", (float) sum[ 1] / REPETITIONS);
  printf("??    | N      | X      | %1.3f\n", (float) sum[ 2] / REPETITIONS);
  printf("~N    | N      | Hit    | %1.3f\n", (float) sum[ 3] / REPETITIONS);
  printf("SN    | N      | Hit    | %1.3f\n", (float) sum[ 4] / REPETITIONS);
  printf("-------------------------------------------------\n");
  printf("SN    | T      | Miss   | %1.3f\n", (float) sum[ 5] / REPETITIONS);
  printf("~N    | T      | Miss   | %1.3f\n", (float) sum[ 6] / REPETITIONS);
  printf("??    | T      | X      | %1.3f\n", (float) sum[ 7] / REPETITIONS);
  printf("~T    | T      | Hit    | %1.3f\n", (float) sum[ 8] / REPETITIONS);
  printf("ST    | T      | Hit    | %1.3f\n", (float) sum[ 9] / REPETITIONS);
  printf("-------------------------------------------------\n");
  printf("(%d repetitions)\n", REPETITIONS);
}

#else /* Skylake, Kabylake */

int main()
{
  uint32_t sum[PROBE_CNT] = {};
  // Bring to Strong Taken
  pht3b_branch_rdtsc(PRIME_T); // 1
  pht3b_branch_rdtsc(PRIME_T); // 2
  pht3b_branch_rdtsc(PRIME_T); // 3
  pht3b_branch_rdtsc(PRIME_T); // 4
  pht3b_branch_rdtsc(PRIME_T); // 5
  pht3b_branch_rdtsc(PRIME_T); // 6
  pht3b_branch_rdtsc(PRIME_T); // 7
  for (int r = 0; r < REPETITIONS; r++) {
    // From Strong Taken to Strong Non-Taken
    buffer[r][ 0] = pht3b_branch_rdtsc(PROBE_N); // Miss
    buffer[r][ 1] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 2] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 3] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 4] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 5] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 6] = pht3b_branch_rdtsc(PROBE_N); // .
    buffer[r][ 7] = pht3b_branch_rdtsc(PROBE_N); // Hit
    // From Strong Non-Taken to Strong Taken
    buffer[r][ 8] = pht3b_branch_rdtsc(PROBE_T); // Miss
    buffer[r][ 9] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][10] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][11] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][12] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][13] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][14] = pht3b_branch_rdtsc(PROBE_T); // .
    buffer[r][15] = pht3b_branch_rdtsc(PROBE_T); // Hit
    // Average sums
    for (int i = 0; i < PROBE_CNT; i++)
      sum[i] += buffer[r][i];
  }
  save_buffer();
  // Print report
  printf("State | Branch | Event  | Average TSC Measurement\n");
  printf("-------------------------------------------------\n");
  printf("ST    | N      | Miss   | %1.3f\n", (float) sum[ 0] / REPETITIONS);
  printf(" T    | N      | Miss   | %1.3f\n", (float) sum[ 1] / REPETITIONS);
  printf(" T    | N      | Miss   | %1.3f\n", (float) sum[ 2] / REPETITIONS);
  printf(" U    | N      | X      | %1.3f\n", (float) sum[ 3] / REPETITIONS);
  printf(" U    | N      | X      | %1.3f\n", (float) sum[ 4] / REPETITIONS);
  printf(" N    | N      | Hit    | %1.3f\n", (float) sum[ 5] / REPETITIONS);
  printf(" N    | N      | Hit    | %1.3f\n", (float) sum[ 6] / REPETITIONS);
  printf("SN    | N      | Hit    | %1.3f\n", (float) sum[ 7] / REPETITIONS);
  printf("-------------------------------------------------\n");
  printf("SN    | T      | Miss   | %1.3f\n", (float) sum[ 8] / REPETITIONS);
  printf(" N    | T      | Miss   | %1.3f\n", (float) sum[ 9] / REPETITIONS);
  printf(" N    | T      | Miss   | %1.3f\n", (float) sum[10] / REPETITIONS);
  printf(" U    | T      | X      | %1.3f\n", (float) sum[11] / REPETITIONS);
  printf(" U    | T      | X      | %1.3f\n", (float) sum[12] / REPETITIONS);
  printf(" T    | T      | Hit    | %1.3f\n", (float) sum[13] / REPETITIONS);
  printf(" T    | T      | Hit    | %1.3f\n", (float) sum[14] / REPETITIONS);
  printf("ST    | T      | Hit    | %1.3f\n", (float) sum[15] / REPETITIONS);
  printf("-------------------------------------------------\n");
  printf("(%d repetitions)\n", REPETITIONS);
}

#endif /* main */
