#include "cache.h"
#include <math.h>
#include <ap_int.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>

#define MAX_N_NODES 1000    //??? Comprehend also boundary nodes
#define MAX_N_OBS 1400      //???
#define N_NEIGH 4
#define ALTTREEEDGE_MAX 10
#define SHELL_AREA_MAX 4
#define BLOSSOM_CHILDREN_MAX 4
#define N_REGIONS MAX_N_NODES // Exagerated assumption
#define NODE_BIT (int) ceil(log2(MAX_N_NODES+1))
#define REGION_BIT (int) ceil(log2(N_REGIONS+1))
#define OBS_BIT (int) ceil(log2(MAX_N_OBS))

typedef ap_uint<NODE_BIT> node_idx_t;
typedef ap_uint<REGION_BIT> region_idx_t;
typedef ap_uint<OBS_BIT> obs_mask_t;

typedef uint32_t weight_t;           //TODO: define more efficient

#define NULL_NODE 0
#define NULL_REGION 0

//typedefs
typedef int flood_type_t;
typedef int node_t;         //index of detector node
typedef int pm_time_t;
typedef int region_t;
typedef int obs_int_t;
typedef int altTreeNode_t;

typedef struct {
    flood_type_t type;
    pm_time_t time;
    node_t node; //node index
} flood_event_t;

typedef struct{
    node_t src;
    node_t dest;
    //obs_mask_t obs_mask;
    obs_int_t obs_mask;
} compressed_edge_t;

typedef struct{
	region_idx_t region;
	compressed_edge_t ce;
} region_edge_t;

enum mwpm_type{
    RegionHitRegionEventData,
    RegionHitBoundaryEventData,
    BlossomShatterEventData
};

typedef struct{
    region_t region_src;
    region_t region_dst;
    region_t region;
    compressed_edge_t ce;
    region_t blossom_region;
    region_t in_parent_region;
    region_t in_child_region;
    enum mwpm_type type;
    /*
     RegionHitRegionEventData
         GraphFillRegion *region1; //region_src
         GraphFillRegion *region2; //region_dst
         CompressedEdge edge;

     RegionHitBoundaryEventData
         GraphFillRegion *region; //region
         CompressedEdge edge;

     BlossomShatterEventData
         GraphFillRegion *blossom_region;
         GraphFillRegion *in_parent_region;
         GraphFillRegion *in_child_region;
     */
} mwpm_event_t;

enum radius_status_t{
    GROWING = 1,
    FROZEN = 0,
    SHRINKING = -1
};

typedef struct{
    pm_time_t value;
    enum radius_status_t status;
} radius_t;

typedef struct{ 
    node_t index;
    region_t region_idx;
    region_t top_region_idx;
    int wrapped_radius_cached;
    node_t reached_from_source;
    //obs_mask_t obs_inter;
    obs_int_t obs_inter;
    int radius_of_arrival;
    node_t neigh[4]; //if node.neigh[2] == 0 -> node hasn't the neigh[2]
    int neigh_weights[4];
    //obs_mask_t neigh_obs[4];
    obs_int_t neigh_obs[4];
} node_data_t;


//NEW
typedef struct{
    region_t region;
    compressed_edge_t edge;
}
match_t;

typedef struct{
    region_t index;
    region_t blossom_parent_region_idx;
    region_t blossom_parent_top_region_idx;
    altTreeNode_t alt_tree_node;
    radius_t radius;
    //QueuedEventTracker shrink_event_traker
    match_t match;
    node_data_t shell_area[4]; //4 random
    region_edge_t blossom_children[4]; //4 random
} region_data_t;

typedef struct{
    compressed_edge_t edge;
    altTreeNode_t alt_tree_node;
} altTreeEdge_t;

typedef struct{ //ho aggiunto int state
    altTreeNode_t index;
    region_t inner_region_idx;
    region_t outer_region_idx;
    compressed_edge_t inner_to_outer_edge;
    altTreeEdge_t parent;
    altTreeEdge_t children[ALTTREEEDGE_MAX];
    bool visited;
    int state;
} altTreeNode_data_t;

enum f_event_type{
    NODE = 0,
    REGION_SHR = 1
};

typedef struct
{
    int num_nodes;
    int num_obs;
    node_data_t nodes[MAX_N_NODES];
    region_data_t regions[N_REGIONS];
    altTreeNode_data_t alttree[ALTTREEEDGE_MAX];
} FpgaGraph;

