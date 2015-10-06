#include "onebox-common.h"
#include "util-debug.h"

#if 0
int vasprintf(char **strp, const char *fmt, va_list args) 
{
    int size;

    size = vsnprintf(NULL, 0, fmt, args);
    if ((*strp = malloc(size + 1)) == NULL) {
        return -1;
    }

    size = vsnprintf(*strp, size + 1, fmt, args);
    return size;
}

int asprintf(char **strp, const char *fmt, ...) 
{
    va_list args;
    int size;

    va_start(args, fmt);
    size = vasprintf(strp, fmt, args);

    va_end(args);
    return size;
}

#endif
