#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "x86-64.h"
#include "pmc-user.h"

/* ----------------------------- Experiment ----------------------------------*/
#define EXPERIMENT "hs-cycles"
/*----------------------------------------------------------------------------*/

/* Repetitions to calculate average cycle count. */
#define REPETITIONS 100000

int main()
{
  uint32_t tsc_s, tsc_e;
  uint32_t tsc_avg = 0;
  for (int r = 0; r < REPETITIONS; r++) {
    _RDTSC_32(tsc_s);
    // History Saturation
    asm volatile(
      ".Lloop_start:\n\t"
      "loop .Lloop_start"
      :: "c" (100) :
    );
    _RDTSC_32(tsc_e);
    tsc_avg += (tsc_e - tsc_s);
  }
  printf("Average latency of History Saturation (%d Repetitions): %.3f cycles\n", REPETITIONS, ((float) tsc_avg / REPETITIONS));
}
