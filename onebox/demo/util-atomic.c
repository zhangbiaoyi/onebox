#include "onebox-common.h"
#include "util-atomic.h"

int OBAtomicTest(void)
{
    int result = 0;
    int a = 10;
    int b = 20;
    int *temp_int = NULL;

    OB_ATOMIC_DECL_AND_INIT(void *, temp);

    temp_int = OB_ATOMIC_GET(temp);
    if (temp_int != NULL)
        goto end;

    (void)OB_ATOMIC_SET(temp, &a);
    temp_int = OB_ATOMIC_GET(temp);
    if (temp_int == NULL)
        goto end;
    if (*temp_int != a)
        goto end;

    (void)OB_ATOMIC_SET(temp, &b);
    temp_int = OB_ATOMIC_GET(temp);
    if (temp_int == NULL)
        goto end;
    if (*temp_int != b)
        goto end;

    result = 1;

 end:
    return result;
}

