#ifndef __UTIL_MACRO_H__
#define __UTIL_MACRO_H__

#define myprintf(...) printk("[lch]:File:%s, Line:%d, Function:%s," \  
					__VA_ARGS__, __FILE__, __LINE__ ,__FUNCTION__);  

#ifndef LOG_NDEBUG_FUNCTION  
#define LOGFUNC(...) ((void)0)  
#else  
#define LOGFUNC(...) (printk(__VA_ARGS__))  
#endif

#define XNAME(n) x ## n  
#define PRINT_XN(n) printf("x" #n " = %d/n", x ## n);


#endif
