#define _GNU_SOURCE

#include <time.h>
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/wait.h>

#include "x86-64.h"

/*------------------------------ Experiment ----------------------------------*/
#define EXPERIMENT "pht3b-covertc-rdtsc"
/*----------------------------------------------------------------------------*/

/* Both processes execute on the same physical core. */
#define SENDER_CPU 0

/* BPU. */
#define HS_S 100
#define PRIME_T 0
#define PRIME_N 1
#define PROBE_T 0
#define PROBE_N 1

#define SENDER_T 0
#define SENDER_N 1

/* Thereshold to distinguish between branch prediction misses and hits. */
#define PROBE_THR 14

/* Repetitions to calculate accuracy. */
#define REPETITIONS 100000

struct comms_struct{
  uint32_t sender_ready;
  uint32_t sender_branch;
  uint32_t sender_bit;
};

static struct comms_struct *comms_ptr;

uint32_t pht3b_branch_rdtsc(uint32_t bd)
{
  uint32_t tsc_s, tsc_e;
  // History Saturation
  asm volatile(
    ".Lloop_start:\n\t"
    "loop .Lloop_start"
    :: "c" (HS_S) :
  );
  _RDTSC_32(tsc_s);
  _LFENCE;
  // true (1) -> Non-Taken, false (0) -> Taken
  if (bd)
    _1BYTE_NOP;
  _RDTSCP_32(tsc_e);
  return (tsc_e - tsc_s);
}

/*
 * pht3b_prime_entry
 * -----------------
 * Prime the PHT entry by moving it to one of the strong states.
 */
static void pht3b_prime_entry(uint32_t prime_transition)
{
  pht3b_branch_rdtsc(prime_transition);  // 1
  pht3b_branch_rdtsc(prime_transition);  // 2
  pht3b_branch_rdtsc(prime_transition);  // 3
  pht3b_branch_rdtsc(prime_transition);  // 4
  pht3b_branch_rdtsc(prime_transition);  // 5
  pht3b_branch_rdtsc(prime_transition);  // 6
  pht3b_branch_rdtsc(prime_transition);  // 7
}

/*----------------------------------------------------------------------------*/

/*
 * pht3b_probe_entry_rdtsc
 * -----------------------
 * Probe the PHT entry and identify the bit sent by the sender branch by
 * only using the TSC.
 *
 * @return: 0 when the sender branch was Non-Taken (SENDER_N)
 *          1    ''      ''      ''      Taken     (SENDER_T)
 */
#if defined(SANDYBRIDGE) || defined(IVYBRIDGE) || defined(HASWELL)

static uint32_t pht3b_probe_entry_rdtsc(uint32_t probe_transition)
{
  uint32_t m0, m1, m2, h0, h1, h2, a0, a1;
  m0 = pht3b_branch_rdtsc(probe_transition);
  m1 = pht3b_branch_rdtsc(probe_transition);
  m2 = pht3b_branch_rdtsc(probe_transition);
       pht3b_branch_rdtsc(probe_transition);
  h0 = pht3b_branch_rdtsc(probe_transition);
  h1 = pht3b_branch_rdtsc(probe_transition);
  h2 = pht3b_branch_rdtsc(probe_transition);
  a0 = ((m0 + m1 + m2) / 3);
  a1 = ((h0 + h1 + h2) / 3) + PROBE_THR;
  if (a0 < a1)
    return SENDER_N;
  return SENDER_T;
}

#elif defined(ZEN)

static uint32_t pht3b_probe_entry_rdtsc(uint32_t probe_transition)
{
  uint32_t m0, m1, h0, h1, a0, a1;
  m0 = pht3b_branch_rdtsc(probe_transition);
  m1 = pht3b_branch_rdtsc(probe_transition);
       pht3b_branch_rdtsc(probe_transition);
  h0 = pht3b_branch_rdtsc(probe_transition);
  h1 = pht3b_branch_rdtsc(probe_transition);
  a0 = ((m0 + m1) / 2);
  a1 = ((h0 + h1) / 2) + PROBE_THR;
  if (a0 < a1)
    return SENDER_N;
  return SENDER_T;
}

#else /* Skylake, Kabylake */

