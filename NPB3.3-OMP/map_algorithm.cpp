#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <limits>
#include <iostream>

#include <lemon/smart_graph.h>
#include <lemon/matching.h>

#include "map_algorithm.h"

using namespace lemon;

template <class TGRAPH, class TWEIGHT, class TEDGEMAP>
class floyd_warshall_t {
	private:

		// result
		TWEIGHT **distance;
		
		// graph
		TGRAPH *graph;
		TEDGEMAP *weights;
		uint32_t nvertex;
	
		//TWEIGHT **adjacency_matrix;

		inline void set(uint32_t n1, uint32_t n2, TWEIGHT v) {
			this->distance[n1][n2] = v;
		}		
		
		void alloc();
		void destroy();
	
	public:

		floyd_warshall_t();
		~floyd_warshall_t();
		void init(TGRAPH *graph, TEDGEMAP *weights, uint32_t nvertex);
		void run();
		
		inline TWEIGHT get(uint32_t n1, uint32_t n2) {
			return this->distance[n1][n2];
		}
};

template <class TGRAPH, class TWEIGHT, class TEDGEMAP>
floyd_warshall_t<TGRAPH, TWEIGHT, TEDGEMAP>::floyd_warshall_t()
{
	//this->adjacency_matrix = NULL;
	this->distance = NULL;
}

template <class TGRAPH, class TWEIGHT, class TEDGEMAP>
floyd_warshall_t<TGRAPH, TWEIGHT, TEDGEMAP>::~floyd_warshall_t()
{
	this->destroy();
}


template <class TGRAPH, class TWEIGHT, class TEDGEMAP>
void floyd_warshall_t<TGRAPH, TWEIGHT, TEDGEMAP>::init(TGRAPH *graph, TEDGEMAP *weights, uint32_t nvertex)
{
	this->graph = graph;
	this->weights = weights;

//	this->adjacency_matrix = (TWEIGHT**)calloc(this->nvertex * this->nvertex, sizeof(TWEIGHT*));
//	assert(this->adjacency_matrix != NULL);

//	for (uint32_t i=0; i<this->nvertex; i++) {
//		for (uint32_t j=0; j<this->nvertex; j++) {
//			this->adjacency_matrix[i*this->nvertex + j] = NULL;
//		}
//	}

//	for (SmartGraph::EdgeIt i(this->graph); i!=INVALID; ++i) {
//		uint32_t n1 = this->graph.id(this->graph.u(i));
//		uint32_t n2 = this->graph.id(this->graph.v(i));

//		this->adjacency_matrix[n1*this->nvertex + n2] = &((*(this->weights))[i]);
//		this->adjacency_matrix[n2*this->nvertex + n1] = &((*(this->weights))[i]);
//	}

	if (this->distance != NULL) {
		if (this->nvertex != nvertex) {
			this->destroy();
			this->nvertex = nvertex;
			this->alloc();
		}
	}
	else {
		this->nvertex = nvertex;
		this->alloc();
	}
}

template <class TGRAPH, class TWEIGHT, class TEDGEMAP>
void floyd_warshall_t<TGRAPH, TWEIGHT, TEDGEMAP>::alloc()
{
	this->distance = (TWEIGHT**)calloc(this->nvertex, sizeof(TWEIGHT*));
	assert(this->distance != NULL);
	
	this->distance[0] = new TWEIGHT[ this->nvertex * this->nvertex ];
	for (uint32_t i=1; i<this->nvertex; i++) {
		this->distance[i] = this->distance[0] + i*this->nvertex;
	}
}

template <class TGRAPH, class TWEIGHT, class TEDGEMAP>
void floyd_warshall_t<TGRAPH, TWEIGHT, TEDGEMAP>::destroy()
{
	if (this->distance) {
		delete this->distance[0];
		free(this->distance);
		this->distance = NULL;
	}
}

template <class TGRAPH, class TWEIGHT, class TEDGEMAP>
void floyd_warshall_t<TGRAPH, TWEIGHT, TEDGEMAP>::run()
{
	for (uint32_t i=0; i<this->nvertex-1; i++) {
		this->set(i, i, 0);
		for (uint32_t j=i+1; j<this->nvertex; j++) {
//			this->set(i, j, std::numeric_limits<TWEIGHT>::max());
//			this->set(j, i, std::numeric_limits<TWEIGHT>::max());
			this->set(i, j, std::numeric_limits<uint16_t>::max());
			this->set(j, i, std::numeric_limits<uint16_t>::max());
		}
	}
	
	for (SmartGraph::EdgeIt i(*this->graph); i!=INVALID; ++i) {
		this->set(this->graph->id(this->graph->u(i)), this->graph->id(this->graph->v(i)), (*(this->weights))[i] );
		this->set(this->graph->id(this->graph->v(i)), this->graph->id(this->graph->u(i)), (*(this->weights))[i] );
	}
	
	for (uint32_t k=0; k<this->nvertex; k++) {
		for (uint32_t i=0; i<this->nvertex; i++) {
			for (uint32_t j=0; j<this->nvertex; j++) {
				TWEIGHT sum;
				sum = this->get(i, k) + this->get(k, j);
				//printf("sum is %llu\n", sum);
				if (this->get(i, j) > sum) {
					this->set(i, j, sum);
				}
			}
		}
	}
}

