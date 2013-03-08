#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <pthread.h>

#include "app.h"

#define PASTE(f,b) f ## b ## _
#define EVAL(f,b) PASTE(f,b)
#define BM(i) EVAL(BENCHMARK,i)
#define STR(s) #s
#define XSTR(s) STR(s)
#define CASEDEF(i) case i: printf("Starting %s\n", XSTR(BM(i))); extern void BM(i)(void); BM(i)(); break;
#define CA CASEDEF(__COUNTER__)


void* call(void *info) {
	long id = (long) info;

	switch(id) {
		CA CA CA CA CA CA CA CA CA CA CA CA CA CA CA CA 
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t *threads;
	long i;

	if (argc<2) {
		printf("Usage: %s <#threads>\n", argv[0]);
		exit(1);
	}

	int nt = atoi(argv[1]);

	threads = malloc(nt * sizeof(pthread_t));

	for (i=0; i<nt; i++)
		pthread_create(&threads[i], NULL, &call, (void*) i);

	for (i=0; i<nt; i++)
		pthread_join(threads[i], NULL);

	printf("Finished\n");

	return 0;
}