#define RD_ENABLED true
#define WR_ENABLED true
#define PORTS 1                    // number of ports (1 if WR_ENABLED is true).
#define MAIN_SIZE_NODE pow(2,ceil(log2(MAX_N_NODES))) // size of the original array.
#define MAIN_SIZE_REGION pow(2,ceil(log2(N_REGIONS))) // size of the original array.
#define MAIN_SIZE_ALT_TREE pow(2,ceil(log2(ALTTREEEDGE_MAX)))   // size of the original array.
#define N_SETS 16                  // the number of L2 sets (1 for fully-associative cache).
#define N_WAYS 16                  // the number of L2 ways (1 for direct-mapped cache).
#define N_WORDS_PER_LINE 4        // the size of the cache line, in words.
#define LRU true                   // the replacement policy least-recently used if true, last-in first-out otherwise.
#define N_L1_SETS 0                // the number of L1 sets.
#define N_L1_WAYS 0                // the number of L1 ways.
#define SWAP_TAG_SET false         // the address bits mapping
#define LATENCY 2                  // the request-response distance of the L2 cache

// typedef cache<data_type, true, false, RD_PORTS, N * M, A_L2_SETS, A_L2_WAYS, A_WORDS, false, A_L1_SETS, A_L1_WAYS, false, A_L2_LATENCY> cache_a
typedef cache<node_data_t, RD_ENABLED, WR_ENABLED, 1, MAIN_SIZE_NODE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, 1, 1, SWAP_TAG_SET, LATENCY> node_cache;
typedef cache<region_data_t, RD_ENABLED, WR_ENABLED, 1, MAIN_SIZE_REGION, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, 1, 1, SWAP_TAG_SET, LATENCY> region_cache;
typedef cache<altTreeNode_data_t, RD_ENABLED, WR_ENABLED, 1, MAIN_SIZE_ALT_TREE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, 1, 1, SWAP_TAG_SET, LATENCY> alt_tree_cache;

typedef ap_uint<MAX_N_NODES> syndr_t;
typedef ap_uint<MAX_N_OBS> corrections_t;

enum choice_t
{
    LOAD_GRAPH = 0,
    DECODE = 1
};

template<T1 nodes, T2 regions, T3 alt_tree>
void decode(T1 nodes, T2 regions, T3 alt_tree, syndr_t syndrome, corrections_t * corrections){
    return;
}

extern "C" void sparse_top(choice_t choice, FpgaGraph* graph, syndr_t syndrome, corrections_t * corrections)
{
#pragma HLS INTERFACE m_axi port = a_arr offset = slave bundle = gmem0 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = b_arr offset = slave bundle = gmem1 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = c_arr offset = slave bundle = gmem2 latency = 0 depth = 1024
#pragma HLS INTERFACE ap_ctrl_hs port = return

    if (choice == LOAD_GRAPH)
    {
        FpgaGraph graph = graph;
#pragma HLS dataflow disable_start_propagation
        static int n_nodes = graph.num_nodes;
        static int n_obs = graph.num_obs;
        static node_cache node_lut(graph.nodes);
        static region_cache region_lut(graph.regions);
        static alt_tree_cache alt_tree_lut(graph.alt_tree);
    }
    else
    {
#pragma HLS dataflow disable_start_propagation
        cache_wrapper(decode<node_cache, region_cache, alt_tree_cache>, node_lut, region_lut, alt_tree_lut, syndrome, corrections);
    }
}
/*

chache_if:
#ifndef __SYNTHESIS__
#ifdef PROFILE
    // TODO: change with out caches
    printf("A hit ratio = \n");
    for (auto port = 0; port < RD_PORTS; port++)
    {
        printf("\tP=%d: L1=%d/%d; L2=%d/%d\n", port,
               a_cache.get_n_l1_hits(port), a_cache.get_n_l1_reqs(port),
               a_cache.get_n_hits(port), a_cache.get_n_reqs(port));
    }
    printf("B hit ratio = L1=%d/%d; L2=%d/%d\n",
           b_cache.get_n_l1_hits(0), b_cache.get_n_l1_reqs(0),
    printf("C hit ratio = L1=%d/%d; L2=%d/%d\n",
           c_cache.get_n_l1_hits(0), c_cache.get_n_l1_reqs(0),
           c_cache.get_n_hits(0), c_cache.get_n_reqs(0));
#endif // PROFILE
#endif // __SYNTHESIS__


void trackerDequeue(flood_event_t * fe){
    fe->node = 100;
    fe->time = 100;
    fe->type = NODE;
}

template<typename T1, typename T2, typename T3>
void decode(T1 nodes, T2 regions, T3 alt_tree, syndr_t syndrome[MAX_N_NODES], err_t errors[MAX_N_OBS]){
   //PriorityQueue queue;

   // Read syndrome and create right events

   bool done;
   while(!done){
       flood_event_t fe = NULL;
       trackerDequeue(&fe);
       f_dispatcher(&fe, done);
       if(done) break;
   }

   //Compute errors from alt_tree
   //errors = ...
}
*/



