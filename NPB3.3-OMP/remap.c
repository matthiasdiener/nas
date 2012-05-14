#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <map_algorithm.h>
#include <libmapping.h>
#include <libremap.h>

#ifdef DEBUG
	#undef DEBUG
#endif

//#define DEBUG

#define ENABLE_REMAP

#ifdef DEBUG
	#define DPRINTF(...) printf(__VA_ARGS__)
#else
	#define DPRINTF(...)
#endif


/*
	types:
		0: time step
		1: parallel_start
		2: parallel_end
		3: barrier
*/

enum {
	TYPE_TIME_STEP = 0,
	TYPE_PARALLEL_START = 1,
	TYPE_PARALLEL_END = 2,
	TYPE_BARRIER = 3
};

#if defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
	static void remap_check_migrate(thread_mapping_t *tm_static, int type)
#elif defined(LIBMAPPING_REAL_REMAP_SIMICS)
	static void remap_check_migrate(int type)
#endif
#if defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE) || defined(LIBMAPPING_REAL_REMAP_SIMICS)
	{
		int code;
		
		if (type != TYPE_BARRIER)
			return;
		
		#if defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
			code = libmapping_remap_check_migrate(tm_static);
		#elif defined(LIBMAPPING_REAL_REMAP_SIMICS)
			code = libmapping_remap_check_migrate();
		#endif
		
		if (code == LIBMAPPING_REMAP_MIGRATED)
			DPRINTF(">>> MIGRATED\n");
	}
#endif

void remap_time_step(int step)
{
	if (libmapping_is_initialized()) {
		//printf("xxx %d", step);

		#if defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_SIMSIDE)
			libmapping_remap(0, step);
		#elif defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
			{
				thread_mapping_t *tm;
			
				tm = get_comm_pattern(TYPE_TIME_STEP, step);
//				assert(tm != NULL);
			
				if (tm != NULL)
					remap_check_migrate(tm, TYPE_TIME_STEP);
			
				//{int i, j; printf("comm matrix(type %i value %i)\n", 0, step); for (i=0; i<tm->nthreads; i++) { for (j=0; j<tm->nthreads; j++) { printf("  %llu", tm->comm_matrix[i][j]); } printf("\n"); } printf("\n"); }
			}
		#elif defined(LIBMAPPING_REAL_REMAP_SIMICS)
			remap_check_migrate(TYPE_TIME_STEP);
		#else
			#error missed define
		#endif
	}
}

// fortran interface
void remap_time_step_(int *step)
{
	remap_time_step(*step);	
}

void *__wrap_GOMP_parallel_start(void *func, void *data, unsigned nt)
{
	static uint32_t n = 0;
	
	if (libmapping_is_initialized()) {
		//printf("pstart %llu\n", n);
		#if defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_SIMSIDE)
			libmapping_remap(1, n);
		#elif defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
			{
				thread_mapping_t *tm;
			
				tm = get_comm_pattern(TYPE_PARALLEL_START, n);
//				assert(tm != NULL);
			
				if (tm != NULL)
					remap_check_migrate(tm, TYPE_PARALLEL_START);
			}
		#elif defined(LIBMAPPING_REAL_REMAP_SIMICS)
			remap_check_migrate(TYPE_PARALLEL_START);
		#else
			#error missed define
		#endif

		n++;
	}
	
	return __real_GOMP_parallel_start(func, data, nt);
}

void *__wrap_GOMP_parallel_end()
{
	static uint32_t n = 0;
	
	__real_GOMP_parallel_end();
	
	if (libmapping_is_initialized()) {
		//printf("pend %llu\n", n);
		#if defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_SIMSIDE)
			libmapping_remap(2, n);
		#elif defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
			{
				thread_mapping_t *tm;
			
				tm = get_comm_pattern(TYPE_PARALLEL_END, n);
	//			assert(tm != NULL);
			
				if (tm != NULL)
					remap_check_migrate(tm, TYPE_PARALLEL_END);
			}
		#elif defined(LIBMAPPING_REAL_REMAP_SIMICS)
			remap_check_migrate(TYPE_PARALLEL_END);
		#else
			#error missed define
		#endif
		
		n++;
	}
}

void *__wrap_GOMP_barrier()
{
	static uint32_t n = 0;
	
	__real_GOMP_barrier();
	#pragma omp master
	{
		if (libmapping_is_initialized()) {
			//printf("pbarrier %llu\n", n);
			#if defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_SIMSIDE)
				libmapping_remap(3, n);
			#elif defined(LIBMAPPING_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
				{
					thread_mapping_t *tm;
			
					tm = get_comm_pattern(TYPE_BARRIER, n);
//					assert(tm != NULL);
			
					if (tm != NULL)
						remap_check_migrate(tm, TYPE_BARRIER);
				}
			#elif defined(LIBMAPPING_REAL_REMAP_SIMICS)
				remap_check_migrate(TYPE_BARRIER);
			#else
				#error missed define
			#endif
		
			n++;
		}
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

