#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "x86-64.h"
#include "pmc-user.h"

/*------------------------------ Experiment ----------------------------------*/
#define EXPERIMENT "pht3b-states-history"
/* ---------------------------------------------------------------------------*/

/* BPU. */
#define HS_S 100
#define PRIME_T 0
#define PRIME_N 1
#define PROBE_T 0
#define PROBE_N 1

/* Repetitions to calculate rates. */
#define REPETITIONS 1000000

uint32_t pht3b_branch(uint32_t br)
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
  if (br)
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

void test_fsm(uint32_t prime_transition, uint32_t probe_transition)
{
  float miss_rate[8] = {};
  uint32_t miss_count[8] = {};
  for (int r = 0; r < REPETITIONS; r++) {
    // Prime
    pht3b_branch(prime_transition); // 1
    pht3b_branch(prime_transition); // 2
    pht3b_branch(prime_transition); // 3
    pht3b_branch(prime_transition); // 4
    pht3b_branch(prime_transition); // 5
    pht3b_branch(prime_transition); // 6
    pht3b_branch(prime_transition); // 7
    // Probe
    miss_count[0] += pht3b_branch(probe_transition);
    miss_count[1] += pht3b_branch(probe_transition);
    miss_count[2] += pht3b_branch(probe_transition);
    miss_count[3] += pht3b_branch(probe_transition);
    miss_count[4] += pht3b_branch(probe_transition);
    miss_count[5] += pht3b_branch(probe_transition);
    miss_count[6] += pht3b_branch(probe_transition);
    miss_count[7] += pht3b_branch(probe_transition);
  }
  // Print Prime
  if (prime_transition == PRIME_T)
    printf("TTTTTTT   | ");
  else
    printf("NNNNNNN   | ");
  // Print Probe
  if (probe_transition == PROBE_T)
    printf("TTTTTTTT   | ");
  else
    printf("NNNNNNNN   | ");
  // Compute Miss rates
  for (int i = 0; i < 8; i++) {
    miss_rate[i] = (float) miss_count[i] / REPETITIONS;
    print_hm(miss_rate[i]);
  }
  // Print rates
  printf(" ( %1.3f - ", miss_rate[0]);
  printf("%1.3f - "   , miss_rate[1]);
  printf("%1.3f - "   , miss_rate[2]);
  printf("%1.3f - "   , miss_rate[3]);
  printf("%1.3f - "   , miss_rate[4]);
  printf("%1.3f - "   , miss_rate[5]);
  printf("%1.3f - "   , miss_rate[6]);
  printf("%1.3f )\n"  , miss_rate[7]);
  printf("------------------------------------------------------");
  printf("-----------------------------------------------------\n");
}

int main()
{
  printf("Prime     | Probe      | Misprediction Rates (Probe)\n");
  printf("------------------------------------------------------");
  printf("-----------------------------------------------------\n");
  test_fsm(PRIME_N, PROBE_T);
  test_fsm(PRIME_T, PROBE_N);
  printf("(%d Repetitions)\n", REPETITIONS);
}
