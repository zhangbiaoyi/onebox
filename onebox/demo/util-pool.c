#include "onebox-common.h"
#include "util-pool.h"
#include "util-debug.h"
#include "util-error.h"
#include "util-mem.h"

static int PoolMemset(void *pitem, void *initdata)
{
	Pool *p = (Pool *) initdata;

	memset(pitem, 0, p->elt_size);
	return 1;
}

/**
 * \brief Check if data is preallocated
 * \retval 0 or -1 if not inside */
static int PoolDataPreAllocated(Pool *p, void *data)
{
	int delta = data - p->data_buffer;
	if ((delta < 0) || (delta > p->data_buffer_size)) {
		return 0;
	}
	return 1;
}

Pool *PoolInit(uint32_t size, uint32_t prealloc_size, uint32_t elt_size,  
		void *(*Alloc)(), int (*Init)(void *, void *), void *InitData,  void (*Cleanup)(void *), void (*Free)(void *))
{
	Pool *p = NULL;

	if (size != 0 && prealloc_size > size) {
		OBLogError(OB_ERR_POOL_INIT, "size error");
		goto error;
	}

	if (size != 0 && elt_size == 0) {
		OBLogError(OB_ERR_POOL_INIT, "size != 0 && elt_size == 0");
		goto error;
	}

	if (elt_size && Free) {
		OBLogError(OB_ERR_POOL_INIT, "elt_size && Free");
		goto error;
	}

	/* setup the filter */
	p = OBMalloc(sizeof(Pool));
	if (unlikely(p == NULL)) {
		OBLogError(OB_ERR_POOL_INIT, "alloc error");
		goto error;
	}

	memset(p,0,sizeof(Pool));

	p->max_buckets = size;
	p->preallocated = prealloc_size;
	p->elt_size = elt_size;
	p->data_buffer_size = prealloc_size * elt_size;
	p->Alloc = Alloc;
	p->Init = Init;
	p->InitData = InitData;
	p->Cleanup = Cleanup;
	p->Free = Free;
	if (p->Init == NULL) {
		p->Init = PoolMemset;
		p->InitData = p;
	}

	/* alloc the buckets and place them in the empty list */
	uint32_t u32 = 0;
	if (size > 0) 
	{
		PoolBucket *pb = OBCalloc(size, sizeof(PoolBucket));
		if (unlikely(pb == NULL)) 
		{
			OBLogError(OB_ERR_POOL_INIT, "alloc error");
			goto error;
		}

		p->pb_buffer = pb;
		memset(pb, 0, size * sizeof(PoolBucket));
		for (u32 = 0; u32 < size; u32++) 
		{
			/* populate pool */
			pb->next = p->empty_stack;
			pb->flags |= POOL_BUCKET_PREALLOCATED;
			p->empty_stack = pb;
			p->empty_stack_size++;
			pb++;
		}
	}
	
	if (size > 0) 
	{
		p->data_buffer = OBCalloc(prealloc_size, elt_size);
		/* FIXME better goto */
		if (p->data_buffer == NULL) 
		{
			OBLogError(OB_ERR_POOL_INIT, "alloc error");
			goto error;
		}
	}

	/* prealloc the buckets and requeue them to the alloc list */
	for (u32 = 0; u32 < prealloc_size; u32++) 
	{
		if (size == 0) 
		{ /* unlimited */
			PoolBucket *pb = OBMalloc(sizeof(PoolBucket));
			if (unlikely(pb == NULL)) 
			{
				OBLogError(OB_ERR_POOL_INIT, "alloc error");
				goto error;
			}

			memset(pb, 0, sizeof(PoolBucket));
			if (p->Alloc) 
			{
				pb->data = p->Alloc();
			}
			else
			{
				pb->data = OBMalloc(p->elt_size);
			}
			if (pb->data == NULL)
			{
				OBLogError(OB_ERR_POOL_INIT, "alloc error");
				OBFree(pb);
				goto error;
			}
			if (p->Init(pb->data, p->InitData) != 1) 
			{
				OBLogError(OB_ERR_POOL_INIT, "init error");
				if (p->Cleanup)
					p->Cleanup(pb->data);
				if (p->Free)
					p->Free(pb->data);
				else
					OBFree(pb->data);
				OBFree(pb);
				goto error;
			}

			p->allocated++;

			pb->next = p->alloc_stack;
			p->alloc_stack = pb;
			p->alloc_stack_size++;
		} 
		else 
		{
			PoolBucket *pb = p->empty_stack;
			if (pb == NULL) 
			{
				OBLogError(OB_ERR_POOL_INIT, "alloc error");
				goto error;
			}

			pb->data = (char *)p->data_buffer + u32 * elt_size;
			if (p->Init(pb->data, p->InitData) != 1) 
			{
				OBLogError(OB_ERR_POOL_INIT, "init error");
				if (p->Cleanup) p->Cleanup(pb->data);
				goto error;
			}

			p->empty_stack = pb->next;
			p->empty_stack_size--;

			p->allocated++;

			pb->next = p->alloc_stack;
			p->alloc_stack = pb;
			p->alloc_stack_size++;
		}
	}

	return p;

error:
	if (p != NULL) {
		PoolFree(p);
	}
	return NULL;
}

