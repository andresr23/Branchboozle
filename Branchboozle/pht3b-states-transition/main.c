#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "pht2b.h"
#include "x86-64.h"
#include "pmc-user.h"

/*------------------------------ Experiment ----------------------------------*/
#define EXPERIMENT "pht3b-states-transition"
/* ---------------------------------------------------------------------------*/

/* BPU. */
#define HS_S 100
#define PRIME_T 0
#define PRIME_N 1
#define PROBE_T 0
#define PROBE_N 1

/* Repetitions to calculate rates. */
#define REPETITIONS 10000

/* Rounds to observe the transition. */
#define ROUNDS 20

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

void print_hm(uint32_t count)
{
  float rate = (float) count / REPETITIONS;
  if (rate > 0.9)
    printf("M ");
  else if (rate < 0.1)
    printf("H ");
  else
    printf("U ");
}

void test_fsm(uint32_t prime_transition, uint32_t probe_transition)
{
  uint32_t buffer[ROUNDS][8] = {};
  for (int s = 0; s < ROUNDS; s++) {
    for (int r = 0; r < REPETITIONS; r++) {
      // Randomize the PHT (Branchscope)
      pht2b_r();
      // Dummy Prime+Probe rounds
      for (int i = 0; i < s; i++) {
        // Prime SL
        pht3b_branch(prime_transition); // 1
        pht3b_branch(prime_transition); // 2
        pht3b_branch(prime_transition); // 3
        pht3b_branch(prime_transition); // 4
        pht3b_branch(prime_transition); // 5
        pht3b_branch(prime_transition); // 6
        pht3b_branch(prime_transition); // 7
        // Probe SL
        pht3b_branch(probe_transition);
        pht3b_branch(probe_transition);
        pht3b_branch(probe_transition);
        pht3b_branch(probe_transition);
        pht3b_branch(probe_transition);
        pht3b_branch(probe_transition);
        pht3b_branch(probe_transition);
        pht3b_branch(probe_transition);
      }
      // Prime SL
      pht3b_branch(prime_transition); // 1
      pht3b_branch(prime_transition); // 2
      pht3b_branch(prime_transition); // 3
      pht3b_branch(prime_transition); // 4
      pht3b_branch(prime_transition); // 5
      pht3b_branch(prime_transition); // 6
      pht3b_branch(prime_transition); // 7
      // Probe SL
      buffer[s][0] += pht3b_branch(probe_transition);
      buffer[s][1] += pht3b_branch(probe_transition);
      buffer[s][2] += pht3b_branch(probe_transition);
      buffer[s][3] += pht3b_branch(probe_transition);
      buffer[s][4] += pht3b_branch(probe_transition);
      buffer[s][5] += pht3b_branch(probe_transition);
      buffer[s][6] += pht3b_branch(probe_transition);
      buffer[s][7] += pht3b_branch(probe_transition);
    }
  }
  for (int s = 0; s < ROUNDS; s++) {
    printf(" %d\t| ", s);
    for (int i = 0; i < 8; i++) {
      print_hm(buffer[s][i]);
    }
    printf("\n");
  }
  printf("-------------------------\n");
}

int main()
{
  printf("Rounds\t| States\n");
  printf("-------------------------\n");
  test_fsm(PRIME_N, PROBE_T);
  printf("(%d Repetitions)\n", REPETITIONS);
}
