#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <pthread.h>

#include "app.h"

#define PASTE2(f,b) f ## b
#define EVAL2(f,b) PASTE2(f,b)
#define PASTE3(f,m,b) f ## m ## b
#define EVAL3(f,m,b) PASTE3(f,m,b)
#define BM(i) EVAL3(BENCHMARK,i,_)
#define str(s) #s
#define xstr(s) str(s)
#define xx(i) case i: printf("%s\n", xstr(BM(i))); extern void BM(i)(void); BM(i)(); break;
#define CA xx(__COUNTER__)

#define X0 CA
#define X1 CA X0
#define X2 CA X1
#define X3 CA X2
#define X4 CA X3
#define X5 CA X4
#define X6 CA X5
#define X7 CA X6
#define X8 CA X7

#define FC EVAL2(X,NTHREADS)
#define FUNC_CALLS FC

void* call(void *info) {
	long id = (long) info;

	switch(id) {
		FUNC_CALLS
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t *threads;
	long i;

	threads = malloc(NTHREADS+1 * sizeof(pthread_t));

	for (i=0; i<NTHREADS+1; i++)
		pthread_create(&threads[i], NULL, &call, (void*) i);

	for (i=0; i<NTHREADS+1; i++)
		pthread_join(threads[i], NULL);

	printf("Finished\n");

	return 0;
}