#ifndef __ONEBOX_H__
#define __ONEBOX_H__

/**************************Description***********************/
/*******
	Descp:  It is a global defination (some path/config ...) for the project
	Author: Zhangbiaoyi
	Ver: 	  0.1
	Time:   2015/09/22
 *******/

/**************************System Header***********************/
#include "onebox-common.h"

/**************************Third Party Header***********************/

/**************************Project Header***********************/

/************************* struct define********************/
typedef struct OBInstance_ {
    char pcap_dev[128];
    char *pid_filename;

    int daemon;
    int unittest;

    struct timeval start_time;

    char *log_dir;
} OBInstance;

/**************************Macro Definition***********************/
/* the name of our binary */
#define PROG_NAME  "onebox"
#define PROG_VER   "0.1.0"

#define DEFAULT_CONF_FILE	"/xyfx/etc/onebox.yaml"
#define DEFAULT_PID_FILE		"/xyfx/var/onebox.pid"

/* runtime engine control flags */
#define ONEBOX_STOP    (1 << 0)   /**< gracefully stop the engine: process all
                                     outstanding packets first */
#define ONEBOX_KILL    (1 << 1)   /**< shut down asap, discarding outstanding
                                     packets. */
#define ONEBOX_DONE    (1 << 2)   /**< packets capture ended */

/* Engine stage/status*/
enum {
    ONEBOX_INIT = 0,
    ONEBOX_RUNTIME,
    ONEBOX_DEINIT
};

/**************************Global Vars***********************/
extern char *conf_filename;

/**************************Function Declaration***********************/

#endif
