#ifndef __map_algorithm_h__
#define __map_algorithm_h__

#include <stdint.h>

/*****************************************/

#ifdef __cplusplus
extern "C"
{
#endif

	struct thread_mapping_t {
		uint32_t *map;
		uint64_t *comm_matrix;
		uint32_t nthreads;
	};
	
	void wrapper_load_harpertown_hierarchy();

#ifdef __cplusplus
} // end extern "C"
#endif

/*****************************************/

#endif
