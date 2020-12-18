#ifndef _PMC_MODULE_H_
#define _PMC_MODULE_H_

#if defined(AMD)
/*
 * Performance Event Select.
 * -------------------------
 * Architectural Performance Monitoring facilities provide a number of
 * MSRs to invididually configure PMCs.
 *
 * References:
 * Open Source Register Reference for AMD Family 17h Processors Models 00h-2Fh
 * section 2.1.10.3 MSRs, page 136.
 */
#define PERF_CTL_0 0xC0010200U
#define PERF_CTL_1 0xC0010202U
#define PERF_CTL_2 0xC0010204U
#define PERF_CTL_3 0xC0010206U

/*
 * Performance Monitoring Counters.
 * --------------------------------
 * References:
 * Processor Programming Reference (PPR) for AMD Family 17h Models 00h-0Fh
 * Processors, Section 2.1.15.
 */

/*
 * Retired Branch Instructions Mispredicted.
 * -----------------------------------------
 * 00H : All branches.
 */
#define EX_RET_BRN_MISP 0x0C3UL
// Umasks
#define ALL_BRANCHES 0x00UL

#else /* Intel */
/*
 * Performance Event Select Register MSRs.
 * ---------------------------------------
 * Architectural Performance Monitoring facilities provide a number of
 * MSRs to invididually configure PMCs.
 *
 * References:
 * Intel® 64 and IA-32 architectures software developer's manual combined
 * volumes 3A, 3B, 3C, and 3D: System programming guide,
 * Section 18.2.1.1 Architectural Performance Monitoring Facilities, Page 18-3.
 *
 * Intel® 64 and IA-32 architectures software developer's manual volume 4:
 * Model-specific registers.
 */
#define PERFEVTSEL0 0x186U
#define PERFEVTSEL1 0x187U
#define PERFEVTSEL2 0x188U
#define PERFEVTSEL3 0x189U

/*
 * Pre-defined Architectural Performance Events.
 * ---------------------------------------------
 * References:
 * Intel® 64 and IA-32 architectures software developer's manual combined
 * volumes 3A, 3B, 3C, and 3D: System programming guide, Table 18-1.
 */

/*
 * Architectural Event: Mistpredicted Branch.
 * ------------------------------------------
 * 00H : Mispredicted branch instructions at retirement.
 * 01H : Mispredicted conditional branch instructions retired.
 * 04H : Mispredicted macro branch instructions retired.
 * 20H : Number of near branch instructions retired that were taken.
 */
#define BR_MISP_RETIRED 0xC5UL
// Umasks
#define ALL_BRANCHES   0x00UL
#define CONDITIONAL    0x01UL
#define ALL_BRANCHES_M 0x04UL
#define NEAR_TAKEN     0x20UL

#endif /* AMD or Intel */

#endif /* _PMC_MODULE_H_ */
