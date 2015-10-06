#ifndef __UTIL_ENUM_H__
#define __UTIL_ENUM_H__

typedef struct OBEnumCharMap_ {
    char *enum_name;
    int enum_value;
} OBEnumCharMap;

int OBMapEnumNameToValue(const char *, OBEnumCharMap *);

const char * OBMapEnumValueToName(int, OBEnumCharMap *);

#endif
