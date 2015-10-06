#include "onebox-common.h"
#include "util-error.h"

#define CASE_CODE(E)  case E: return #E

/**
 * \brief Maps the error code, to its string equivalent
 *
 * \param The error code
 *
 * \retval The string equivalent for the error code
 */
const char * OBErrorToString(OBError err)
{
    switch (err) {
        CASE_CODE (OB_OK);
        CASE_CODE (OB_ERR_DAEMON);
        CASE_CODE (OB_ERR_MEM_ALLOC);
        CASE_CODE (OB_ERR_FOPEN);
        CASE_CODE (OB_ERR_POOL_INIT);
        CASE_CODE (OB_ERR_CONF_LOAD);
        CASE_CODE (OB_ERR_CONF_YAML_ERROR);
        CASE_CODE (OB_ERR_CONF_NAME_TOO_LONG);
        CASE_CODE (OB_ERR_FATAL);
    }

    return "UNKNOWN_ERROR";
}
