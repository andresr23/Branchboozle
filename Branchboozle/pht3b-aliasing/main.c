#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "aliasing-test.h"

/*------------------------------ Experiment ----------------------------------*/
#define EXPERIMENT "pht3b-aliasing"
/* ---------------------------------------------------------------------------*/

/* BPU. */
#define PRIME_T 0
#define PRIME_N 1
#define PROBE_T 0
#define PROBE_N 1

#define ALIASING_T 0
#define ALIASING_N 1

/* Repetitions to calculate frequency. */
#define REPETITIONS 100000

void (*aliasing_branch_array[15])(uint32_t) = {
  &aliasing_branch_12,
  &aliasing_branch_13,
  &aliasing_branch_14,
  &aliasing_branch_15,
  &aliasing_branch_16,
  &aliasing_branch_17,
  &aliasing_branch_18,
  &aliasing_branch_19,
  &aliasing_branch_20,
  &aliasing_branch_21,
  &aliasing_branch_22,
  &aliasing_branch_23,
  &aliasing_branch_24,
  &aliasing_branch_25,
  &aliasing_branch_26
};

void print_hm(float rate)
{
  if (rate > 0.9)
    printf("M ");
  else if (rate < 0.1)
    printf("H ");
  else
    printf("U ");
}

void test_aliasing(uint32_t aliasing_bits)
{
  float miss_rate[8] = {};
  uint32_t miss_count[8] = {};

  void (*aliasing_branch)(uint32_t) = aliasing_branch_array[aliasing_bits - 12];

  for (int r = 0; r < REPETITIONS; r++) {
    /* Prime Taken, Alias Non-Taken, Probe Non-Taken. */
    // Prime
    pht3b_branch_reference(PRIME_T); // 1
    pht3b_branch_reference(PRIME_T); // 2
    pht3b_branch_reference(PRIME_T); // 3
    pht3b_branch_reference(PRIME_T); // 4
    pht3b_branch_reference(PRIME_T); // 5
    pht3b_branch_reference(PRIME_T); // 6
    pht3b_branch_reference(PRIME_T); // 7
    // Aliasing
    aliasing_branch(ALIASING_N); // 1
    aliasing_branch(ALIASING_N); // 2
    aliasing_branch(ALIASING_N); // 3
    aliasing_branch(ALIASING_N); // 4
    aliasing_branch(ALIASING_N); // 5
    aliasing_branch(ALIASING_N); // 6
    aliasing_branch(ALIASING_N); // 7
    // Probe
    miss_count[0] += pht3b_branch_reference(PROBE_N);
    miss_count[1] += pht3b_branch_reference(PROBE_N);
    miss_count[2] += pht3b_branch_reference(PROBE_N);
    miss_count[3] += pht3b_branch_reference(PROBE_N);

    /* Prime Non-Taken, Alias Taken, Probe Taken. */
    // Prime
    pht3b_branch_reference(PRIME_N); // 1
    pht3b_branch_reference(PRIME_N); // 2
    pht3b_branch_reference(PRIME_N); // 3
    pht3b_branch_reference(PRIME_N); // 4
    pht3b_branch_reference(PRIME_N); // 5
    pht3b_branch_reference(PRIME_N); // 6
    pht3b_branch_reference(PRIME_N); // 7
    // Aliasing
    aliasing_branch(ALIASING_T); // 1
    aliasing_branch(ALIASING_T); // 2
    aliasing_branch(ALIASING_T); // 3
    aliasing_branch(ALIASING_T); // 4
    aliasing_branch(ALIASING_T); // 5
    aliasing_branch(ALIASING_T); // 6
    aliasing_branch(ALIASING_T); // 7
    // Probe
    miss_count[4] += pht3b_branch_reference(PROBE_T);
    miss_count[5] += pht3b_branch_reference(PROBE_T);
    miss_count[6] += pht3b_branch_reference(PROBE_T);
    miss_count[7] += pht3b_branch_reference(PROBE_T);
  }

  // Print
  printf("%d            | TTTTTTT    | NNNNNNN         | NNNN       | ", aliasing_bits);

  for (int i = 0; i < 4; i++) {
    miss_rate[i] = (float) miss_count[i] / REPETITIONS;
    print_hm(miss_rate[i]);
  }

  printf(" ( %1.3f - ", miss_rate[0]);
  printf("%1.3f - "   , miss_rate[1]);
  printf("%1.3f - "   , miss_rate[2]);
  printf("%1.3f )\n"  , miss_rate[3]);

  printf("              | NNNNNNN    | TTTTTTT         | TTTT       | ");

  for (int i = 4; i < 8; i++) {
    miss_rate[i] = (float) miss_count[i] / REPETITIONS;
    print_hm(miss_rate[i]);
  }

  printf(" ( %1.3f - ", miss_rate[4]);
  printf("%1.3f - "   , miss_rate[5]);
  printf("%1.3f - "   , miss_rate[6]);
  printf("%1.3f )\n"  , miss_rate[7]);
  printf("---------------------------------------------------");
  printf("---------------------------------------------------\n");
}

int main()
{
  printf("Aliasing Bits | Prime      | Aliasing Branch | Probe      | Mis-prediction Rates (Probe)\n");
  printf("---------------------------------------------------");
  printf("---------------------------------------------------\n");
  test_aliasing(12);
  test_aliasing(13);
  test_aliasing(14);
  test_aliasing(15);
  test_aliasing(16);
  test_aliasing(17);
  test_aliasing(18);
  test_aliasing(19);
  test_aliasing(20);
  test_aliasing(21);
  test_aliasing(22);
  test_aliasing(23);
  test_aliasing(24);
  test_aliasing(25);
  test_aliasing(26);
  printf("(%d Repetitions)\n", REPETITIONS);
  printf("-----------------------------------\n");

  for (int i = 0; i < 15; i++)
    printf("aliasing_branch_%d @ %p\n", (i + 12), aliasing_branch_array[i]);

  printf("-----------------------------------\n");
  printf("pht3b_branch_reference @ %p\n", pht3b_branch_reference);
}
