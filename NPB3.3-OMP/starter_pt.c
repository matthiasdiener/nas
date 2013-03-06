#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <pthread.h>

char *name;

#define ABC() lu
#define BM(i) ABC()_ ## i
#define str(s) #s
#define xstr(s) str(s)
#define xx(i) case i: printf("%s\n", xstr(BM(i))); break;
#define BLA xx(__COUNTER__)

void* call(void *info) {
	long id = (long) info;

	switch(id) {
		BLA BLA BLA BLA BLA BLA BLA BLA BLA BLA BLA BLA BLA BLA BLA BLA
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t *threads;
	long num, i;

	if (argc!=3) {
		printf("Usage: %s <appname> <#groups>\n", argv[0]);
		return 42;
	}

	name = argv[1];
	num = atoi(argv[2]);
	threads = malloc(num * sizeof(pthread_t));

	for (i=0; i<num; i++)
		pthread_create(&threads[i], NULL, &call, (void*) i);

	for (i=0; i<num; i++)
		pthread_join(threads[i], NULL);

	printf("Finished\n");

	return 0;
}