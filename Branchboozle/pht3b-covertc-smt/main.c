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
#include "pmc-user.h"

/* ----------------------------- Exeriment -----------------------------------*/
#define EXPERIMENT "pht3b-covertc-smt"
/* ---------------------------------------------------------------------------*/

#define WAIT_30NS nanosleep((const struct timespec[]){{0, 30L}}, NULL)
#define WAIT_20NS nanosleep((const struct timespec[]){{0, 20L}}, NULL)
#define WAIT_10NS nanosleep((const struct timespec[]){{0, 10L}}, NULL)
#define WAIT_05NS nanosleep((const struct timespec[]){{0, 05L}}, NULL)

/* Define sibling physical threads, Receiver runs in core 0. */
#if defined(ZEN)
 #define SENDER_CPU 1
#else /* Intel */
 #define SENDER_CPU 4
#endif /* SENDER_CPU */

/* BPU. */
#define HS_S 100
#define PRIME_T 0
#define PRIME_N 1
#define PROBE_T 0
#define PROBE_N 1

#define SENDER_T 0
#define SENDER_N 1

/* Repetitions to calculate accuracy. */
#define REPETITIONS 100000

struct comms_struct{
  uint32_t sender_ready;
  uint32_t sender_branch;
  uint32_t sender_bit;
};

struct comms_struct *comms_ptr;

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

/*
 * pht3b_prime_entry
 * -----------------
 * Prime the PHT entry by moving it to one of the strong states.
 * Regardless of the micro-architecture, seven transitions suffice to move it
 * the FSM to the strong states.
 */
void pht3b_prime_entry(uint32_t prime_transition)
{
  pht3b_branch(prime_transition);  // 1
  pht3b_branch(prime_transition);  // 2
  pht3b_branch(prime_transition);  // 3
  pht3b_branch(prime_transition);  // 4
  pht3b_branch(prime_transition);  // 5
  pht3b_branch(prime_transition);  // 6
  pht3b_branch(prime_transition);  // 7
}

/*
 * pht3b_probe_entry
 * -----------------
 * Probe the PHT entry and identify the bit sent by the sender branch.
 *
 * @return: 1 when the sender branch was Non-Taken (SENDER_N)
 *          0    ''      ''      ''      Taken     (SENDER_T)
 */
uint32_t pht3b_probe_entry(uint32_t probe_transition)
{
  register uint32_t m0;
  m0 = pht3b_branch(probe_transition);
  return (m0 >= 1) ? SENDER_T : SENDER_N;
}

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
    WAIT_10NS;
  }
}

void receiver_process()
{
  uint32_t random_bit;
  uint32_t recover_bit;
  uint32_t score[3] = {};
  clock_t time_s, time_e;
  double timing[3];
  // Wait for sender to start
  while (comms_ptr->sender_ready == 0)
    sched_yield();
  // Send and receive only 0's (Sender branch is always Taken)
  comms_ptr->sender_bit = SENDER_T;
  time_s = clock();
  for (int r = 0; r < REPETITIONS; r++) {
    // Prime
    pht3b_prime_entry(PRIME_T);
    // Sender and wait
    comms_ptr->sender_branch = 1;
    WAIT_30NS;
    // Probe
    recover_bit = pht3b_probe_entry(PROBE_N);
    if (recover_bit == 0)
      score[0]++;
  }
  time_e = clock();
  timing[0] = ((double) (time_e - time_s)) / CLOCKS_PER_SEC;
  // Send and receive only 1's (Sender branch is always Non-Taken)
  comms_ptr->sender_bit = SENDER_N;
  time_s = clock();
  for (int r = 0; r < REPETITIONS; r++) {
    // Prime
    pht3b_prime_entry(PRIME_T);
    // Sender and wait
    comms_ptr->sender_branch = 1;
    WAIT_30NS;
    // Probe
    recover_bit = pht3b_probe_entry(PROBE_N);
    if (recover_bit == 1)
      score[1]++;
  }
  time_e = clock();
  timing[1] = ((double) (time_e - time_s)) / CLOCKS_PER_SEC;
  // Send and receive random bits
  srand(time(0));
  time_s = clock();
  for (int r = 0; r < REPETITIONS; r++) {
    // Set random bit
    random_bit = rand() & 0x1;
    // Prime
    pht3b_prime_entry(PRIME_T);
    // Sender and wait
    comms_ptr->sender_bit = random_bit;
    comms_ptr->sender_branch = 1;
    WAIT_30NS;
    // Probe
    recover_bit = pht3b_probe_entry(PROBE_N);
    if (recover_bit == random_bit)
      score[2]++;
  }
  time_e = clock();
  timing[2] = ((double) (time_e - time_s)) / CLOCKS_PER_SEC;
  // Terminate sender
  comms_ptr->sender_ready = 0;
  wait(NULL);
  // Print
  printf("Test          | Accuracy | Total time (s) | bits/s\n");
  printf("------------------------------------------------------\n");
  printf("All Taken     | %1.6f | %1.3lf          | %1.3lf\n", (float) score[0] / REPETITIONS, timing[0], (double) REPETITIONS / timing[0]);
  printf("All Non-Taken | %1.6f | %1.3lf          | %1.3lf\n", (float) score[1] / REPETITIONS, timing[1], (double) REPETITIONS / timing[1]);
  printf("Random        | %1.6f | %1.3lf          | %1.3lf\n", (float) score[2] / REPETITIONS, timing[2], (double) REPETITIONS / timing[2]);
  printf("------------------------------------------------------\n");
}

int main()
{
  // Map shared memory for communications
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