void PoolFree(Pool *p) 
{
	if (p == NULL) return;

	while (p->alloc_stack != NULL) 
	{
		PoolBucket *pb = p->alloc_stack;
		p->alloc_stack = pb->next;
		if (p->Cleanup) p->Cleanup(pb->data);

		if (PoolDataPreAllocated(p, pb->data) == 0) 
		{
			if (p->Free) p->Free(pb->data);
			else OBFree(pb->data);
		}

		pb->data = NULL;
		if (! pb->flags & POOL_BUCKET_PREALLOCATED) {
			OBFree(pb);
		}
	}
	
	while (p->empty_stack != NULL) 
	{
		PoolBucket *pb = p->empty_stack;
		p->empty_stack = pb->next;
		if (pb->data!= NULL) 
		{
			if (p->Cleanup) p->Cleanup(pb->data);
			if (PoolDataPreAllocated(p, pb->data) == 0) 
			{
				if (p->Free) p->Free(pb->data);
				else OBFree(pb->data);
			}
			pb->data = NULL;
		}
		if (! pb->flags & POOL_BUCKET_PREALLOCATED) 
		{
			OBFree(pb);
		}
	}

	if (p->pb_buffer) OBFree(p->pb_buffer);
	if (p->data_buffer) OBFree(p->data_buffer);
	OBFree(p);
}


void PoolPrint(Pool *p) 
{
	printf("\n----------- Hash Table Stats ------------\n");
	printf("Buckets:               %u\n", p->empty_stack_size + p->alloc_stack_size);
	printf("-----------------------------------------\n");
}

void *PoolGet(Pool *p) 
{
	PoolBucket *pb = p->alloc_stack;

	if (pb != NULL) 
	{
		/* pull from the alloc list */
		p->alloc_stack = pb->next;
		p->alloc_stack_size--;

		/* put in the empty list */
		pb->next = p->empty_stack;
		p->empty_stack = pb;
		p->empty_stack_size++;
	} 
	else 
	{
		if (p->max_buckets == 0 || p->allocated < p->max_buckets) 
		{
			void *pitem;
			OBLogDebug("max_buckets %"PRIu32"", p->max_buckets);

			if (p->Alloc != NULL) {
				pitem = p->Alloc();
			} else {
				pitem = OBMalloc(p->elt_size);
			}

			if (pitem != NULL) 
			{
				if (p->Init(pitem, p->InitData) != 1) 
				{
					if (p->Cleanup) p->Cleanup(pitem);
					if (p->Free != NULL) p->Free(pitem);
					else OBFree(pitem);
					return NULL;
				}

				p->allocated++;

				p->outstanding++;
				if (p->outstanding > p->max_outstanding)
					p->max_outstanding = p->outstanding;
			}

			return pitem;
		} 
		else 
		{
			return NULL;
		}
	}

	void *ptr = pb->data;
	pb->data = NULL;
	p->outstanding++;
	if (p->outstanding > p->max_outstanding)
		p->max_outstanding = p->outstanding;

	return ptr;
}


void PoolReturn(Pool *p, void *data) 
{
	PoolBucket *pb = p->empty_stack;
	OBLogDebug("pb %p", pb);

	if (pb == NULL) 
	{
		p->allocated--;
		p->outstanding--;
		if (p->Cleanup != NULL) {
			p->Cleanup(data);
		}

		if (PoolDataPreAllocated(p, data) == 0) 
		{
			if (p->Free) p->Free(data);
			else OBFree(data);
		}

		OBLogDebug("tried to return data %p to the pool %p, but no more "
                   "buckets available. Just freeing the data.", data, p);
		return;
	}

	/* pull from the alloc list */
	p->empty_stack = pb->next;
	p->empty_stack_size--;

	/* put in the alloc list */
	pb->next = p->alloc_stack;
	p->alloc_stack = pb;
	p->alloc_stack_size++;

	pb->data = data;
	p->outstanding--;
}

void PoolPrintSaturation(Pool *p) 
{
	OBLogDebug("pool %p is using %u out of %u items (%02.1f%%), max %u (%02.1f%%): pool struct memory %lu.", 
		p, p->outstanding, p->max_buckets, (float)(p->outstanding/(float)(p->max_buckets))*100, p->max_outstanding, 
		(float)(p->max_outstanding/(float)(p->max_buckets))*100, (uint64_t)(p->max_buckets * sizeof(PoolBucket)));
}