static uint32_t pht3b_probe_entry_rdtsc(uint32_t probe_transition)
{
  uint32_t m0, m1, m2, h0, h1, h2, a0, a1;
  m0 = pht3b_branch_rdtsc(probe_transition);
  m1 = pht3b_branch_rdtsc(probe_transition);
  m2 = pht3b_branch_rdtsc(probe_transition);
       pht3b_branch_rdtsc(probe_transition);
       pht3b_branch_rdtsc(probe_transition);
  h0 = pht3b_branch_rdtsc(probe_transition);
  h1 = pht3b_branch_rdtsc(probe_transition);
  h2 = pht3b_branch_rdtsc(probe_transition);
  a0 = ((m0 + m1 + m2) / 3);
  a1 = ((h0 + h1 + h2) / 3) + PROBE_THR;
  if (a0 < a1)
    return SENDER_N;
  return SENDER_T;
}

#endif /* pht3b_probe_entry_rdtsc */

/*----------------------------------------------------------------------------*/

void sender_process()
{
  cpu_set_t cpuset;
  // Set sender's affinity
  CPU_ZERO(&cpuset);
  CPU_SET(SENDER_CPU, &cpuset);
  sched_setaffinity(0, sizeof(cpuset), &cpuset);
  // Sender is ready
  comms_ptr->sender_ready = 1;
  // Wait for requests
  while (comms_ptr->sender_ready) {
    if (comms_ptr->sender_branch) {
      pht3b_prime_entry(comms_ptr->sender_bit);
      comms_ptr->sender_branch = 0;
    }
    sched_yield();
  }
}

void receiver_process()
{
  uint32_t random_bit;
  uint32_t recover_bit;
  uint32_t score[3] = {};
  // Wait for sender to start
  while (comms_ptr->sender_ready == 0)
    sched_yield();
  // Send and receive only 0's (Sender branch is always Taken)
  comms_ptr->sender_bit = SENDER_T;
  for (int r = 0; r < REPETITIONS; r++) {
    // Prime
    pht3b_prime_entry(PRIME_T);
    // Sender and wait
    comms_ptr->sender_branch = 1;
    sched_yield();
    // Probe
    recover_bit = pht3b_probe_entry_rdtsc(PROBE_N);
    if (recover_bit == 0)
      score[0]++;
  }
  // Send and receive only 1's (Sender branch is always Non-Taken)
  comms_ptr->sender_bit = SENDER_N;
  for (int r = 0; r < REPETITIONS; r++) {
    // Prime
    pht3b_prime_entry(PRIME_T);
    // Sender and wait
    comms_ptr->sender_branch = 1;
    sched_yield();
    // Probe
    recover_bit = pht3b_probe_entry_rdtsc(PROBE_N);
    if (recover_bit == 1)
      score[1]++;
  }
  // Send and receive random bits
  srand(time(0));
  for (int r = 0; r < REPETITIONS; r++) {
    // Set random bit
    random_bit = rand() & 0x1;
    // Prime
    pht3b_prime_entry(PRIME_T);
    // Sender and wait
    comms_ptr->sender_bit = random_bit;
    comms_ptr->sender_branch = 1;
    sched_yield();
    // Probe
    recover_bit = pht3b_probe_entry_rdtsc(PROBE_N);
    if (recover_bit == random_bit)
      score[2]++;
  }
  // Terminate sender
  comms_ptr->sender_ready = 0;
  wait(NULL);
  // Print
  printf("Test          | Accuracy\n");
  printf("------------------------\n");
  printf("All Taken     | %1.6f\n", (float) score[0] / REPETITIONS);
  printf("All Non-Taken | %1.6f\n", (float) score[1] / REPETITIONS);
  printf("Random        | %1.6f\n", (float) score[2] / REPETITIONS);
  printf("------------------------\n");
}

int main()
{
  // Map shared memory for communication
  comms_ptr = (struct comms_struct *) mmap(NULL, sizeof(struct comms_struct),
               PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (fork() == 0) {
    sender_process();
  }
  else {
    receiver_process();
    munmap(comms_ptr, sizeof(struct comms_struct));
    printf("(%d Repetitions)\n", REPETITIONS);
  }
}
