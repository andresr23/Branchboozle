#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "pht2b.h"
#include "aliasing-test.h"

/*------------------------------ Experiment ----------------------------------*/
#define EXPERIMENT "pht2b-aliasing"
/* ---------------------------------------------------------------------------*/

/* BPU. */
#define TAKEN     0
#define NON_TAKEN 1

/* Repetitions to calculate frequency. */
#define REPETITIONS 10000

void (*aliasing_branch_array[9])(uint32_t) = {
  &aliasing_branch_12,
  &aliasing_branch_13,
  &aliasing_branch_14,
  &aliasing_branch_15,
  &aliasing_branch_16,
  &aliasing_branch_17,
  &aliasing_branch_18,
  &aliasing_branch_19,
  &aliasing_branch_20
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
    pht2b_r(); // Randomize the PHT (BranchScope)
    // Prime
    pht2b_branch(TAKEN); // 1
    pht2b_branch(TAKEN); // 2
    pht2b_branch(TAKEN); // 3
    // Aliasing
    aliasing_branch(NON_TAKEN); // 1
    aliasing_branch(NON_TAKEN); // 2
    aliasing_branch(NON_TAKEN); // 3
    // Probe
    miss_count[0] += pht2b_branch(NON_TAKEN);
    miss_count[1] += pht2b_branch(NON_TAKEN);
    miss_count[2] += pht2b_branch(NON_TAKEN);
    miss_count[3] += pht2b_branch(NON_TAKEN);

    /* Prime Non-Taken, Alias Taken, Probe Taken. */
    pht2b_r(); // Randomize the PHT (BranchScope)
    // Prime
    pht2b_branch(NON_TAKEN); // 1
    pht2b_branch(NON_TAKEN); // 2
    pht2b_branch(NON_TAKEN); // 3
    // Aliasing
    aliasing_branch(TAKEN); // 1
    aliasing_branch(TAKEN); // 2
    aliasing_branch(TAKEN); // 3
    // Probe
    miss_count[4] += pht2b_branch(TAKEN);
    miss_count[5] += pht2b_branch(TAKEN);
    miss_count[6] += pht2b_branch(TAKEN);
    miss_count[7] += pht2b_branch(TAKEN);
  }

  // Print Test 1
  printf("%d            | TTT        | NNN             | NNNN       | ", aliasing_bits);
  // Compute Miss Rates 1
  for (int i = 0; i < 4; i++) {
    miss_rate[i] = (float) miss_count[i] / REPETITIONS;
    print_hm(miss_rate[i]);
  }
  // Print rates 1
  printf(" ( %1.3f - ", miss_rate[0]);
  printf("%1.3f - ",    miss_rate[1]);
  printf("%1.3f - ",    miss_rate[2]);
  printf("%1.3f )\n",   miss_rate[3]);

  // Print Test 2
  printf("              | NNN        | TTT             | TTTT       | ");
  // Compute Miss Rates 2
  for (int i = 4; i < 8; i++) {
    miss_rate[i] = (float) miss_count[i] / REPETITIONS;
    print_hm(miss_rate[i]);
  }
  // Print rates 2
  printf(" ( %1.3f - ", miss_rate[4]);
  printf("%1.3f - ",    miss_rate[5]);
  printf("%1.3f - ",    miss_rate[6]);
  printf("%1.3f )\n",   miss_rate[7]);
  printf("---------------------------------------------------");
  printf("---------------------------------------------------\n");
}

int main()
{
  printf("Aliasing Bits | Prime      | Aliasing Branch | Probe      | Misprediction Rates (Probe)\n");
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
  printf("(%d Repetitions)\n", REPETITIONS);
  printf("-----------------------------\n");
  printf("aliasing_branch_12 @ %p\n", aliasing_branch_12);
  printf("aliasing_branch_13 @ %p\n", aliasing_branch_13);
  printf("aliasing_branch_14 @ %p\n", aliasing_branch_14);
  printf("aliasing_branch_15 @ %p\n", aliasing_branch_15);
  printf("aliasing_branch_16 @ %p\n", aliasing_branch_16);
  printf("aliasing_branch_17 @ %p\n", aliasing_branch_17);
  printf("aliasing_branch_18 @ %p\n", aliasing_branch_18);
  printf("aliasing_branch_19 @ %p\n", aliasing_branch_19);
  printf("aliasing_branch_20 @ %p\n", aliasing_branch_20);
  printf("-----------------------------\n");
  printf("pht2b_branch  @ %p\n", pht2b_branch);
}
