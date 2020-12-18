#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "x86-64.h"
#include "pmc-user.h"

/*------------------------------ Experiment ----------------------------------*/
#define EXPERIMENT "sl"
/* ---------------------------------------------------------------------------*/

/* How many times to execute a branch sequence. */
#define ITERATIONS 20

/* Repetitions to calculate rates. */
#define REPETITIONS 10000

int main()
{
  uint32_t random_bits;
  uint32_t pmc_s, pmc_e;
  uint32_t buffer[ITERATIONS] = {};
  srand(time(0));
  for (int r = 0; r < REPETITIONS; r++) {
    random_bits = rand();
    for (int i = 0; i < ITERATIONS; i++) {
      _RDPMC_32(pmc_s, PMC_0);
      if ((random_bits >> 0) & 0x1)
        _1BYTE_NOP;
      if ((random_bits >> 1) & 0x1)
        _1BYTE_NOP;
      if ((random_bits >> 2) & 0x1)
        _1BYTE_NOP;
      if ((random_bits >> 3) & 0x1)
        _1BYTE_NOP;
      if ((random_bits >> 4) & 0x1)
        _1BYTE_NOP;
      if ((random_bits >> 5) & 0x1)
        _1BYTE_NOP;
      if ((random_bits >> 6) & 0x1)
        _1BYTE_NOP;
      if ((random_bits >> 7) & 0x1)
        _1BYTE_NOP;
      if ((random_bits >> 8) & 0x1)
        _1BYTE_NOP;
      if ((random_bits >> 9) & 0x1)
        _1BYTE_NOP;
      _RDPMC_32(pmc_e, PMC_0);
      buffer[i] += pmc_e - pmc_s;
    }
  }
  for (int i = 0; i < ITERATIONS; i++)
    printf("%f\n", (float) buffer[i] / REPETITIONS);
}
