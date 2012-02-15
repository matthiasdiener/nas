#include <stdio.h>
#include <stdint.h>

static void initialize_dynmap()
{
}

void remap_time_step(int step)
{
	static int init = 0;
	
	if (!init) {
		initialize_dynmap();
		init = 1;
	}
	
	//printf("xxx %d", step);

	#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE
		mapping_lib_remap(0, step);
	#endif
}

// fortran interface
void remap_time_step_(int *step)
{
	remap_time_step(*step);	
}

/*void *__wrap_GOMP_parallel_start(void *func, void *data, unsigned nt)*/
/*{*/
/*	static uint64_t n = 0;*/
/*	printf("ooooooooooooo %llu\n", n++);*/
/*	return __real_GOMP_parallel_start(func, data, nt);*/
/*}*/


/*extern gomp_resolve_num_threads (int);*/


/*void GOMP_parallel_start( void (*fn) (void *), void *data, unsigned num_threads) {*/

/*	printf("libgomp was here\n"); */
/*	int * ptr =0;*/
/*	*ptr = 42;*/
/*	//num_threads = gomp_resolve_num_threads (num_threads);*/
/*    //gomp_team_start (fn, data, num_threads, 0);*/
/*    //load_libraray(libgomp.so)*/
/*}*/

