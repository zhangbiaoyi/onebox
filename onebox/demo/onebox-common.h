#ifndef __ONEBOX_COMMON_H__
#define __ONEBOX_COMMON_H__

/**************************Description***********************/
/*******
	Descp:  It is a common header for the project
	Author: Zhangbiaoyi
	Ver: 	  0.1
	Time:   2015/09/22
 *******/

/**************************System Header***********************/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <getopt.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <limits.h>
#include <malloc.h>
#include <sched.h>
#include <regex.h>
#include <ctype.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <crypt.h>

/**************************Third Party Header***********************/
#include <pcre.h>

/**************************Project Header***********************/

/**************************Macro Definition var***********************/
//#define _GNU_SOURCE

#define TRUE  1
#define False 0

#ifdef __GNUC__
# define UNUSED(d) d __attribute__ ((unused))
#else
# define UNUSED(d) d
#endif

#define xstr(s) str(s)
#define str(s)  #s

#ifndef likely
#define likely(expr) __builtin_expect(!!(expr), 1)
#endif
#ifndef unlikely
#define unlikely(expr) __builtin_expect(!!(expr), 0)
#endif

/**************************Macro Definition func***********************/
/* this is faster than the table lookup */
#include <ctype.h>
#define u8_tolower(c) tolower((uint8_t)(c))

/**************************Global Vars***********************/

/**************************Function Declaration***********************/
#ifndef HAVE_STRLCAT
size_t strlcat(char *, const char *src, size_t siz);
#endif
#ifndef HAVE_STRLCPY
size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#endif
