#ifndef __UTIL_TIME_H__
#define __UTIL_TIME_H__

void TimeInit(void);
void TimeDeinit(void);

void TimeSet(struct timeval *);
void TimeGet(struct timeval *);

#endif
