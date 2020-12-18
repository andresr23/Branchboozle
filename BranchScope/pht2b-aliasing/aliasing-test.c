#include "aliasing-test.h"

#include "x86-64.h"
#include "pmc-user.h"

__attribute__((aligned(0x1 << 20)))
void aliasing_branch_20(uint32_t bd)
{
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 19)))
void aliasing_branch_19(uint32_t bd)
{
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 18)))
void aliasing_branch_18(uint32_t bd)
{
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 17)))
void aliasing_branch_17(uint32_t bd)
{
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 16)))
void aliasing_branch_16(uint32_t bd)
{
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 15)))
void aliasing_branch_15(uint32_t bd)
{
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 14)))
void aliasing_branch_14(uint32_t bd)
{
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 13)))
void aliasing_branch_13(uint32_t bd)
{
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 12)))
void aliasing_branch_12(uint32_t bd)
{
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

/*-------------------------- Prime+Probe branch ------------------------------*/

__attribute__((aligned(0x1 << 20)))
uint32_t pht2b_branch(uint32_t bd)
{
  uint32_t pmc_s, pmc_e;
  _RDPMC_32(pmc_s, PMC_0);
  // true (1) -> Non-Taken, false (0) -> Taken, 0x40c029
  if (bd)
    _1BYTE_NOP;
  _RDPMC_32(pmc_e, PMC_0);
  return (pmc_e - pmc_s);
}

/*----------------------------------------------------------------------------*/
