#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "x86-64.h"
#include "pmc-user.h"

/* ----------------------------- Experiment ----------------------------------*/
#define EXPERIMENT "hs-miss"
/*----------------------------------------------------------------------------*/

/* History Saturation space. */
#define SAT_SPACE 120

/* Repetitions to calculate average misprediction rate. */
#define REPETITIONS 10000

int main()
{
  uint32_t pmc_s, pmc_e;
  uint32_t buffer[SAT_SPACE] = {};

  for (int s = 1; s <= SAT_SPACE; s++) {
    for (int r = 0; r < REPETITIONS; r++) {
      _RDPMC_32(pmc_s, PMC_0);
      // History Saturation
      asm volatile(
        ".Lloop_start:\n\t"
        "loop .Lloop_start"
        :: "c" (s) :
      );
      _RDPMC_32(pmc_e, PMC_0);
      buffer[s - 1] += pmc_e - pmc_s;
    }
  }

  for (int s = 0; s < SAT_SPACE; s++)
    printf("%1.3f\n", (float) buffer[s] / REPETITIONS);
}
