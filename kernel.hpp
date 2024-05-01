#ifndef _KERNEL_HPP
#define _KERNEL_HPP

#include "DaCH/src/cache.h" // or include DaCH src path in the cflags
#include <math.h>
#include "ap_int.h"

#define MAX_N_NODES 1000
#define MAX_N_OBS 1400
#define N_NEIGH 4

#define N_REGIONS MAX_N_NODES

#define NODE_BIT ceil(log2(MAX_N_NODES))
#define REGION_BIT ceil(log2(N_REGIONS))
#define OBS_BIT ceil(log2(MAX_N_OBS))

#define LLONG_MAX 2147483647

typedef ap_uint<NODE_BIT> node_idx_t;
typedef ap_uint<REGION_BIT> region_idx_t;
typedef ap_uint<OBS_BIT> obs_mask_t;

typedef int radius_t; //TODO: define more efficient
typedef uint weight_t; //TODO: define more efficient

typedef struct {
    fe_type_t type;
    time_t time;
    node_idx_t node; //node index
} flood_event_t;

typedef struct{
    node_idx_t src;
    node_idx_t dest;
    obs_mask_t obs_mask;
} compressed_edge_t;

typedef struct{
    region_idx_t region;
    compressed_edge_t ce;
} region_edge_t;

typedef struct{
    region_idx_t region;
    compressed_edge_t ce;
    //?
} mwpm_event_t;

typedef struct{ //TODO: change types to be more adaptable
    node_idx_t index;
    region_idx_t region_idx;
    region_idx_t top_region_idx;
    radius_t wrapped_radius_cached;
    node_idx_t reached_from_source;
    obs_mask_t obs_inter;
    radius_t radius_of_arrival;
    node_idx_t neigh[4]; //if node.neigh[2] == 0 -> node hasn't the neigh[2]
    weight_t neigh_weights[4];
    obs_mask_t neigh_obs[4];
} node_data_t;

typedef struct{ //TODO: change types to be more adaptable
    region_idx_t blossom_parent_region_idx;
    region_idx_t blossom_parent_top_region_idx;
    //AltTreeNode *alt_tree_node
    radius_t radius;
    //QueuedEventTracker shrink_event_traker
    //Match match
    node_data_t shell_area[4]; //4 random
    region_edge_t blossom_children[4]; //4 random
} region_data_t;

typedef enum fe_type_t{
    NODE = 0,
    REGION_SHR = 1
};

#define RD_ENABLED true
#define WR_ENABLED true
#define PORTS 1                     // number of ports (1 if WR_ENABLED is true).
#define MAIN_SIZE_NODE MAX_N_NODES  // size of the original array.
#define MAIN_SIZE_REGION N_REGIONS  // size of the original array.
#define MAIN_SIZE_ALT_TREE 10000    // size of the original array.
#define N_SETS 1                    // the number of L2 sets (1 for fully-associative cache).
#define N_WAYS 1                    // the number of L2 ways (1 for direct-mapped cache).
#define N_WORDS_PER_LINE 1          // the size of the cache line, in words.
#define LRU true                    // the replacement policy least-recently used if true, last-in first-out otherwise.
#define N_L1_SETS 0                 // the number of L1 sets.
#define N_L1_WAYS 0                 // the number of L1 ways.
#define SWAP_TAG_SET true           // the address bits mapping
#define LATENCY 2                   // the request-response distance of the L2 cache

typedef cache<node_data_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE_NODE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, SWAP_TAG_SET, LATENCY> node_cache;
typedef cache<region_data_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE_REGION, N_SETS, N_WAYS, N_WORDS_PER_LINE. LRU, SWAP_TAG_SET, LATENCY> region_cache;
typedef cache<alt_node_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE_ALT_TREE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, SWAP_TAG_SET, LATENCY> alt_tree_cache;

typedef ap_uint<MAX_N_NODES> syndr_t;
typedef ap_uint<MAX_N_OBS> corr_t;

extern "C" void compute_corr(syndr_t *, corr_t *);

#endif
