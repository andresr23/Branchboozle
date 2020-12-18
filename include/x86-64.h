#ifndef _X86_64_H_
#define _X86_64_H_

/* Memory fencing. */
#define _LFENCE asm volatile("lfence")
#define _SFENCE asm volatile("sfence")
#define _MFENCE asm volatile("mfence")

/* Read Time-Stamp Counter (TSC), only the 32 LSBs. */
#define _RDTSC_32(_l)  asm volatile("rdtsc"  : "=a" (_l) ::        "rdx")
#define _RDTSCP_32(_l) asm volatile("rdtscp" : "=a" (_l) :: "rcx", "rdx")

/* Read Time-Stamp Counter (TSC), all 64 bits. */
#define _RDTSC_64(_h, _l)  asm volatile("rdtsc"  : "=a" (_l), "=d" (_h) ::      )
#define _RDTSCP_64(_h, _l) asm volatile("rdtscp" : "=a" (_l), "=d" (_h) :: "rcx")

/* Read Performance Monitoring Counters (PMC). */
#define _RDPMC_32(_l, _c)     asm volatile("rdpmc" : "=a" (_l)            : "c" (_c) : "rdx")
#define _RDPMC_64(_h, _l, _c) asm volatile("rdpmc" : "=a" (_l), "=d" (_h) : "c" (_c) :      )

/* 'cpuid' serialization. */
#define _CPUID asm volatile("cpuid" :: "a" (0) : "rbx", "rcx", "rdx")

/* Cache line flush. */
#define _CLFLUSH(_a) asm volatile("clflush 0(%%rax)" :: "a" (_a) : "memory")

/* Basic memory access. */
#define _MACCESS(_a) asm volatile("movq 0(%%rax), %%rax" :: "a" (_a) : "memory")

/* 'nop' Instruction Sequences. */
#define _NOP_1 asm volatile("nop")
#define _NOP_2   _NOP_1  ; _NOP_1
#define _NOP_4   _NOP_2  ; _NOP_2
#define _NOP_8   _NOP_4  ; _NOP_4
#define _NOP_16  _NOP_8  ; _NOP_8
#define _NOP_32  _NOP_16 ; _NOP_16
#define _NOP_64  _NOP_32 ; _NOP_32
#define _NOP_128 _NOP_64 ; _NOP_64
#define _NOP_256 _NOP_128; _NOP_128
#define _NOP_512 _NOP_256; _NOP_256

/* 'fnop' Instruction Sequences. */
#define _FNOP_1 asm volatile("fnop")
#define _FNOP_2   _FNOP_1  ; _FNOP_1
#define _FNOP_4   _FNOP_2  ; _FNOP_2
#define _FNOP_8   _FNOP_4  ; _FNOP_4
#define _FNOP_16  _FNOP_8  ; _FNOP_8
#define _FNOP_32  _FNOP_16 ; _FNOP_16
#define _FNOP_64  _FNOP_32 ; _FNOP_32
#define _FNOP_128 _FNOP_64 ; _FNOP_64
#define _FNOP_256 _FNOP_128; _FNOP_128
#define _FNOP_512 _FNOP_256; _FNOP_256

/* Variable size 'nop' instructions. */
#define _1BYTE_NOP asm volatile(".byte 0x90")
#define _2BYTE_NOP asm volatile(".byte 0x66, 0x90")
#define _3BYTE_NOP asm volatile(".byte 0x0f, 0x1f, 0x00")
#define _4BYTE_NOP asm volatile(".byte 0x0f, 0x1f, 0x40, 0x00")
#define _5BYTE_NOP asm volatile(".byte 0x0f, 0x1f, 0x44, 0x00, 0x00")
#define _6BYTE_NOP asm volatile(".byte 0x66, 0x0f, 0x1f, 0x44, 0x00, 0x00")
#define _7BYTE_NOP asm volatile(".byte 0x0f, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00")
#define _8BYTE_NOP asm volatile(".byte 0x0f, 0x1f, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00")
#define _9BYTE_NOP asm volatile(".byte 0x66, 0x0f, 0x1f, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00")

#endif /* _X86_64_H_ */
