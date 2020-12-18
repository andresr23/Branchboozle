#include "primeprobe-strat.h"

#include "x86-64.h"
#include "pmc-user.h"

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

void pht3b_prime_entry_0(uint32_t prime_transition)
{
  pht3b_branch(prime_transition); // 1
  pht3b_branch(prime_transition); // 2
  pht3b_branch(prime_transition); // 3
  pht3b_branch(prime_transition); // 4
  pht3b_branch(prime_transition); // 5
  pht3b_branch(prime_transition); // 6
  pht3b_branch(prime_transition); // 7
}

void pht3b_prime_entry_1(uint32_t prime_transition)
{
  pht3b_branch(prime_transition); // 1
  pht3b_branch(prime_transition); // 2
  pht3b_branch(prime_transition); // 3
  pht3b_branch(prime_transition); // 4
  pht3b_branch(prime_transition); // 5
  pht3b_branch(prime_transition); // 6
  pht3b_branch(prime_transition); // 7

  pht3b_branch(~prime_transition); // Move away from strong state
}

void pht3b_prime_entry_2(uint32_t prime_transition)
{
  pht3b_branch(prime_transition); // 1
  pht3b_branch(prime_transition); // 2
  pht3b_branch(prime_transition); // 3
  pht3b_branch(prime_transition); // 4
  pht3b_branch(prime_transition); // 5
  pht3b_branch(prime_transition); // 6
  pht3b_branch(prime_transition); // 7

  pht3b_branch(~prime_transition); // Move away from strong state
  pht3b_branch(~prime_transition); // Move away from strong state
}

void pht3b_prime_entry_3(uint32_t prime_transition)
{
  pht3b_branch(prime_transition); // 1
  pht3b_branch(prime_transition); // 2
  pht3b_branch(prime_transition); // 3
  pht3b_branch(prime_transition); // 4
  pht3b_branch(prime_transition); // 5
  pht3b_branch(prime_transition); // 6
  pht3b_branch(prime_transition); // 7

  pht3b_branch(~prime_transition); // Move away from strong state
  pht3b_branch(~prime_transition); // Move away from strong state
  pht3b_branch(~prime_transition); // Move away from strong state
}

void (*prime_strat_array[4])(uint32_t) = {
  pht3b_prime_entry_0,
  pht3b_prime_entry_1,
  pht3b_prime_entry_2,
  pht3b_prime_entry_3,
};
