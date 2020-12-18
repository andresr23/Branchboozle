#include <stdio.h>
#include <stdint.h>

#include "pht2b.h"
#include "x86-64.h"

/*------------------------------ Experiment ----------------------------------*/
#define EXPERIMENT "pht2b-randomize-cycles"
/* ---------------------------------------------------------------------------*/

/* Repetitions to calculate average cycle count. */
#define REPETITIONS 10000

int main()
{
  uint32_t tsc_s, tsc_e;
  uint32_t tsc_avg = 0;
  for (int r = 0; r < REPETITIONS; r++) {
    _RDTSC_32(tsc_s);
    pht2b_r();
    _RDTSC_32(tsc_e);
    tsc_avg += (tsc_e - tsc_s);
  }
  printf("Average latency of \'randomize_pht\' (%d Repetitions): %.3f cycles\n", REPETITIONS, ((float) tsc_avg / REPETITIONS));
}
