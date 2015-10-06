#ifndef __UTIL_ERROR_H__
#define __UTIL_ERROR_H__

/* different error types */
typedef enum {
    OB_OK,
    OB_ERR_DAEMON,
    OB_ERR_MEM_ALLOC,
    OB_ERR_CONF_LOAD,
    OB_ERR_FOPEN,
    OB_ERR_POOL_INIT,
    OB_ERR_CONF_YAML_ERROR,
    OB_ERR_CONF_NAME_TOO_LONG,
    OB_ERR_FATAL
}OBError;

const char *OBErrorToString(OBError);

#endif
