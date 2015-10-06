#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <inttypes.h>

#define SIZEG 1024*1024*1024 //40G

int main(void)
{
	FILE *fp = NULL;
	time_t prev=0, cur=0;
	uint64_t wrtsize = 0, totalsize=1024*1024*1024;

	char *fname = "aaa.txt";
	char content[4096]= {0};

	fp = fopen(fname, "a+");
	if(fp == NULL) return 0;

	
	prev = time(NULL);

	while(wrtsize < totalsize)
	{
		fwrite(content, 1, 4096, fp);
		wrtsize += 100;
	}
	cur = time(NULL);

	printf(">>>>>use %d second\r\n", cur-prev);
	return 0;
}
