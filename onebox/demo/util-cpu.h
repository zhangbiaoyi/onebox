#ifndef __UTIL_CPU_H__
#define __UTIL_CPU_H__

/* Processors configured: */
uint16_t UtilCpuGetNumProcessorsConfigured();

/* Processors online: */
uint16_t UtilCpuGetNumProcessorsOnline();

void UtilCpuPrintSummary();

uint64_t UtilCpuGetTicks(void);

#endif
