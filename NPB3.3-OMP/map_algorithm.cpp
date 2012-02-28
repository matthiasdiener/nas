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

enum mapping_hierarchy_t {
	MAPPING_HIERARCHY_HARPERTOWN,
	MAPPING_HIERARCHY_NULL // used to detect when the hierarchy is not initialized
};

/*****************************************/

//static SmartGraph graph;
//static SmartGraph::Node *nodes;
//static SmartGraph::Edge *edges;
//static SmartGraph::EdgeMap<uint64_t> weight(graph);

static uint16_t **distance_between_cores;
static uint16_t nthreads;
static mapping_hierarchy_t machine = MAPPING_HIERARCHY_NULL;

/*****************************************/

static void load_harpertown_hierarchy()
{
	SmartGraph graph;
	SmartGraph::Node nodes_cores[8], nodes_caches[4], nodes_procs[2], node_root;
	SmartGraph::Edge edges[14];
	uint32_t i;
	SmartGraph::EdgeMap<uint64_t> weight(graph);
	
	machine = MAPPING_HIERARCHY_HARPERTOWN;
	
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

struct tm_result_node_t {
	tm_result_node_t *left, *right;
	uint32_t coreid;
};

#define COREID_NONCORE 0xFFFFFFFF

static void generate_matching(SmartGraph& graph, SmartGraph::Node *nodes, SmartGraph::Edge *edges, SmartGraph::EdgeMap<uint64_t>& weight, SmartGraph::NodeMap<tm_result_node_t>& nodemap, tm_result_node_t *pairs_found, int *found_buffer, uint32_t nnodes)
{
	uint32_t i, z;
	
	MaxWeightedPerfectMatching<SmartGraph, SmartGraph::EdgeMap<uint64_t> > edmonds(graph, weight);
	edmonds.run();
	
	for (i=0; i<nnodes; i++) {
		found_buffer[i] = 0;
	}
	
	z = 0;
	for (i=0; i<nnodes; i++) {
		if (found_buffer[graph.id(nodes[i])] == 0) {
			found_buffer[graph.id(nodes[i])] = 1;
			found_buffer[graph.id(edmonds.mate(nodes[i]))] = 1;
		
			node_pairs[z].left = nodemap[ nodes[i] ];
			node_pairs[z].right = nodemap[ edmonds.mate(nodes[i]) ];
			
			z++;
		}
	}
}

static void generate_thread_mapping_harpertown(thread_mapping_t *tm)
{
	uint32_t i, j, z;

	int found1[8];
	uint32_t nedges1, nnodes1;

	SmartGraph graph1;
	SmartGraph::Node *nodes1;
	SmartGraph::Edge *edges1;
	SmartGraph::EdgeMap<uint64_t> weight1(graph1);
	SmartGraph::NodeMap<tm_result_node_t> nodemap1(graph1);

	nnodes1 = nthreads;

	// we want a complete graph
	nedges1 = ((nnodes1 + 1) * nnodes1) / 2;

	nodes1 = new SmartGraph::Node[nnodes1];
	edges1 = new SmartGraph::Edge[nedges1];
	
	for (i=0; i<nnodes1; i++) {
		nodes1[i] = graph1.addNode();
		nodemap1[ nodes1[i] ].coreid = i;
		nodemap1[ nodes1[i] ].left = NULL;
		nodemap1[ nodes1[i] ].right = NULL;
	}
	
	z = 0;
	for (i=0; i<nnodes1-1; i++) {
		for (j=i+1; j<nnodes1; j++) {
			edges1[z] = graph1.addEdge(nodes1[i], nodes1[j]);
			weight1[edges1[z]] = tm->comm_matrix[i][j];
			z++;
		}
	}
	
/*	z = 0;
	for (i=0; i<nnodes1-1; i++) {
		for (j=i+1; j<nnodes1; j++) {
			weight1[edges1[z]] = tm->comm_matrix[i][j];
			z++;
		}
	}*/

	generate_matching(graph1, nodes1, edges1, weight1, found1, nnodes1);

	delete nodes1;
	delete edges1;
	
	/* regenerate communication matrix */

	int found2[4], found1_verified[8];
	uint32_t nedges2, nnodes2;

	SmartGraph graph2;
	SmartGraph::Node *nodes2;
	SmartGraph::Edge *edges2;
	SmartGraph::EdgeMap<uint64_t> weight2(graph2);

	nnodes2 = nthreads / 2;

	// we want a complete graph
	nedges2 = ((nnodes2 + 1) * nnodes2) / 2;

	nodes2 = new SmartGraph::Node[nnodes2];
	edges2 = new SmartGraph::Edge[nedges2];
	
	for (i=0; i<nthreads; i++) {
		found1_verified[i] = 0;
	}

	z = 0;
	for (i=0; i<nthreads; i++) {
		if (found1_verified[i] == 0) {
			found1_verified[i] = 1;
			found1_verified[found1[i]] = 1;
			
			nodes2[z] = graph2.addNode();
			nodemap2[ nodes2[z] ].coreid = COREID_NONCORE;
			nodemap2[ nodes2[z] ].left = &nodemap1[ nodes1[] ];
			
			z++;
		}
	}
	
	for (i=0; i<nnodes2; i++) {
		nodes2[i] = graph2.addNode();
		nodemap2[ nodes2[i] ].coreid = COREID_NONCORE;
		nodemap2[ nodes2[i] ].left = NULL;
		nodemap2[ nodes2[i] ].right = NULL;
	}
	
	z = 0;
	for (i=0; i<nnodes2-1; i++) {
		for (j=i+1; j<nnodes2; j++) {
			edges2[z] = graph2.addEdge(nodes2[i], nodes2[j]);
			weight2[edges2[z]] = tm->comm_matrix[i][j];
			z++;
		}
	}
	
	z = 0;
	for (i=0; i<nthreads; i++) {
		edges2[z] = graph2.addEdge(nodes2[i], nodes2[found[i]]);
		
		z++;
	}
	
	/*z = 0;
	for (i=0; i<nnodes2-1; i++) {
		for (j=i+1; j<nnodes2; j++) {
			weight2[edges2[z]] = tm->comm_matrix[i][j];
			z++;
		}
	}*/

	generate_matching(graph2, nodes2, edges2, weight2, found2, nnodes2);

	delete nodes2;
	delete edges2;
}

/*****************************************/

extern "C"
{

	void wrapper_generate_thread_mapping(thread_mapping_t *tm)
	{
		switch (machine) {
			case MAPPING_HIERARCHY_HARPERTOWN:
				generate_thread_mapping_harpertown(tm);
				break;
			
			default:
				assert(0);
		}
	}
	
	void wrapper_load_harpertown_hierarchy()
	{
		load_harpertown_hierarchy();
	}

} // end extern "C"