/*****************************************/

//static SmartGraph graph;
//static SmartGraph::Node *nodes;
//static SmartGraph::Edge *edges;
//static SmartGraph::EdgeMap<uint64_t> weight(graph);

static uint16_t **distance_between_cores;
static uint16_t nthreads;

/*****************************************/

static void load_harpertown_hierarchy()
{
	SmartGraph graph;
	SmartGraph::Node nodes_cores[8], nodes_caches[4], nodes_procs[2], node_root;
	SmartGraph::Edge edges[14];
	uint32_t i;
	SmartGraph::EdgeMap<uint64_t> weight(graph);
	
	nthreads = 8;
	distance_between_cores = (uint16_t**)calloc(nthreads, sizeof(uint16_t*));
	assert(distance_between_cores != NULL);
	distance_between_cores[0] = (uint16_t*)calloc(nthreads*nthreads, sizeof(uint16_t));
	assert(distance_between_cores[0] != NULL);
	for (i=1; i<nthreads; i++)
		distance_between_cores[i] = distance_between_cores[0] + i*nthreads;
	
	for (i=0; i<8; i++) {
		nodes_cores[i] = graph.addNode();
	}
	
	for (i=0; i<4; i++) {
		nodes_caches[i] = graph.addNode();
	}
	
	for (i=0; i<2; i++) {
		nodes_procs[i] = graph.addNode();
	}
	
	node_root = graph.addNode();
	
	// link cores to cache 0
	edges[0] = graph.addEdge(nodes_cores[0], nodes_caches[0]);
	edges[1] = graph.addEdge(nodes_cores[1], nodes_caches[0]);
	
	// link cores to cache 1
	edges[2] = graph.addEdge(nodes_cores[2], nodes_caches[1]);
	edges[3] = graph.addEdge(nodes_cores[3], nodes_caches[1]);

	// link cores to cache 2
	edges[4] = graph.addEdge(nodes_cores[4], nodes_caches[2]);
	edges[5] = graph.addEdge(nodes_cores[5], nodes_caches[2]);

	// link cores to cache 3
	edges[6] = graph.addEdge(nodes_cores[6], nodes_caches[3]);
	edges[7] = graph.addEdge(nodes_cores[7], nodes_caches[3]);

	// link caches to proc 0
	edges[8] = graph.addEdge(nodes_caches[0], nodes_procs[0]);
	edges[9] = graph.addEdge(nodes_caches[1], nodes_procs[0]);
	
	// link caches to proc 1
	edges[10] = graph.addEdge(nodes_caches[2], nodes_procs[1]);
	edges[11] = graph.addEdge(nodes_caches[3], nodes_procs[1]);
	
	// link root node
	edges[12] = graph.addEdge(nodes_procs[0], node_root);
	edges[13] = graph.addEdge(nodes_procs[1], node_root);

	for (i=0; i<14; i++) {
		weight[edges[i]] = 1;
	}

	floyd_warshall_t<SmartGraph, uint64_t, SmartGraph::EdgeMap<uint64_t> > floyd;
	
	floyd.init(&graph, &weight, 15);
	
	floyd.run();

	for (i=0; i<8; i++) {
		for (uint32_t j=0; j<8; j++) {
			//std::cout << "distancia " << i << " e " << j << " eh " << floyd.get(i, j) << "\n";
			distance_between_cores[i][j] = floyd.get(i, j) / 2;
		}
	}
	
//	for (i=0; i<8; i++) {
//		for (uint32_t j=0; j<8; j++) {
//			std::cout << "distancia " << i << " e " << j << " eh " << distance_between_cores[i][j] << "\n";
//		}
//	}
}

/*****************************************/

extern "C"
{

	void wrapper_generate_thread_mapping(thread_mapping_t *tm)
	{
		
	}
	
	void wrapper_load_harpertown_hierarchy()
	{
		load_harpertown_hierarchy();
	}

} // end extern "C"
