#ifndef SPARSE_KERNEL_CPP
#define SPARSE_KERNEL_CPP

#include "/home/users/davide.salonico/sparse_blossom_prj/include/sparse_kernel.hpp"
#include "/home/users/davide.salonico/sparse_blossom_prj/DaCH/src/cache.h"

#define RD_ENABLED true
#define WR_ENABLED true
#define PORTS 1                    // number of ports (1 if WR_ENABLED is true).
#define MAIN_SIZE_NODE ((unsigned int)(1 << (unsigned int)ceil(log2(MAX_N_NODES)))) // size of the original array.
#define MAIN_SIZE_REGION ((unsigned int)(1 << (unsigned int)ceil(log2(N_REGIONS)))) // size of the original array.
#define MAIN_SIZE_ALT_TREE ((unsigned int)(1 << (unsigned int)ceil(log2(ALTTREEEDGE_MAX))))// size of the original array.
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

FpgaGraph init_graph;

int n_nodes;
int n_obs;
node_cache node_lut{static_cast<node_data_t * const>(init_graph.nodes)};
region_cache region_lut{static_cast<region_data_t * const>(init_graph.regions)};
alt_tree_cache alt_tree_lut{static_cast<altTreeNode_data_t * const>(init_graph.alttree)};

template<typename T1, typename T2, typename T3>
void decode(T1& nodes, T2& regions, T3& alt_tree, syndr_t syndrome, corrections_t * corrections){
    #ifndef __SYNTHENSYS__
    for (int i = 1; i <= 5; ++i) {
        node_data_t node = nodes[i];
        std::cout << "Node: " << node.index << std::endl;
    }

    #endif //__SYNTHESYS__
    return;
}

extern "C" void sparse_top(choice_t choice, FpgaGraph* graph, syndr_t syndrome, corrections_t * corrections)
{
/*
#pragma HLS INTERFACE m_axi port = a_arr offset = slave bundle = gmem0 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = b_arr offset = slave bundle = gmem1 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = c_arr offset = slave bundle = gmem2 latency = 0 depth = 1024
#pragma HLS INTERFACE ap_ctrl_hs port = return
*/

    if (choice == LOAD_GRAPH)
    {
#pragma HLS dataflow disable_start_propagation
        /*
        static int n_nodes = graph.num_nodes;
        static int n_obs = graph.num_obs;
        static node_cache node_lut(graph.nodes);
        static region_cache region_lut(graph.regions);
        static alt_tree_cache alt_tree_lut(graph.alttree);
        */
        
        n_nodes = graph->num_nodes;
        n_obs = graph->num_obs;
        /*
        node_lut(graph->nodes);
        region_lut(graph->regions);
        alt_tree_lut(graph->alttree);
        */
        for(int i = 0; i < n_nodes; i++){
            node_lut[i] = graph->nodes[i];
        }

        //DEBUG
        #ifndef __SYNTHESYS__
        std::cout << "num_nodes: " << n_nodes << std::endl;
        std::cout << "n_obs: " << n_obs << std::endl;
        for(int i = 0; i < n_nodes; i++){
            node_data_t node = node_lut[i];
            std::cout << "Node: " << node.index << std::endl;
        }
        #endif //_SYNTHESYS__
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

#endif //SPARSE_KERNEL_CPP



