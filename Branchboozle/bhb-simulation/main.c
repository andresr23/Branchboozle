#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* ----------------------------- Experiment ----------------------------------*/
#define EXPERIMENT "bhb-simulation"
/*----------------------------------------------------------------------------*/

#define PRINT_T printf("'loop' Ins. VA   | BHB Intial State |  n | Iter. (n - 1)   | Iter. (n)        | Iter. (n + 1)    | Iter. (n + 2)    | Iter. (n + 3)    |\n")
#define PRINT_F printf("----------------------------------------------------------------------------------------------------------------------------------------\n")

#define _N 25

#define BIT_MASK_58 0x03FFFFFFFFFFFFFFUL
#define BIT_MASK_48 0x0000FFFFFFFFFFFFUL

uint64_t bhb = 0UL;

uint64_t bhb_h[5] = {0};

void update_bhb(uint64_t src, uint64_t dst)
{
  // Shift
  bhb <<= 2;
  // Destination XORing
  bhb ^= (dst & 0x3FUL);          // bhb[ 5: 0] ^= dst[ 5: 0]
  // Source XORing
  bhb ^= (src & 0x000C0UL) >>  6; // bhb[ 1: 0] ^= src[ 7: 6]
  bhb ^= (src & 0x00C00UL) >>  8; // bhb[ 3: 2] ^= src[11:10]
  bhb ^= (src & 0x0C000UL) >> 10; // bhb[ 5: 4] ^= src[15:14]
  bhb ^= (src & 0x00030UL) <<  2; // bhb[ 7: 6] ^= src[ 5: 4]
  bhb ^= (src & 0x00300UL) <<  0; // bhb[ 9: 8] ^= src[ 9: 8]
  bhb ^= (src & 0x03000UL) >>  2; // bhb[11:10] ^= src[13:12]
  bhb ^= (src & 0x30000UL) >>  4; // bhb[13:12] ^= src[17:16]
  bhb ^= (src & 0xC0000UL) >>  4; // bhb[15:14] ^= src[19:18]
  // Mask
  bhb &= BIT_MASK_58;
}

void print_bits(uint64_t reg, int bits)
{
  while (bits > -1) {
    if ((reg >> bits) & 0x1)
      printf("1");
    else
      printf("0");
    bits--;
  }
  printf(" ");
}

uint64_t new_address()
{
  uint64_t h, l;
  uint64_t addr;
  // Random
  h = (uint64_t) rand();
  l = (uint64_t) rand();
  // Concatenate
  addr = (h << 32) | l;
  // Verify that 'addr' is not in the NULL page
  if (addr < 4096)
    addr += 4096;
  return (addr & BIT_MASK_48);
}

void randomize_bhb()
{
  uint64_t h, l;
  uint64_t addr;
  // Random
  h = (uint64_t) rand();
  l = (uint64_t) rand();
  // Concatenate
  bhb = ((h << 32) | l) & BIT_MASK_58;
}

int bhb_is_not_saturated()
{
  int n = 4;
  // Track History
  for (int i = 4; i > 0; i--)
    bhb_h[i] = bhb_h[i - 1];
  bhb_h[0] = bhb;
  // Check if saturated
  while (--n > 0 && bhb_h[n] == bhb_h[0]);
  return (n != 0);
}

int main()
{
  int count;
  uint64_t branch_s, branch_d;
  srand(time(0));
  PRINT_T;
  PRINT_F;
  for (int j = 0; j < 20; j++) {
    count = 0;
    // Get new address and simulate a 'loop' that jumps to itself.
    branch_s = branch_d = new_address();
    // Randomize the BHB.
    randomize_bhb();
    // Print 'loop' address and initial BHB state.
    printf("%016lx | ", branch_s);
    printf("%016lx | ", bhb);
    // Taken until saturated.
    while (bhb_is_not_saturated()) {
      update_bhb(branch_s, branch_d);
      count++;
    }
    // BHB is stagnant.
    printf("%02d |", count - 3);
    for (int i = 4; i >= 0; i--)
      printf("%016lx | ", bhb_h[i]);
    printf("\n");
    PRINT_F;
  }
}
