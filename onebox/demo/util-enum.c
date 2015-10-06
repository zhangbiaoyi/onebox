#include "onebox-common.h"
#include "util-enum.h"

int OBMapEnumNameToValue(const char *enum_name, OBEnumCharMap *table)
{
    int result = -1;

    if (enum_name == NULL || table == NULL) {
        printf("Invalid argument(s) passed into SCMapEnumNameToValue\n");
        return -1;
    }

    for (; table->enum_name != NULL; table++) {
        if (strcasecmp(table->enum_name, enum_name) == 0) {
            result = table->enum_value;
            break;
        }
    }

    return result;
}

const char * OBMapEnumValueToName(int enum_value, OBEnumCharMap *table)
{
    if (table == NULL) {
        printf("Invalid argument(s) passed into SCMapEnumValueToName\n");
        return NULL;
    }

    for (; table->enum_name != NULL; table++) {
        if (table->enum_value == enum_value) {
            return table->enum_name;
        }
    }

    printf("A enum by the value %d doesn't exist in this table\n", enum_value);

    return NULL;
}

