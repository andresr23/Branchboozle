#include <stdio.h>
#include <stdint.h>

int main()
{
  uint32_t eax = 0x0A;
  asm volatile("cpuid" : "+a" (eax) :: "rbx", "rcx", "rdx");
  printf("Performance Monitoring\n");
  printf("----------------------\n");
  printf("Version        | %d\n", (eax >>  0) & 0xFF);
  printf("PMCs available | %d\n", (eax >>  8) & 0xFF);
}
