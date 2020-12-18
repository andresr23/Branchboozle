#include <stdio.h>
#include <stdint.h>

#include "x86-64.h"
#include "flush-reload.h"

int main()
{
  char recover;
  uint8_t *mem = (uint8_t *) fr_mem_allocate_pages();
  printf(" Encode | Decode\n");
  printf("------------------\n");
  fr_thr_calibrate();
  for (int c = 65; c <= 90; c++) {
    fr_flush_pages();
    _MACCESS(mem + c * FR_PAGESIZE);
    recover = (char) fr_reload_decode_pages();
    printf("    %c   |   %c\n", (char) c, recover);
  }
  printf("------------------\n");
  fr_thr_report();
}
