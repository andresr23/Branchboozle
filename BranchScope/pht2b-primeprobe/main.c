#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "pht2b.h"
#include "x86-64.h"
#include "pmc-user.h"

/*------------------------------ Experiment ----------------------------------*/
#define EXPERIMENT "pht-primeprobe"
/* ---------------------------------------------------------------------------*/

/* BPU. */
#define PRIME_T 0
#define PRIME_N 1
#define PROBE_T 0
#define PROBE_N 1

#define TARGET_T 0
#define TARGET_N 1

/* Repetittions to calculate rates. */
#define REPETITIONS 10000

uint32_t pht2b_branch(uint32_t bd)
{
  uint32_t pmc_s, pmc_e;
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

void test_primeprobe(uint32_t prime_transition, uint32_t target_transition, uint32_t probe_transition)
{
  float miss_rate[2] = {0};
  uint32_t miss_count[2] = {0};
  for (int r = 0; r < REPETITIONS; r++) {
    // Randomize the PHT (BranchScope)
    pht2b_r();
    // Prime
    pht2b_branch(prime_transition); // 1
    pht2b_branch(prime_transition); // 2
    pht2b_branch(prime_transition); // 3
    // Target
    pht2b_branch(target_transition);
    // Probe
    miss_count[0] += pht2b_branch(probe_transition);
    miss_count[1] += pht2b_branch(probe_transition);
  }
  // Print Prime
  if (prime_transition == PRIME_T)
    printf("TTT     | ");
  else
    printf("NNN     | ");
  // Print Target
  if (target_transition == TARGET_T)
    printf("T      | ");
  else
    printf("N      | ");
  // Print Probe
  if (probe_transition == PROBE_T)
    printf("TT         | ");
  else
    printf("NN         | ");
  // Compute Miss rates
  for (int i = 0; i < 2; i++) {
    miss_rate[i] = (float) miss_count[i] / REPETITIONS;
    print_hm(miss_rate[i]);
  }
  // Print rates
  printf(" ( %1.3f - ", miss_rate[0]);
  printf("%1.3f )\n",   miss_rate[1]);
  printf("------------------------------------------------------------\n");
}

int main()
{
  printf("Prime   | Target | Probe      | Misprediction Rates (Probe)\n");
  printf("------------------------------------------------------------\n");
  test_primeprobe(PRIME_N, TARGET_N, PROBE_N);
  test_primeprobe(PRIME_N, TARGET_N, PROBE_T);
  test_primeprobe(PRIME_N, TARGET_T, PROBE_N);
  test_primeprobe(PRIME_N, TARGET_T, PROBE_T);
  test_primeprobe(PRIME_T, TARGET_N, PROBE_N);
  test_primeprobe(PRIME_T, TARGET_N, PROBE_T);
  test_primeprobe(PRIME_T, TARGET_T, PROBE_N);
  test_primeprobe(PRIME_T, TARGET_T, PROBE_T);
  printf("(%d Repetitions)\n", REPETITIONS);
}
