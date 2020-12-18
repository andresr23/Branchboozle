#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "x86-64.h"
#include "pmc-user.h"
#include "primeprobe-strat.h"

/* --------------------------- Exeriment's name ------------------------------*/
#define EXPERIMENT "pht3b-prime-strat"
/* ---------------------------------------------------------------------------*/

#define _PRINTFRAME printf("-------------------------------------------------------------------------------------------------------------\n")

/* BPU. */
#define PRIME_T 0
#define PRIME_N 1
#define PROBE_T 0
#define PROBE_N 1

#define TARGET_T 0
#define TARGET_N 1

/* Repetittions to calculate branch misprediction rate. */
#define REPETITIONS 1000000

uint32_t buffer[8][3] = {};

void print_hm(float rate)
{
  if (rate > 0.9)
    printf("M ");
  else if (rate < 0.1)
    printf("H ");
  else
    printf("U ");
}

void test_primeprobe(int strat, uint32_t prime_transition, uint32_t target_transition, uint32_t probe_transition)
{
  float miss_rate[7] = {};
  uint32_t miss_count[7] = {};
  for (int r = 0; r < REPETITIONS; r++) {
    // Prime
    (*prime_strat_array[strat])(prime_transition);
    // Target
    pht3b_branch(target_transition);
    // Probe
    miss_count[0] += pht3b_branch(probe_transition);
    miss_count[1] += pht3b_branch(probe_transition);
    miss_count[2] += pht3b_branch(probe_transition);
    miss_count[3] += pht3b_branch(probe_transition);
    miss_count[4] += pht3b_branch(probe_transition);
    miss_count[5] += pht3b_branch(probe_transition);
    miss_count[6] += pht3b_branch(probe_transition);
  }
  // Print Prime
  if (prime_transition == PRIME_T) {
    printf("TTTTTTT ");
    if (strat == 1)
      printf(" N   | ");
    else if(strat == 2)
      printf(" NN  | ");
    else if(strat == 3)
      printf(" NNN | ");
    else
      printf("     | ");
  } else if (prime_transition == PRIME_N) {
    printf("NNNNNNN ");
    if (strat == 1)
      printf(" T   | ");
    else if(strat == 2)
      printf(" TT  | ");
    else if(strat == 3)
      printf(" TTT | ");
    else
      printf("     | ");
  }
  // Print Target
  if (target_transition == TARGET_T)
    printf("T      | ");
  else
    printf("N      | ");
  // Print Probe
  if (probe_transition == PROBE_T)
    printf("TTTTTTT    | ");
  else
    printf("NNNNNNN    | ");
  // Compute Miss rates
  for (int i = 0; i < 7; i++) {
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
  printf("%1.3f )\n"  , miss_rate[6]);
  _PRINTFRAME;
}

int main()
{
  printf("Prime        | Target | Probe      | Mis-prediction Rates\n");
  _PRINTFRAME;
  test_primeprobe(0, PRIME_N, TARGET_N, PROBE_T);
  test_primeprobe(0, PRIME_N, TARGET_T, PROBE_T);
  test_primeprobe(0, PRIME_T, TARGET_N, PROBE_N);
  test_primeprobe(0, PRIME_T, TARGET_T, PROBE_N);
  printf("\n");
  _PRINTFRAME;
  test_primeprobe(1, PRIME_N, TARGET_N, PROBE_T);
  test_primeprobe(1, PRIME_N, TARGET_T, PROBE_T);
  test_primeprobe(1, PRIME_T, TARGET_N, PROBE_N);
  test_primeprobe(1, PRIME_T, TARGET_T, PROBE_N);
  printf("\n");
  _PRINTFRAME;
  test_primeprobe(2, PRIME_N, TARGET_N, PROBE_T);
  test_primeprobe(2, PRIME_N, TARGET_T, PROBE_T);
  test_primeprobe(2, PRIME_T, TARGET_N, PROBE_N);
  test_primeprobe(2, PRIME_T, TARGET_T, PROBE_N);

#ifndef ZEN
  printf("\n");
  _PRINTFRAME;
  test_primeprobe(3, PRIME_N, TARGET_N, PROBE_T);
  test_primeprobe(3, PRIME_N, TARGET_T, PROBE_T);
  test_primeprobe(3, PRIME_T, TARGET_N, PROBE_N);
  test_primeprobe(3, PRIME_T, TARGET_T, PROBE_N);
#endif /* Ryzen does not support strat 3. */

  printf("(%d Repetitions)\n", REPETITIONS);
}
