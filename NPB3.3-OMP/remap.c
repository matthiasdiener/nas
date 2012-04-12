#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <map_algorithm.h>
#include <mapping-lib.h>

#ifdef DEBUG
	#undef DEBUG
#endif

#define DEBUG

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

static uint32_t *current_map;

#ifndef MPLIB_MAPPING_ALGORITHM_STATIC
	static uint32_t *new_map = NULL;
#endif

static uint16_t nthreads;

#if defined(MAPPING_LIB_REAL_REMAP_SIMICS)
	static uint64_t **comm_matrix_cores;
	static uint64_t **comm_matrix_threads;
	
	thread_mapping_t tm_;

	static void get_communication_matrix(uint64_t **comm_matrix)
	{
		int i, j;

		for (i=0; i<ncores; i++) {
			for (j=0; j<ncores; j++) {
				comm_matrix[i][j] = mapping_lib_get_communication(i, j);
			}
		}
	}
#endif

//static thread_mapping_t map;

static void check_init()
{
	static char init = 0;
	int i, j;
	
	if (!init) {
		init = 1;
		assert(wrapper_dynmap_initialized());
		
		nthreads = wrapper_get_nthreads();

		current_map = (uint32_t*)calloc(nthreads, sizeof(uint32_t));
		assert(current_map != NULL);

		#if defined(MAPPING_LIB_REAL_REMAP_SIMICS)
			comm_matrix_cores = comm_matrix_malloc(nthreads);
			comm_matrix_threads = comm_matrix_malloc(nthreads);
			tm_.comm_matrix = comm_matrix_malloc(nthreads);
			
			tm_.nthreads = nthreads;
			
			for (i=0; i<nthreads; i++) {
				for (j=0; j<nthreads; j++) {
					comm_matrix_cores[i][j] = 0;
					comm_matrix_threads[i][j] = 0;
					tm_.comm_matrix[i][j] = 0;
				}
			}		
		#endif
		
		#if !defined(MPLIB_MAPPING_ALGORITHM_STATIC) && defined(ENABLE_REMAP)
			new_map = (uint32_t*)calloc(nthreads, sizeof(uint32_t));
			assert(new_map != NULL);
		#endif
		
		mapping_lib_copy_initial_map(current_map);

		//{ int i; for (i=0; i<nthreads; i++) { current_map[i] = 0; } }
		{ int i; printf("init with map: "); for (i=0; i<nthreads; i++) { printf("%i,", current_map[i]); } printf("\n"); }
	}
}

#if defined(MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
	static void remap_check_migrate(thread_mapping_t *tm, int type)
#elif defined(MAPPING_LIB_REAL_REMAP_SIMICS)
	static void remap_check_migrate(int type)
#endif
	{
		static uint32_t n_migrations = 0;
		uint32_t diff, i, j;
		#if defined(MAPPING_LIB_REAL_REMAP_SIMICS)
			thread_mapping_t *tm = &tm_;
		#endif

//		if (type != 0)
//			return;
		
		#if defined(MAPPING_LIB_REAL_REMAP_SIMICS)
			get_communication_matrix(comm_matrix_cores);
			mapping_lib_clear_communication();

			#ifdef DEBUG			
				printf("\ncore matrix\n");
				for (i=0; i<nthreads; i++) {
					for (j=0; j<nthreads; j++) {
						printf("%llu  ", comm_matrix_cores[i][j]);
					}
					printf("\n");
				}
			#endif
			
			for (i=0; i<nthreads; i++) {
				for (j=0; j<nthreads; j++) {
					comm_matrix_threads[i][j] += comm_matrix_cores[ wrapper_core_ids_pos(current_map[i]) ][ wrapper_core_ids_pos(current_map[j]) ];
				}
			}
			
			printf("\nthread matrix\n");
			for (i=0; i<nthreads; i++) {
				for (j=0; j<nthreads; j++) {
					tm->comm_matrix[i][j] = comm_matrix_threads[i][j];
				}
			}

			#ifdef DEBUG			
				printf("\nfinal matrix\n");
				for (i=0; i<nthreads; i++) {
					for (j=0; j<nthreads; j++) {
						printf("%llu  ", tm->comm_matrix[i][j]);
					}
					printf("\n");
				}
			#endif
		#endif
		
		#if !defined(MPLIB_MAPPING_ALGORITHM_STATIC) && defined(ENABLE_REMAP)
			tm->map = new_map;
			wrapper_generate_thread_mapping(tm);
		#endif
		
		#if defined(ENABLE_REMAP)
			diff = wrapper_generate_difference_between_mappings(current_map, tm->map, tm->nthreads);
		#else
			diff = 0;
		#endif
//		{ int i; printf("new map type %i: ", type); for (i=0; i<nthreads; i++) { printf("%i,", tm->map[i]); } printf("\n diff %i\n\n", diff); } getchar();
		if (diff > 0) {
			n_migrations++;
			printf("migrando %i diff %u\n", n_migrations, diff);

			{ int i; printf("new map type %i: ", type); for (i=0; i<nthreads; i++) { printf("%i,", tm->map[i]); } printf("\n"); }

			for (i=0; i<nthreads; i++) { // migrate
				mapping_lib_set_aff_of_thread(i, tm->map[i]);
			}

			for (i=0; i<nthreads; i++)
				current_map[i] = tm->map[i];
		}
	}
