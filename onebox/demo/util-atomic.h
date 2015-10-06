#ifndef __UTIL_ATOMIC_H__
#define __UTIL_ATOMIC_H__

/** from http://en.wikipedia.org/wiki/Memory_ordering
 *
 *  C Compiler memory barrier
 */
#define cc_barrier() __asm__ __volatile__("": : :"memory")

#define ob_memory_barrier()    __asm__ volatile ("" ::: "memory")

/* old "as" does not support "pause" opcode */
#define ob_cpu_pause()         __asm__ (".byte 0xf3, 0x90")

/** from http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
 *
 * Hardware memory barrier
 */
#define hw_barrier() __sync_synchronize()

//#if (!defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8) || !defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) || 
//     !defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2) || !defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1)) )

/**
 *  \brief wrapper for OS/compiler specific atomic compare and swap (CAS)
 *         function.
 *
 *  \param addr Address of the variable to CAS
 *  \param tv Test value to compare the value at address against
 *  \param nv New value to set the variable at addr to
 *
 *  \retval 0 CAS failed
 *  \retval 1 CAS succeeded
 */
#define OBAtomicCompareAndSwap(addr, tv, nv) \
    __sync_bool_compare_and_swap((addr), (tv), (nv))

/**
 *  \brief wrapper for OS/compiler specific atomic fetch and add
 *         function.
 *
 *  \param addr Address of the variable to add to
 *  \param value Value to add to the variable at addr
 */
#define OBAtomicFetchAndAdd(addr, value) \
    __sync_fetch_and_add((addr), (value))

/**
 *  \brief wrapper for OS/compiler specific atomic fetch and sub
 *         function.
 *
 *  \param addr Address of the variable to add to
 *  \param value Value to sub from the variable at addr
 */
#define OBAtomicFetchAndSub(addr, value) \
    __sync_fetch_and_sub((addr), (value))

/**
 *  \brief wrapper for OS/compiler specific atomic fetch and add
 *         function.
 *
 *  \param addr Address of the variable to add to
 *  \param value Value to add to the variable at addr
 */
#define OBAtomicAddAndFetch(addr, value) \
    __sync_add_and_fetch((addr), (value))

/**
 *  \brief wrapper for OS/compiler specific atomic fetch and sub
 *         function.
 *
 *  \param addr Address of the variable to add to
 *  \param value Value to sub from the variable at addr
 */
#define OBAtomicSubAndFetch(addr, value) \
    __sync_sub_and_fetch((addr), (value))

/**
 *  \brief wrapper for OS/compiler specific atomic fetch and "AND"
 *         function.
 *
 *  \param addr Address of the variable to AND to
 *  \param value Value to add to the variable at addr
 */
#define OBAtomicFetchAndAnd(addr, value) \
    __sync_fetch_and_and((addr), (value))

/**
 *  \brief wrapper for OS/compiler specific atomic fetch and "NAND"
 *         function.
 *
 *  \param addr Address of the variable to NAND to
 *  \param value Value to add to the variable at addr
 */
#define OBAtomicFetchAndNand(addr, value) \
    __sync_fetch_and_nand((addr), (value))

/**
 *  \brief wrapper for OS/compiler specific atomic fetch and "XOR"
 *         function.
 *
 *  \param addr Address of the variable to XOR to
 *  \param value Value to add to the variable at addr
 */
#define OBAtomicFetchAndXor(addr, value) \
    __sync_fetch_and_xor((addr), (value))

/**
 *  \brief wrapper for OS/compiler specific atomic fetch and or
 *         function.
 *
 *  \param addr Address of the variable to or to
 *  \param value Value to add to the variable at addr
 */
#define OBAtomicFetchAndOr(addr, value) \
    __sync_fetch_and_or((addr), (value))

/**
 *  \brief wrapper for declaring atomic variables.
 *
 *  \warning Only char, short, int, long, long long and their unsigned
 *           versions are supported.
 *
 *  \param type Type of the variable (char, short, int, long, long long)
 *  \param name Name of the variable.
 *
 *  We just declare the variable here as we rely on atomic operations
 *  to modify it, so no need for locks.
 *
 *  \warning variable is not initialized
 */
