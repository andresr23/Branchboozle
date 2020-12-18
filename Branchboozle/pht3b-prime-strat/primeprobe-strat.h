#ifndef _PRIMEPROBE_STRAT_H_
#define _PRIMEPROBE_STRAT_H_

#include <stdint.h>

/* BPU. */
#define HS_S 100

uint32_t pht3b_branch(uint32_t);

extern void (*prime_strat_array[4])(uint32_t);

#endif /* _PRIMEPROBE_STRAT_H_ */
