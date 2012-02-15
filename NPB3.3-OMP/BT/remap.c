#include <stdio.h>
#include <stdint.h>


void remap_(int* step) {
	
	printf("xxx %d", *step);
	//setaffinity()
	
}

void *__wrap_GOMP_parallel_start(void *func, void *data, unsigned nt)
{
	static uint64_t n = 0;
	printf("ooooooooooooo %llu\n", n++);
	return __real_GOMP_parallel_start(func, data, nt);
}


static void initialize_dynmap_() {

	//int []

}


/*extern gomp_resolve_num_threads (int);*/


/*void GOMP_parallel_start( void (*fn) (void *), void *data, unsigned num_threads) {*/

/*	printf("libgomp was here\n"); */
/*	int * ptr =0;*/
/*	*ptr = 42;*/
/*	//num_threads = gomp_resolve_num_threads (num_threads);*/
/*    //gomp_team_start (fn, data, num_threads, 0);*/
/*    //load_libraray(libgomp.so)*/
/*}*/