#endif

void remap_time_step(int step)
{
	check_init();
	
	//printf("xxx %d", step);

	#if defined(MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE)
		mapping_lib_remap(0, step);
	#elif defined(MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
		{
			thread_mapping_t *tm;
			
			tm = wrapper_get_comm_pattern(TYPE_TIME_STEP, step);
			assert(tm != NULL);
			
			remap_check_migrate(tm, TYPE_TIME_STEP);
			
			//{int i, j; printf("comm matrix(type %i value %i)\n", 0, step); for (i=0; i<tm->nthreads; i++) { for (j=0; j<tm->nthreads; j++) { printf("  %llu", tm->comm_matrix[i][j]); } printf("\n"); } printf("\n"); }
		}
	#elif defined(MAPPING_LIB_REAL_REMAP_SIMICS)
		remap_check_migrate(TYPE_TIME_STEP);
	#endif
}

// fortran interface
void remap_time_step_(int *step)
{
	remap_time_step(*step);	
}

void *__wrap_GOMP_parallel_start(void *func, void *data, unsigned nt)
{
	static uint32_t n = 0;
	
	check_init();
	
	//printf("pstart %llu\n", n);
	#if defined(MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE)
		mapping_lib_remap(1, n);
	#elif defined(MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
		{
			thread_mapping_t *tm;
			
			tm = wrapper_get_comm_pattern(TYPE_PARALLEL_START, n);
			assert(tm != NULL);
			
			remap_check_migrate(tm, TYPE_PARALLEL_START);
		}
	#elif defined(MAPPING_LIB_REAL_REMAP_SIMICS)
		remap_check_migrate(TYPE_PARALLEL_START);
	#endif

	n++;
	return __real_GOMP_parallel_start(func, data, nt);
}

void *__wrap_GOMP_parallel_end()
{
	static uint32_t n = 0;
	
	check_init();
	
	__real_GOMP_parallel_end();
	//printf("pend %llu\n", n);
	#if defined(MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE)
		mapping_lib_remap(2, n);
	#elif defined(MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
		{
			thread_mapping_t *tm;
			
			tm = wrapper_get_comm_pattern(TYPE_PARALLEL_END, n);
			assert(tm != NULL);
			
			remap_check_migrate(tm, TYPE_PARALLEL_END);
		}
	#elif defined(MAPPING_LIB_REAL_REMAP_SIMICS)
		remap_check_migrate(TYPE_PARALLEL_END);
	#endif
		
	n++;
}

void *__wrap_GOMP_barrier()
{
	static uint32_t n = 0;
	
	check_init();
	
	__real_GOMP_barrier();
	#pragma omp master
	{
		//printf("pbarrier %llu\n", n);
		#if defined(MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE)
			mapping_lib_remap(3, n);
		#elif defined(MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE)
			{
				thread_mapping_t *tm;
			
				tm = wrapper_get_comm_pattern(TYPE_BARRIER, n);
				assert(tm != NULL);
			
				remap_check_migrate(tm, TYPE_BARRIER);
			}
		#elif defined(MAPPING_LIB_REAL_REMAP_SIMICS)
			remap_check_migrate(TYPE_BARRIER);
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

