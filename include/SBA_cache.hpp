#ifndef SBA_CACHE_HPP
#define SBA_CACHE_HPP

#include "/home/users/davide.salonico/sparse_blossom_prj/DaCH/src/cache.h"
#include "/home/users/davide.salonico/sparse_blossom_prj/include/SBA_kernel.hpp"


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

static FpgaGraph init_graph;

static int n_nodes;
static int n_obs;
static node_cache node_lut{static_cast<node_data_t * const>(init_graph.nodes)};
static region_cache region_lut{static_cast<region_data_t * const>(init_graph.regions)};
static alt_tree_cache alt_tree_lut{static_cast<altTreeNode_data_t * const>(init_graph.alttree)};

#endif //SBA_CACHE_HPP