#define OB_ATOMIC_DECLARE(type, name) \
    type name ## _ob_atomic__

/**
 *  \brief wrapper for referencing an atomic variable declared on another file.
 *
 *  \warning Only char, short, int, long, long long and their unsigned
 *           versions are supported.
 *
 *  \param type Type of the variable (char, short, int, long, long long)
 *  \param name Name of the variable.
 *
 *  We just declare the variable here as we rely on atomic operations
 *  to modify it, so no need for locks.
 *
 */
#define OB_ATOMIC_EXTERN(type, name) \
    extern type name ## _ob_atomic__

/**
 *  \brief wrapper for declaring an atomic variable and initializing it.
 **/
#define OB_ATOMIC_DECL_AND_INIT(type, name) \
    type name ## _ob_atomic__ = 0

/**
 *  \brief wrapper for initializing an atomic variable.
 **/
#define OB_ATOMIC_INIT(name) \
    (name ## _ob_atomic__) = 0

/**
 *  \brief wrapper for reinitializing an atomic variable.
 **/
#define OB_ATOMIC_RESET(name) \
    (name ## _ob_atomic__) = 0

/**
 *  \brief No-op.
 */
#define OB_ATOMIC_DESTROY(name)

/**
 *  \brief add a value to our atomic variable
 *
 *  \param name the atomic variable
 *  \param val the value to add to the variable
 */
#define OB_ATOMIC_ADD(name, val) \
    OBAtomicAddAndFetch(&(name ## _ob_atomic__), (val))

/**
 *  \brief sub a value from our atomic variable
 *
 *  \param name the atomic variable
 *  \param val the value to sub from the variable
 */
#define OB_ATOMIC_SUB(name, val) \
    OBAtomicSubAndFetch(&(name ## _ob_atomic__), (val))

/**
 *  \brief Bitwise OR a value to our atomic variable
 *
 *  \param name the atomic variable
 *  \param val the value to OR to the variable
 */
#define OB_ATOMIC_OR(name, val) \
    OBAtomicFetchAndOr(&(name ## _ob_atomic__), (val))

/**
 *  \brief Bitwise AND a value to our atomic variable
 *
 *  \param name the atomic variable
 *  \param val the value to AND to the variable
 */
#define OB_ATOMIC_AND(name, val) \
    OBAtomicFetchAndAnd(&(name ## _ob_atomic__), (val))

/**
 *  \brief Bitwise NAND a value to our atomic variable
 *
 *  \param name the atomic variable
 *  \param val the value to NAND to the variable
 */
#define OB_ATOMIC_NAND(name, val) \
    OBAtomicFetchAndNand(&(name ## _ob_atomic__), (val))

/**
 *  \brief Bitwise XOR a value to our atomic variable
 *
 *  \param name the atomic variable
 *  \param val the value to XOR to the variable
 */
#define OB_ATOMIC_XOR(name, val) \
    OBAtomicFetchAndXor(&(name ## _ob_atomic__), (val))

/**
 *  \brief atomic Compare and Switch
 *
 *  \warning "name" is passed to us as "&var"
 */
#define OB_ATOMIC_CAS(name, cmpval, newval) \
    OBAtomicCompareAndSwap((name ## _ob_atomic__), cmpval, newval)

/**
 *  \brief Get the value from the atomic variable.
 *
 *  \retval var value
 */
#define OB_ATOMIC_GET(name) \
    (name ## _ob_atomic__)

/**
 *  \brief Set the value for the atomic variable.
 *
 *  \retval var value
 */
#define OB_ATOMIC_SET(name, val) ({       \
    while (OB_ATOMIC_CAS(&name, OB_ATOMIC_GET(name), val) == 0) \
        ;                                                       \
        })


int OBAtomicTest(void);
#endif
