#ifndef __UTIL_DEBUG_H__
#define __UTIL_DEBUG_H__

/**
 * \brief The various log levels
 */
typedef enum {
    OB_LOG_NOTSET = -1,
    OB_LOG_NONE = 0,
    OB_LOG_EMERGENCY,
    OB_LOG_ALERT,
    OB_LOG_CRITICAL,
    OB_LOG_ERROR,
    OB_LOG_WARNING,
    OB_LOG_NOTICE,
    OB_LOG_INFO,
    OB_LOG_DEBUG,
    OB_LOG_LEVEL_MAX,
} OBLogLevel;

/* The different log format specifiers supported by the API */
#define OB_LOG_FMT_TIME             't' /* Timestamp in standard format */
#define OB_LOG_FMT_PID              'p' /* PID */
#define OB_LOG_FMT_TID              'i' /* Thread ID */
#define OB_LOG_FMT_TM               'm' /* Thread module name */
#define OB_LOG_FMT_LOG_LEVEL        'd' /* Log level */
#define OB_LOG_FMT_FILE_NAME        'f' /* File name */
#define OB_LOG_FMT_LINE             'l' /* Line number */
#define OB_LOG_FMT_FUNCTION         'n' /* Function */

/* The log format prefix for the format specifiers */
#define SC_LOG_FMT_PREFIX           '%'

#define OBLogInfo(err_code, ...) 		printf(__VA_ARGS__); printf("\r\n")
#define OBLogNotice(err_code, ...) 		printf(__VA_ARGS__); printf("\r\n")
#define OBLogWarning(err_code, ...) 		printf(__VA_ARGS__); printf("\r\n")
#define OBLogError(err_code, ...) 		printf(__VA_ARGS__); printf("\r\n")
#define OBLogCritical(err_code, ...) 	printf(__VA_ARGS__); printf("\r\n")
#define OBLogAlert(err_code, ...) 		printf(__VA_ARGS__); printf("\r\n")
#define OBLogEmerg(err_code, ...) 		printf(__VA_ARGS__); printf("\r\n")

#define OBLogDebug(...)       			printf(__VA_ARGS__); printf("\r\n")
#endif
