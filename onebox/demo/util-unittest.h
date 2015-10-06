#ifndef __UTIL_UNITTEST_H__
#define __UTIL_UNITTEST_H__

typedef struct UtTest_ {
    char *name;
    int(*TestFn)(void);
    int evalue;

    struct UtTest_ *next;
} UtTest;

#endif
