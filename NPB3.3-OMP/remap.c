#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <map_algorithm.h>
#include <mapping-lib.h>

/*
	types:
		0: time step
		1: parallel_start
		2: parallel_end
		3: barrier
*/

static uint32_t *current_map, *new_map;
static uint16_t nthreads;

//static thread_mapping_t map;

static void check_init()
{
	static char init = 0;
	if (!init) {
		init = 1;
		assert(wrapper_dynmap_initialized());
		
		nthreads = wrapper_get_threads();

		current_map = (uint32_t*)calloc(nthreads, sizeof(uint32_t));
		assert(current_map != NULL);
		
		new_map = (uint32_t*)calloc(nthreads, sizeof(uint32_t));
		assert(new_map != NULL);
		
		mapping_lib_copy_initial_map(current_map);

		//{ int i; for (i=0; i<nthreads; i++) { current_map[i] = 0; } }
		{ int i; printf("init with map: "); for (i=0; i<nthreads; i++) { printf("%i,", current_map[i]); } printf("\n"); }
	}
}

#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE
	static void remap_check_migrate(thread_mapping_t *tm, int type)
	{
		static uint32_t n_migrations = 0;
		uint32_t diff, i;
		
//		if (type != 0)
//			return;

		tm->map = new_map;
		wrapper_generate_thread_mapping(tm);
		
		diff = wrapper_generate_difference_between_mappings(current_map, new_map, tm->nthreads);
//		{ int i; printf("new map type %i: ", type); for (i=0; i<nthreads; i++) { printf("%i,", new_map[i]); } printf("\n diff %i\n\n", diff); } getchar();
		if (diff > 0) {
			n_migrations++;
			printf("migrando %i diff %u\n", n_migrations, diff);

//			{ int i; printf("new map type %i: ", type); for (i=0; i<nthreads; i++) { printf("%i,", new_map[i]); } printf("\n"); }

			for (i=0; i<nthreads; i++) { // migrate
				mapping_lib_set_aff_of_thread(i, new_map[i]);
			}

			for (i=0; i<nthreads; i++)
				current_map[i] = new_map[i];
		}
	}
#endif

void remap_time_step(int step)
{
	check_init();
	
	//printf("xxx %d", step);

	#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE
		mapping_lib_remap(0, step);
	#endif

	#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE
		{
			thread_mapping_t *tm;
			
			tm = wrapper_get_comm_pattern(0, step);
			assert(tm != NULL);
			
			remap_check_migrate(tm, 0);
			
			//{int i, j; printf("comm matrix(type %i value %i)\n", 0, step); for (i=0; i<tm->nthreads; i++) { for (j=0; j<tm->nthreads; j++) { printf("  %llu", tm->comm_matrix[i][j]); } printf("\n"); } printf("\n"); }
		}
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
	#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE
		mapping_lib_remap(1, n);
	#endif

	#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE
		{
			thread_mapping_t *tm;
			
			tm = wrapper_get_comm_pattern(1, n);
			assert(tm != NULL);
			
			remap_check_migrate(tm, 1);
		}
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
	#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE
		mapping_lib_remap(2, n);
	#endif

	#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE
		{
			thread_mapping_t *tm;
			
			tm = wrapper_get_comm_pattern(2, n);
			assert(tm != NULL);
			
			remap_check_migrate(tm, 2);
		}
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
		#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_SIMSIDE
			mapping_lib_remap(3, n);
		#endif
		
		#ifdef MAPPING_LIB_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE
			{
				thread_mapping_t *tm;
			
				tm = wrapper_get_comm_pattern(3, n);
				assert(tm != NULL);
			
				remap_check_migrate(tm, 3);
			}
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

