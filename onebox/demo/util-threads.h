#ifndef __UTIL_THREADS_H__
#define __UTIL_THREADS_H__

#include <sys/syscall.h>
#include <sys/prctl.h>
#define THREAD_NAME_LEN 16

enum {
    PRIO_LOW = 2,
    PRIO_MEDIUM = 0,
    PRIO_HIGH = -2,
};

#include <pthread.h>

/* mutex */
#define OBMutex 				pthread_mutex_t
#define OBMutexAttr 			pthread_mutexattr_t
#define OBMutexInit(mut, mutattr ) 	pthread_mutex_init(mut, mutattr)
#define OBMutexLock(mut) 		pthread_mutex_lock(mut)
#define OBMutexTrylock(mut) 		pthread_mutex_trylock(mut)
#define OBMutexUnlock(mut) 		pthread_mutex_unlock(mut)
#define OBMutexDestroy 			pthread_mutex_destroy
#define OBMUTEX_INITIALIZER 		PTHREAD_MUTEX_INITIALIZER

/* rwlocks */
#define OBRWLock 				pthread_rwlock_t
#define OBRWLockInit(rwl, rwlattr ) 	pthread_rwlock_init(rwl, rwlattr)
#define OBRWLockWRLock(rwl) 		pthread_rwlock_wrlock(rwl)
#define OBRWLockRDLock(rwl) 		pthread_rwlock_rdlock(rwl)
#define OBRWLockTryWRLock(rwl) 	pthread_rwlock_trywrlock(rwl)
#define OBRWLockTryRDLock(rwl) 	pthread_rwlock_tryrdlock(rwl)
#define OBRWLockUnlock(rwl) 		pthread_rwlock_unlock(rwl)
#define OBRWLockDestroy 			pthread_rwlock_destroy

/* conditions */
#define OBCondT 				pthread_cond_t
#define OBCondInit 			pthread_cond_init
#define OBCondSignal 			pthread_cond_signal
#define OBCondDestroy 			pthread_cond_destroy
#define OBCondWait(cond, mut) 		pthread_cond_wait(cond, mut)

/* ctrl mutex */
#define OBCtrlMutex 			pthread_mutex_t
#define OBCtrlMutexAttr 			pthread_mutexattr_t
#define OBCtrlMutexInit(mut,mutattr)	pthread_mutex_init(mut, mutattr)
#define OBCtrlMutexLock(mut) 		pthread_mutex_lock(mut)
#define OBCtrlMutexTrylock(mut) 	pthread_mutex_trylock(mut)
#define OBCtrlMutexUnlock(mut) 	pthread_mutex_unlock(mut)
#define OBCtrlMutexDestroy 		pthread_mutex_destroy

/* ctrl conditions */
#define OBCtrlCondT 			pthread_cond_t
#define OBCtrlCondInit 			pthread_cond_init
#define OBCtrlCondSignal 		pthread_cond_signal
#define OBCtrlCondTimedwait 		pthread_cond_timedwait
#define OBCtrlCondDestroy 		pthread_cond_destroy

/* spinlocks */
#define OBSpinlock                   pthread_spinlock_t
#define OBSpinLock(spin)             pthread_spin_lock(spin)
#define OBSpinTrylock(spin)          pthread_spin_trylock(spin)
#define OBSpinUnlock(spin)           pthread_spin_unlock(spin)
#define OBSpinInit(spin, spin_attr)  pthread_spin_init(spin, spin_attr)
#define OBSpinDestroy(spin)          pthread_spin_destroy(spin)

/** Get the Current Thread Id */
#define OBGetThreadIdLong(...) ({ \
   pid_t tmpthid; \
   tmpthid = syscall(SYS_gettid); \
   u_long tid = (u_long)tmpthid; \
   tid; \
})

/**
 * \brief Set the threads name
 */
#define OBSetThreadName(n) ({ \
    char tname[THREAD_NAME_LEN + 1] = ""; \
    if (strlen(n) > THREAD_NAME_LEN) \
        OBLogDebug("Thread name is too long, truncating it..."); \
    strlcpy(tname, n, THREAD_NAME_LEN); \
    int ret = 0; \
    if ((ret = prctl(PR_SET_NAME, tname, 0, 0, 0)) < 0) \
        OBLogDebug("Error setting thread name \"%s\": %s", tname, strerror(errno)); \
    ret; \
})


#define OBSchedYield() sched_yield(void)

int UtilThreadTest(void);
#endif
