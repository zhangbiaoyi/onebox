#include "onebox-common.h"
#include "util-debug.h"
#include "util-error.h"
#include "util-mem.h"

/*********** vars ***********/
size_t global_mem=0;
uint8_t print_mem_flag=0;

/*********** funcs ***********/
void OBMemGetInfo(void)
{

}

void *OBMalloc(size_t size)
{
	void *ptrmem = NULL; 

	ptrmem = malloc(size);
	if (ptrmem == NULL) 
	{
            //OBLogError(OB_ERR_MEM_ALLOC, "OBMalloc failed: %s, while trying to allocate %d bytes", strerror(errno), size); 
	}
	return (void*)ptrmem; 
}

void *OBCalloc(size_t nmemb, size_t size)
{
	void *ptrmem = NULL; 

	ptrmem = calloc(nmemb, size);
	if (ptrmem == NULL) 
	{
            //OBLogError(OB_ERR_MEM_ALLOC, "OBCalloc failed: %s, while trying to allocate %d bytes", strerror(errno), size*nmemb); 
	}
	return (void*)ptrmem;
}

void *OBRealloc(void *ptr, size_t size)
{
	void *ptrmem = NULL;
	ptrmem = realloc(ptr, size);
	if (ptrmem == NULL) 
	{
		//OBLogError(OB_ERR_MEM_ALLOC, "OBRealloc failed: %s, while trying to allocate %d bytes", strerror(errno), size); 
        }
	return (void*)ptrmem;
}

char *OBStrdup(char *astr)
{
	void *ptrmem = NULL; 

	ptrmem = strdup(astr);
	if (ptrmem == NULL) 
	{
            //OBLogError(OB_ERR_MEM_ALLOC, "OBMalloc failed: %s, while trying to allocate %d bytes", strerror(errno), size); 
	}
	return (void*)ptrmem; 
}

void  OBFree(void *ptr)
{
	free((ptr));
}

void *OBMemAlign(size_t alignment, size_t size)
{
    void  *p;

    p = memalign(alignment, size);
    if (p == NULL) {
        //OBLogError(NGX_LOG_EMERG, log, ngx_errno, "memalign(%uz, %uz) failed", alignment, size);
    }

    //OBLogError(NGX_LOG_DEBUG_ALLOC, log, 0, "memalign: %p:%uz @%uz", p, size, alignment);

    return p;
}


