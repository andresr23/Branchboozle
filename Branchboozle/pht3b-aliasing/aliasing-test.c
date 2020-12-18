#include "aliasing-test.h"

#include "x86-64.h"
#include "pmc-user.h"

__attribute__((aligned(0x1 << 26)))
void aliasing_branch_26(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_26:\n\t"
    "loop .Lloop_start_26"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 25)))
void aliasing_branch_25(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_25:\n\t"
    "loop .Lloop_start_25"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 24)))
void aliasing_branch_24(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_24:\n\t"
    "loop .Lloop_start_24"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 23)))
void aliasing_branch_23(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_23:\n\t"
    "loop .Lloop_start_23"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 22)))
void aliasing_branch_22(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_22:\n\t"
    "loop .Lloop_start_22"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 21)))
void aliasing_branch_21(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_21:\n\t"
    "loop .Lloop_start_21"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 20)))
void aliasing_branch_20(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_20:\n\t"
    "loop .Lloop_start_20"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 19)))
void aliasing_branch_19(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_19:\n\t"
    "loop .Lloop_start_19"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 18)))
void aliasing_branch_18(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_18:\n\t"
    "loop .Lloop_start_18"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 17)))
void aliasing_branch_17(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_17:\n\t"
    "loop .Lloop_start_17"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 16)))
void aliasing_branch_16(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_16:\n\t"
    "loop .Lloop_start_16"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 15)))
void aliasing_branch_15(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_15:\n\t"
    "loop .Lloop_start_15"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 14)))
void aliasing_branch_14(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_14:\n\t"
    "loop .Lloop_start_14"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 13)))
void aliasing_branch_13(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_13:\n\t"
    "loop .Lloop_start_13"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

__attribute__((aligned(0x1 << 12)))
void aliasing_branch_12(uint32_t bd)
{
  asm volatile(
    ".Lloop_start_12:\n\t"
    "loop .Lloop_start_12"
    :: "c" (HS_S) :
  );
  _8BYTE_NOP;
  _4BYTE_NOP;
  if (bd)
    _1BYTE_NOP;
}

/*-------------------------- Prime+Probe branch ------------------------------*/

__attribute__((aligned(0x1 << 26)))
uint32_t pht3b_branch_reference(uint32_t bd)
{
  uint32_t pmc_s, pmc_e;
  // History Saturation
  asm volatile(
    ".Lloop_start_ref:\n\t"
    "loop .Lloop_start_ref"
    :: "c" (HS_S) :
  );
  _RDPMC_32(pmc_s, PMC_0);
  // true (1) -> Non-Taken, false (0) -> Taken
  if (bd)
    _1BYTE_NOP;
  _RDPMC_32(pmc_e, PMC_0);
  return (pmc_e - pmc_s);
}

/*----------------------------------------------------------------------------*/
