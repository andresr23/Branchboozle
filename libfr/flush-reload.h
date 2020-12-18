#ifndef _FLUSH_RELOAD_H_
#define _FLUSH_RELOAD_H_

#include <stdint.h>
#include <stdlib.h>

/* Repetitions to calibrate Flush+Reload's timing thereshold. */
#define FR_CALIBRATE 1000000

/* Default thereshold to distinguish cache hits from misses. */
#define FR_THR_DEFAULT   120
#define FR_THR_DEFAULT_H  60
#define FR_THR_DEFAULT_M 300
// Calibration failed.
#define FR_THR_SAFE  0
#define FR_THR_ALERT 1

/*
 * State of the memory pointed by 'fr_mem'.
 * ----------------------------------------
 * FR_MEM_FREE  : 'fr_mem' is currently NULL.
 * FR_MEM_ALLOC : 'fr_mem' points to memory allocated by this library.
 * FR_MEM_EXT   : 'fr_mem' points to external memory.
 */
#define FR_MEM_FREE  0
#define FR_MEM_ALLOC 1
#define FR_MEM_EXT   2

/*
 * Offsets to implement Flush+Reload's analysis aproaches.
 * -------------------------------------------------------
 * FR_PAGESIZE : Only use a subset of cache sets at the cost of more memory.
 * FR_LINESIZE : Analyze contigous memory.
 */
#define FR_PAGESIZE 4096 // 2^12
#define FR_LINESIZE   64 // 2^8

/*-------------------------------- API ---------------------------------------*/

/* Memory management. */
void fr_mem_ext(void *, size_t);
void * fr_mem_allocate(size_t);
void * fr_mem_allocate_pages();
void * fr_mem_allocate_lines();
void fr_mem_free();

/* Calibrate Flush+Reload's timing thereshold. */
void fr_thr_calibrate();
void fr_thr_report();

/* Initial Flush of memory. */
void fr_flush_ext();
void fr_flush_pages();
void fr_flush_lines();

/* Reload and decode from transient execution. */
uint8_t fr_reload_decode_pages();
uint8_t fr_reload_decode_lines();

/*----------------------------------------------------------------------------*/

#endif /* _FLUSH_RELOAD_H_ */
