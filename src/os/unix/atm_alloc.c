#include <atm_core.h>
/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
void *
atm_alloc(atm_uint_t size)
{
    void * ptr = NULL;
    /* TODO if use atm_log,
     * a loop call will happen,
     * then core dump*/
#if (ATM_HAVE_JEMALLOC)
    ptr = je_malloc(size);
#else
    ptr = malloc(size);
#endif
    if (ptr != NULL) {
        memset(ptr, ATM_MEM_ZERO, size);
    } else {
        atm_log_rout(ATM_LOG_FATAL,
            "malloc(%u) failed",size);
        atm_log_fflush();
        abort();
    }
    return ptr;
}


void *
atm_calloc(atm_uint_t nmemb, atm_uint_t size)
{
    void * ptr = NULL;
    /* TODO if use atm_log,
     * a loop call will happen,
     * then core dump*/
#if (ATM_HAVE_JEMALLOC)
    ptr = je_calloc(nmemb, size);
#else
    ptr = calloc(nmemb, size);
#endif
    if (ptr == NULL) {
        atm_log_rout(ATM_LOG_FATAL,
            "calloc(%u,%u) failed",nmemb,size);
        atm_log_fflush();
        abort();
    }
    return ptr;
}


void *
atm_realloc(void *p, atm_uint_t osz, atm_uint_t nsz)
{
    void * ptr = NULL;
    atm_uint_t len = atm_min(osz, nsz);

    ptr = atm_alloc(nsz);

    if (ptr != NULL) {
        if (len > 0) {
            memcpy(ptr, p, len);
        }
        atm_free(p);
    } else {
        atm_log_rout(ATM_LOG_FATAL,
            "atm_alloc(%p,%u) failed",p,nsz);
        atm_log_fflush();
        abort();
    }
    return ptr;
}


void
atm_free(void *ptr)
{
    if (ptr != NULL) {
#if (ATM_HAVE_JEMALLOC)
        /* TODO if use atm_log,
         * a loop call will happen,
         * then core dump*/
        je_free(ptr);
#else
        free(ptr);
#endif
    }
}


/* Returns the size of physical memory (RAM) in bytes.
 * It looks ugly, but this is the cleanest way to achive
 * cross platform results.
 * Cleaned up from:
 *
 * http://nadeausoftware.com/articles/2012/09/c_c_tip_how_get_physical_memory_size_system
 *
 * Note that this function:
 * 1) Was released under the following CC attribution license:
 *    http://creativecommons.org/licenses/by/3.0/deed.en_US.
 * 2) Was originally implemented by David Robert Nadeau.
 * 3) Was modified for Redis by Matt Stancliff.
 * 4) This note exists in order to comply with the original license.
 */
atm_uint_t
atm_phy_mems()
{
#if defined(__unix__) || defined(__unix) || defined(unix) || \
    (defined(__APPLE__) && defined(__MACH__))
#if defined(CTL_HW) && (defined(HW_MEMSIZE) || defined(HW_PHYSMEM64))
    int mib[2];
    mib[0] = CTL_HW;
#if defined(HW_MEMSIZE)
    mib[1] = HW_MEMSIZE;            /* OSX. --------------------- */
#elif defined(HW_PHYSMEM64)
    mib[1] = HW_PHYSMEM64;          /* NetBSD, OpenBSD. --------- */
#endif
    int64_t size = 0;               /* 64-bit */
    size_t len = sizeof(size);
    if (sysctl( mib, 2, &size, &len, NULL, 0) == 0)
        return (size_t)size;
    return 0L;          /* Failed? */

#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
    /* FreeBSD, Linux, OpenBSD, and Solaris. -------------------- */
    return (size_t)sysconf(_SC_PHYS_PAGES) * (size_t)sysconf(_SC_PAGESIZE);

#elif defined(CTL_HW) && (defined(HW_PHYSMEM) || defined(HW_REALMEM))
    /* DragonFly BSD, FreeBSD, NetBSD, OpenBSD, and OSX. -------- */
    int mib[2];
    mib[0] = CTL_HW;
#if defined(HW_REALMEM)
    mib[1] = HW_REALMEM;        /* FreeBSD. ----------------- */
#elif defined(HW_PYSMEM)
    mib[1] = HW_PHYSMEM;        /* Others. ------------------ */
#endif
    unsigned int size = 0;      /* 32-bit */
    size_t len = sizeof(size);
    if (sysctl(mib, 2, &size, &len, NULL, 0) == 0)
        return (size_t)size;
    return 0L;          /* Failed? */
#else
    return 0L;          /* Unknown method to get the data. */
#endif
#else
    return 0L;          /* Unknown OS. */
#endif
}
