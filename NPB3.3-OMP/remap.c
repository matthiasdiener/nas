#include <stdio.h>
#include <stdint.h>

/*
	types:
		0: time step
		1: parallel_start
		2: parallel_end
		3: barrier
*/

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

void *__wrap_GOMP_parallel_start(void *func, void *data, unsigned nt)
{
	static uint64_t n = 0;
	//printf("pstart %llu\n", n);
	#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE
		mapping_lib_remap(1, n);
	#endif
	n++;
	return __real_GOMP_parallel_start(func, data, nt);
}

void *__wrap_GOMP_parallel_end()
{
	static uint64_t n = 0;
	__real_GOMP_parallel_end();
	//printf("pend %llu\n", n);
	#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE
		mapping_lib_remap(2, n);
	#endif
	n++;
}

void *__wrap_GOMP_barrier()
{
	static uint64_t n = 0;
	__real_GOMP_barrier();
	#pragma omp master
	{
		//printf("pbarrier %llu\n", n);
		#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE
			mapping_lib_remap(3, n);
		#endif
		n++;
	}
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

