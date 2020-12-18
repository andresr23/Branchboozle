#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "x86-64.h"
#include "pmc-user.h"

/* ----------------------------- Experiment ----------------------------------*/
#define EXPERIMENT "hs-implementation"
/*----------------------------------------------------------------------------*/

/* History Saturation space. */
#define SAT_SPACE 120

/* BPU. */
#define PRIME_T 0
#define PRIME_N 1
#define PROBE_T 0
#define PROBE_N 1

/* Repetitions to calculate average miss rate. */
#define REPETITIONS 10000

uint32_t pht3b_branch_loop(uint32_t bd, uint32_t s)
{
  uint32_t pmc_s, pmc_e;
  // History Saturation
  asm volatile(
    ".Lloop_start:\n\t"
    "loop .Lloop_start"
    :: "c" (s) :
  );
  _RDPMC_32(pmc_s, PMC_0);
  // true (1) -> Non-Taken, false (0) -> Taken
  if (bd)
    _1BYTE_NOP;
  _RDPMC_32(pmc_e, PMC_0);
  return (pmc_e - pmc_s);
}

uint32_t pht3b_branch_while(uint32_t bd, uint32_t s)
{
  uint32_t pmc_s, pmc_e;
  // History Saturation
  while (s)
    s--;
  _RDPMC_32(pmc_s, PMC_0);
  // true (1) -> Non-Taken, false (0) -> Taken
  if (bd)
    _1BYTE_NOP;
  _RDPMC_32(pmc_e, PMC_0);
  return (pmc_e - pmc_s);
}

uint32_t pht3b_branch_for(uint32_t bd, uint32_t s)
{
  uint32_t pmc_s, pmc_e;
  // History Saturation
  for (int _s = 0; _s < s; _s++) {}
  _RDPMC_32(pmc_s, PMC_0);
  // true (1) -> Non-Taken, false (0) -> Taken
  if (bd)
    _1BYTE_NOP;
  _RDPMC_32(pmc_e, PMC_0);
  return (pmc_e - pmc_s);
}

//------------------------------------------------------------------------------
void hs_function(uint32_t s)
{
  asm volatile(
    ".Lloop_start_function:\n\t"
    "loop .Lloop_start_function"
    :: "c" (s) :
  );
}

uint32_t pht3b_branch_function(uint32_t bd, uint32_t s)
{
  uint32_t pmc_s, pmc_e;
  // History Saturation
  hs_function(s);
  _RDPMC_32(pmc_s, PMC_0);
  // true (1) -> Non-Taken, false (0) -> Taken
  if (bd)
    _1BYTE_NOP;
  _RDPMC_32(pmc_e, PMC_0);
  return (pmc_e - pmc_s);
}
//------------------------------------------------------------------------------

int main()
{
  uint32_t buffer[4][SAT_SPACE] = {};
  // Experiment
  for (int s = 1; s <= SAT_SPACE; s++) {

    for (int r = 0; r < REPETITIONS; r++) {
      // Prime Taken
      pht3b_branch_loop(PRIME_T, s); // 1
      pht3b_branch_loop(PRIME_T, s); // 2
      pht3b_branch_loop(PRIME_T, s); // 3
      pht3b_branch_loop(PRIME_T, s); // 4
      pht3b_branch_loop(PRIME_T, s); // 5
      pht3b_branch_loop(PRIME_T, s); // 6
      pht3b_branch_loop(PRIME_T, s); // 7
      // Attempt to train Non-Taken
      pht3b_branch_loop(PRIME_N, s); // 1
      pht3b_branch_loop(PRIME_N, s); // 2
      pht3b_branch_loop(PRIME_N, s); // 3
      pht3b_branch_loop(PRIME_N, s); // 4
      pht3b_branch_loop(PRIME_N, s); // 5
      pht3b_branch_loop(PRIME_N, s); // 6
      pht3b_branch_loop(PRIME_N, s); // 7
      // Probe Taken
      buffer[0][s - 1] += pht3b_branch_loop(PROBE_T, s);
    }

    for (int r = 0; r < REPETITIONS; r++) {
      // Prime Taken
      pht3b_branch_while(PRIME_T, s); // 1
      pht3b_branch_while(PRIME_T, s); // 2
      pht3b_branch_while(PRIME_T, s); // 3
      pht3b_branch_while(PRIME_T, s); // 4
      pht3b_branch_while(PRIME_T, s); // 5
      pht3b_branch_while(PRIME_T, s); // 6
      pht3b_branch_while(PRIME_T, s); // 7
      // Attempt to train Non-Taken
      pht3b_branch_while(PRIME_N, s); // 1
      pht3b_branch_while(PRIME_N, s); // 2
      pht3b_branch_while(PRIME_N, s); // 3
      pht3b_branch_while(PRIME_N, s); // 4
      pht3b_branch_while(PRIME_N, s); // 5
      pht3b_branch_while(PRIME_N, s); // 6
      pht3b_branch_while(PRIME_N, s); // 7
      // Probe Taken
      buffer[1][s - 1] += pht3b_branch_while(PROBE_T, s);
    }

    for (int r = 0; r < REPETITIONS; r++) {
      // Prime Taken
      pht3b_branch_for(PRIME_T, s); // 1
      pht3b_branch_for(PRIME_T, s); // 2
      pht3b_branch_for(PRIME_T, s); // 3
      pht3b_branch_for(PRIME_T, s); // 4
      pht3b_branch_for(PRIME_T, s); // 5
      pht3b_branch_for(PRIME_T, s); // 6
      pht3b_branch_for(PRIME_T, s); // 7
      // Attempt to train Non-Taken
      pht3b_branch_for(PRIME_N, s); // 1
      pht3b_branch_for(PRIME_N, s); // 2
      pht3b_branch_for(PRIME_N, s); // 3
      pht3b_branch_for(PRIME_N, s); // 4
      pht3b_branch_for(PRIME_N, s); // 5
      pht3b_branch_for(PRIME_N, s); // 6
      pht3b_branch_for(PRIME_N, s); // 7
      // Probe Taken
      buffer[2][s - 1] += pht3b_branch_for(PROBE_T, s);
    }

    for (int r = 0; r < REPETITIONS; r++) {
      // Prime Taken
      pht3b_branch_function(PRIME_T, s); // 1
      pht3b_branch_function(PRIME_T, s); // 2
      pht3b_branch_function(PRIME_T, s); // 3
      pht3b_branch_function(PRIME_T, s); // 4
      pht3b_branch_function(PRIME_T, s); // 5
      pht3b_branch_function(PRIME_T, s); // 6
      pht3b_branch_function(PRIME_T, s); // 7
      // Attempt to train Non-Taken
      pht3b_branch_function(PRIME_N, s); // 1
      pht3b_branch_function(PRIME_N, s); // 2
      pht3b_branch_function(PRIME_N, s); // 3
      pht3b_branch_function(PRIME_N, s); // 4
      pht3b_branch_function(PRIME_N, s); // 5
      pht3b_branch_function(PRIME_N, s); // 6
      pht3b_branch_function(PRIME_N, s); // 7
      // Probe Taken
      buffer[3][s - 1] += pht3b_branch_function(PROBE_T, s);
    }

  }
  // Print
  for (int s = 0; s < SAT_SPACE; s++) {
    printf("%1.3f %1.3f %1.3f %1.3f\n", (float) buffer[0][s] / REPETITIONS,
                                        (float) buffer[1][s] / REPETITIONS,
                                        (float) buffer[2][s] / REPETITIONS,
                                        (float) buffer[3][s] / REPETITIONS);
  }
}
