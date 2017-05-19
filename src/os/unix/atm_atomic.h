#ifndef _ATM_ATOMIC_H_INCLUDED_
#define _ATM_ATOMIC_H_INCLUDED_

#include <atm_core.h>

/* we use libatomic on priority */
#if (ATM_HAVE_LIBATOMIC)/* IF TAG A */

#define AO_REQUIRE_CAS
#include <atomic_ops.h>

#define ATM_HAVE_ATOMIC_OPS  1
typedef long                        atm_atomic_int_t;
typedef AO_t                        atm_atomic_uint_t;
typedef volatile atm_atomic_uint_t  atm_atomic_t;

#define atm_atomic_cmp_set(lock, old, set) \
    AO_compare_and_swap(lock, old, set)

#define atm_atomic_fetch_add(value, add) \
    AO_fetch_and_add(value, add)

#define atm_memory_barrier() AO_nop()
#define atm_cpu_pause()


#elif (ATM_HAVE_GCC_ATOMIC)/* IF TAG A */
/* GCC 4.1 builtin atomic operations */
#define ATM_HAVE_ATOMIC_OPS  1
typedef long                        atm_atomic_int_t;
typedef unsigned long               atm_atomic_uint_t;
typedef volatile atm_atomic_uint_t  atm_atomic_t;

#define atm_atomic_cmp_set(lock, old, set) \
    __sync_bool_compare_and_swap(lock, old, set)

#define atm_atomic_fetch_add(value, add) \
    __sync_fetch_and_add(value, add)

#define atm_memory_barrier() \
    __sync_synchronize()

#if ( __i386__ || __i386 || __amd64__ || __amd64 )
#define atm_cpu_pause() __asm__ ("pause")
#else
#define atm_cpu_pause()
#endif

#endif/* IF TAG A */



#if !(ATM_HAVE_ATOMIC_OPS)/* IF TAG B */
#define ATM_HAVE_ATOMIC_OPS  0
typedef int32_t                     atm_atomic_int_t;
typedef uint32_t                    atm_atomic_uint_t;
typedef volatile atm_atomic_uint_t  atm_atomic_t;

static atm_inline atm_atomic_uint_t
atm_atomic_cmp_set(atm_atomic_t *lock, atm_atomic_uint_t old,
    atm_atomic_uint_t set)
{
    if (*lock == old) {
        *lock = set;
        return 1;
    }
    return 0;
}

static atm_inline atm_atomic_int_t
atm_atomic_fetch_add(atm_atomic_t *value, atm_atomic_int_t add)
{
    atm_atomic_int_t  old;

    old = *value;
    *value += add;

    return old;
}

#define atm_memory_barrier()
#define atm_cpu_pause()

#endif/* IF TAG B */


#endif /* _ATM_ATOMIC_H_INCLUDED_ */
