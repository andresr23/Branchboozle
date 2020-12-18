#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "pht2b.h"
#include "x86-64.h"
#include "pmc-user.h"

/*------------------------------ Experiment ----------------------------------*/
#define EXPERIMENT "pht3b-states-bimodal"
/* ---------------------------------------------------------------------------*/

/* BPU. */
#define HS_S 100
#define PRIME_T 0
#define PRIME_N 1
#define PROBE_T 0
#define PROBE_N 1

/* Repetitions to calculate rates. */
#define REPETITIONS 10000

uint32_t pht3b_branch(uint32_t bd)
{
  uint32_t pmc_s, pmc_e;
  // History Saturation
  asm volatile(
    ".Lloop_start:\n\t"
    "loop .Lloop_start"
    :: "c" (HS_S) :
  );
  _RDPMC_32(pmc_s, PMC_0);
  // true (1) -> Non-Taken, false (0) -> Taken
  if (bd)
    _1BYTE_NOP;
  _RDPMC_32(pmc_e, PMC_0);
  return (pmc_e - pmc_s);
}

void print_hm(float rate)
{
  if (rate > 0.9)
    printf("M ");
  else if (rate < 0.1)
    printf("H ");
  else
    printf("U ");
}

void test_fsm(uint32_t prime_bit, uint32_t probe_bit)
{
  float miss_rate[8] = {};
  uint32_t miss_count[8] = {};

  for (int r = 0; r < REPETITIONS; r++) {

    // Randomize the 2-bit PHT (Branchscope)
    pht2b_r();

    // Prime SL
    pht3b_branch(prime_bit); // 1
    pht3b_branch(prime_bit); // 2
    pht3b_branch(prime_bit); // 3
    pht3b_branch(prime_bit); // 4

    // Probe SL
    miss_count[0] += pht3b_branch(probe_bit);
    miss_count[1] += pht3b_branch(probe_bit);
    miss_count[2] += pht3b_branch(probe_bit);
    miss_count[3] += pht3b_branch(probe_bit);
    miss_count[4] += pht3b_branch(probe_bit);
  }
  // Print
  if (prime_bit == PRIME_T)
    printf("TTTT      | ");
  else
    printf("NNNN      | ");
  if (probe_bit == PROBE_T)
    printf("TTTTT      | ");
  else
    printf("NNNNN      | ");
  // Compute and print miss rates
  for (int i = 0; i < 5; i++) {
    miss_rate[i] = (float) miss_count[i] / REPETITIONS;
    print_hm(miss_rate[i]);
  }
  printf(" ( %1.3f - ",  miss_rate[0]);
  printf("%1.3f - "   ,  miss_rate[1]);
  printf("%1.3f - "   ,  miss_rate[2]);
  printf("%1.3f - "   ,  miss_rate[3]);
  printf("%1.3f )\n"  ,  miss_rate[4]);
  printf("------------------------------------------------");
  printf("-----------------------------\n");
}

int main()
{
  printf("Prime     | Probe      | Misprediction Rates (Probe)\n");
  printf("------------------------------------------------");
  printf("-----------------------------\n");
  test_fsm(PRIME_N, PROBE_T);
  test_fsm(PRIME_T, PROBE_N);
  printf("(%d Repetitions)\n", REPETITIONS);
}
