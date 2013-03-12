#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <time.h>

#include "app.h"

#define PASTE(f,b) f ## b ## _
#define EVAL(f,b) PASTE(f,b)
#define BM(i) EVAL(BENCHMARK,i)
#define STR(s) #s
#define XSTR(s) STR(s)
#define CASEDEF(i) case i: printf("Starting %s\n", XSTR(BM(i))); extern void BM(i)(void); BM(i)(); break;
#define CASE CASEDEF(__COUNTER__)


void* call(void *info) {
	long id = (long) info;

	switch(id) {
		CASE CASE CASE CASE CASE CASE CASE CASE CASE CASE CASE CASE CASE CASE CASE CASE 
	}

	return NULL;
}


double timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
	return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
		   ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}


int main(int argc, char *argv[]) {
	pthread_t *threads;
	long i;
	struct timespec start, end;
	int nt;

	if (argc!=2) {
		printf("Usage: %s <#groups>\n", argv[0]);
		exit(1);
	}

	nt = atoi(argv[1]);

	threads = malloc(nt * sizeof(pthread_t));

	clock_gettime(CLOCK_MONOTONIC, &start);

	for (i=0; i<nt; i++)
		pthread_create(&threads[i], NULL, &call, (void*) i);

	for (i=0; i<nt; i++)
		pthread_join(threads[i], NULL);

	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("Finished.\nExecution time = %2.3f seconds.\n", timespecDiff(&end, &start)/1000000000);

	return 0;
}
