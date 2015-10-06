#ifndef __UTIL_MEM_H__
#define __UTIL_MEM_H__

#if 0
       void *malloc(size_t size);
       void free(void *ptr);
       void *calloc(size_t nmemb, size_t size);
       void *realloc(void *ptr, size_t size);
#endif

void *OBMalloc(size_t size);
void *OBCalloc(size_t nmemb, size_t size);
void *OBRealloc(void *ptr, size_t size);
void  OBFree(void *ptr);
char *OBStrdup(char *astr);
#endif
