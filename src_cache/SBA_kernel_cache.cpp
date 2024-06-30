#include "SBA_kernel.hpp"
#include "cache.h"


#define RD_ENABLED true
#define WR_ENABLED true
#define PORTS 1                    // number of ports (1 if WR_ENABLED is true).
#define MAIN_SIZE_NODE (unsigned int)(1 << ceillog2(MAX_N_NODES)) // size of the original array.
#define MAIN_SIZE_REGION (unsigned int)(1 << ceillog2(N_REGIONS)) // size of the original array.
#define MAIN_SIZE_ALT_TREE (unsigned int)(1 << ceillog2(ALTTREEEDGE_MAX))// size of the original array.
#define N_SETS 8                  // the number of L2 sets (1 for fully-associative cache).
#define N_WAYS 8                  // the number of L2 ways (1 for direct-mapped cache).
#define N_WORDS_PER_LINE 4        // the size of the cache line, in words.
#define LRU true                   // the replacement policy least-recently used if true, last-in first-out otherwise.
#define N_L1_SETS 0                // the number of L1 sets.
#define N_L1_WAYS 0                // the number of L1 ways.
#define SWAP_TAG_SET false         // the address bits mapping
#define LATENCY 2                  // the request-response distance of the L2 cache

// typedef cache<data_type, true, false, RD_PORTS, N * M, A_L2_SETS, A_L2_WAYS, A_WORDS, false, A_L1_SETS, A_L1_WAYS, false, A_L2_LATENCY> cache_a
typedef cache<node_data_t, RD_ENABLED, WR_ENABLED, 1, N_SETS*N_WAYS*N_WORDS_PER_LINE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, 1, 1, SWAP_TAG_SET, LATENCY> node_cache;
typedef cache<region_data_t, RD_ENABLED, WR_ENABLED, 1, MAIN_SIZE_REGION, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, 1, 1, SWAP_TAG_SET, LATENCY> region_cache;
typedef cache<altTreeNode_data_t, RD_ENABLED, WR_ENABLED, 1, N_SETS*N_WAYS*N_WORDS_PER_LINE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, 1, 1, SWAP_TAG_SET, LATENCY> alt_tree_cache;

typedef struct
{
    int num_nodes;
    int num_obs;
    node_data_t nodes[MAX_N_NODES];
    region_data_t regions[N_REGIONS];
    altTreeNode_data_t alttree[ALTTREEEDGE_MAX];
} FpgaGraph;

enum choice_t
{
    LOAD_GRAPH = 0,
    DECODE = 1
};

ErrorGraph init_graph;

int n_nodes;
int n_obs;
node_cache node_lut{static_cast<node_data_t * const>(init_graph.nodes)};
region_cache region_lut{static_cast<region_data_t * const>(init_graph.regions)};
alt_tree_cache alt_tree_lut{static_cast<altTreeNode_data_t * const>(init_graph.alttree)};


template<typename T1, typename T2, typename T3>
void decode(T1& nodes, T2& regions, T3& alt_tree, syndr_t syndrome, corrections_t * corrections){
    
    // Hardcoded 
    flood_event_t fe;
    fe.node = 2;
    fe.time = 10;
    fe.type = NODE;
    f_dispatcher(fe);

    return;
}

extern "C" void sparse_top(choice_t choice, FpgaGraph* graph, syndr_t syndrome, corrections_t * corrections)
{
#pragma HLS INTERFACE m_axi port = graph offset = slave bundle = gmem0 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = corrections offset = slave bundle = gmem1 latency = 0 depth = 1024

#pragma HLS INTERFACE s_axilite port=choice bundle=control
#pragma HLS INTERFACE s_axilite port=graph bundle=control
#pragma HLS INTERFACE s_axilite port=syndrome bundle=control
#pragma HLS INTERFACE s_axilite port=corrections bundle=control

#pragma HLS INTERFACE s_axilite port = return bundle = control

    if (choice == LOAD_GRAPH)
    {
        n_nodes = graph->num_nodes;
        n_obs = graph->num_obs;

        for(int i = 0; i < n_nodes; i++){
            node_lut[i] = graph->nodes[i];
        }
    }
    else
    {
        cache_wrapper(decode<node_cache, region_cache, alt_tree_cache>, node_lut, region_lut, alt_tree_lut, syndrome, corrections);
    }
}


