#include "onebox-common.h"
#include "util-cpu.h"

/************vars**********/
uint32_t ob_cacheline_size = 0;

/************funcs**********/
static void OBCpuid(uint32_t i, uint32_t *buf)
{
    uint32_t  eax, ebx, ecx, edx;

    __asm__ (

        "cpuid"

    : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) : "a" (i) );

    buf[0] = eax;
    buf[1] = ebx;
    buf[2] = edx;
    buf[3] = ecx;
}

/* auto detect the L2 cache line size of modern and widespread CPUs */

void OBCpuinfo(void)
{
    u_char    *vendor;
    uint32_t   vbuf[6], cpu[4], model;

    vbuf[0] = 0;
    vbuf[1] = 0;
    vbuf[2] = 0;
    vbuf[3] = 0;
    vbuf[4] = 0;
    vbuf[5] = 0;

    OBCpuid(0, vbuf);
    vendor = (u_char *) &vbuf[1];

    if (vbuf[0] == 0) {
        return;
    }

    printf("vendor 0 %s\r\n", vendor);
    OBCpuid(1, cpu);
    printf("vendor 1 %s\r\n", (u_char *)vbuf);

    if (strcmp((const char *)vendor, "GenuineIntel") == 0) {

        switch ((cpu[0] & 0xf00) >> 8) {

        /* Pentium */
        case 5:
            ob_cacheline_size = 32;
            break;

        /* Pentium Pro, II, III */
        case 6:
            ob_cacheline_size = 32;

            model = ((cpu[0] & 0xf0000) >> 8) | (cpu[0] & 0xf0);

            if (model >= 0xd0) {
                /* Intel Core, Core 2, Atom */
                ob_cacheline_size = 64;
            }

            break;

        /*
         * Pentium 4, although its cache line size is 64 bytes,
         * it prefetches up to two cache lines during memory read
         */
        case 15:
            ob_cacheline_size = 128;
            break;
        }

    } else if (strcmp((const char *)vendor, "AuthenticAMD") == 0) {
        ob_cacheline_size = 64;
    }
}


/**
 * \brief Get the number of cpus configured in the system
 * \retval 0 if the syscall is not available or we have an error;
 *           otherwise it will return the number of cpus configured
 */
uint16_t UtilCpuGetNumProcessorsConfigured()
{
    long nprocs = -1;
    nprocs = sysconf(_SC_NPROCESSORS_CONF);
    if (nprocs < 1) {
        //SCLogError(SC_ERR_SYSCALL, "Couldn't retrieve the number of cpus configured (%s)", strerror(errno));
        return 0;
    }

    if (nprocs > UINT16_MAX) {
        //SCLogDebug("It seems that there are more than %d CPUs configured on this system", UINT16_MAX);
        return UINT16_MAX;
    }

    return (uint16_t)nprocs;
}

uint16_t UtilCpuGetNumProcessorsOnline()
{
    long nprocs = -1;
    nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    if (nprocs < 1) {
        //SCLogError(SC_ERR_SYSCALL, "Couldn't retrieve the number of cpus online (%s)", strerror(errno));
        return 0;
    }

    if (nprocs > UINT16_MAX) {
        //SCLogDebug("It seems that there are more than %d CPUs online. ", UINT16_MAX);
        return UINT16_MAX;
    }

    return nprocs;
}

uint64_t UtilCpuGetTicks(void)
{
    uint64_t val=0;
#if defined(__GNUC__) && (defined(__x86_64) || defined(_X86_64_) || defined(ia_64) || defined(__i386__))
#if defined(__x86_64) || defined(_X86_64_) || defined(ia_64)
    __asm__ __volatile__ (
    "xorl %%eax,%%eax\n\t"
    "cpuid\n\t"
    ::: "%rax", "%rbx", "%rcx", "%rdx");
#else
    __asm__ __volatile__ (
    "xorl %%eax,%%eax\n\t"
    "pushl %%ebx\n\t"
    "cpuid\n\t"
    "popl %%ebx\n\t"
    ::: "%eax", "%ecx", "%edx");
#endif
    uint32_t a, d;
    __asm__ __volatile__ ("rdtsc" : "=a" (a), "=d" (d));
    val = ((uint64_t)a) | (((uint64_t)d) << 32);
#if defined(__x86_64) || defined(_X86_64_) || defined(ia_64)
    __asm__ __volatile__ (
    "xorl %%eax,%%eax\n\t"
    "cpuid\n\t"
    ::: "%rax", "%rbx", "%rcx", "%rdx");
#else
    __asm__ __volatile__ (
    "xorl %%eax,%%eax\n\t"
    "pushl %%ebx\n\t"
    "cpuid\n\t"
    "popl %%ebx\n\t"
    ::: "%eax", "%ecx", "%edx");
#endif

#else /* #if defined(__GNU__) */
#warning Using inferior version of UtilCpuGetTicks
    struct timeval now;
    gettimeofday(&now, NULL);
    val = (now.tv_sec * 1000000) + now.tv_usec;
#endif
    return val;
}

/**
 * \brief Print a summary of CPUs detected (configured and online)
 */
void UtilCpuPrintSummary()
{
    uint16_t cpus_conf = UtilCpuGetNumProcessorsConfigured();
    uint16_t cpus_online = UtilCpuGetNumProcessorsOnline();

    uint64_t ticks = UtilCpuGetTicks();

    OBCpuinfo();
    //SCLogDebug("CPUs Summary: ");
    if (cpus_conf > 0)
        //SCLogDebug("CPUs configured: %d", cpus_conf);
	printf("CPUs configured: %d\r\n", cpus_conf);

    if (cpus_online > 0)
        //SCLogInfo("CPUs/cores online: %d", cpus_online);
	printf("CPUs/cores online: %d\r\n", cpus_online);

    if (cpus_online == 0 && cpus_conf == 0)
        //SCLogInfo("Couldn't retireve any information of CPU's");
	printf("Couldn't retireve any information of CPU's\r\n");

    printf("cpu ticks is %lu\r\n", ticks);
}
