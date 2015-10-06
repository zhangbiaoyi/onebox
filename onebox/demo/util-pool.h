#ifndef __UTIL_POOL_H__
#define __UTIL_POOL_H__

#define POOL_BUCKET_PREALLOCATED    (1 << 0)

/* pool bucket structure */
typedef struct PoolBucket_ {
    void *data;
    uint8_t flags;
    struct PoolBucket_ *next;
} PoolBucket;

/* pool structure */
typedef struct Pool_ {
    uint32_t max_buckets;
    uint32_t preallocated;
    uint32_t allocated;         /**< counter of data elements, both currently in
                                 *   the pool and outside of it (outstanding) */

    uint32_t alloc_stack_size;

    PoolBucket *alloc_stack;

    PoolBucket *empty_stack;
    uint32_t empty_stack_size;

    int data_buffer_size;
    void *data_buffer;
    PoolBucket *pb_buffer;

    void *(*Alloc)();
    int (*Init)(void *, void *);
    void *InitData;
    void (*Cleanup)(void *);
    void (*Free)(void *);

    uint32_t elt_size;
    uint32_t outstanding;       /**< counter of data items 'in use'. Pretty much
                                 *   the diff between PoolGet and PoolReturn */
    uint32_t max_outstanding;   /**< max value of outstanding we saw */
} Pool;

/* prototypes */
Pool* PoolInit(uint32_t, uint32_t, uint32_t, void *(*Alloc)(), int (*Init)(void *, void *), void *, void (*Cleanup)(void *), void (*Free)(void *));
void PoolFree(Pool *);
void PoolPrint(Pool *);
void PoolPrintSaturation(Pool *p);

void *PoolGet(Pool *);
void PoolReturn(Pool *, void *);
#endif
