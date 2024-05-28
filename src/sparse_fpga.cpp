#include "DaCH/src/cache.h" // or include DaCH src path in the cflags
#include <math.h>
#include "ap_int.h"

#define MAX_N_NODES 1000 //??? Comprehend also boundary nodes
#define MAX_N_OBS 1400   //???
#define N_NEIGH 4

#define ALTTREEEDGE_MAX 10
#define SHELL_AREA_MAX 50
#define BLOSSOM_CHILDREN_MAX 10

#define N_REGIONS MAX_N_NODES // Exagerated assumption

#define NODE_BIT ceil(log2(MAX_N_NODES + 1))
#define REGION_BIT ceil(log2(N_REGIONS + 1))
#define OBS_BIT ceil(log2(MAX_N_OBS))

#define LLONG_MAX 2147483647

typedef ap_uint<NODE_BIT> node_idx_t;
typedef ap_uint<REGION_BIT> region_idx_t;
typedef ap_uint<OBS_BIT> obs_mask_t;

typedef uint32_t weight_t; // TODO: define more efficient

#define NULL_NODE 0
#define NULL_REGION 0

enum fe_type
{
    NODE = 0,
    REGION_SHR = 1
};

typedef struct
{
    fe_type type;
    time_t time;
    node_idx_t node; // node index
} flood_event_t;

typedef struct
{
    node_idx_t src;
    node_idx_t dest;
    obs_mask_t obs_mask;
} compressed_edge_t;

typedef struct
{
    region_idx_t region;
    compressed_edge_t ce;
} region_edge_t;

enum mwpm_type
{
    RegionHitRegionEventData,
    RegionHitBoundaryEventData,
    BlossomShatterEventData
};

typedef struct
{ // Optimizable
    region_idx_t region_src;
    region_idx_t region_dst;
    region_idx_t region;
    compressed_edge_t ce;
    region_idx_t blossom_region;
    region_idx_t in_parent_region;
    region_idx_t in_child_region;
    enum mwpm_type type;
    /*
     RegionHitRegionEventData

         GraphFillRegion *region1; //region_src
         GraphFillRegion *region2; //region_dst
         CompressedEdge edge;
     */

    /*
     RegionHitBoundaryEventData

         GraphFillRegion *region; //region
         CompressedEdge edge;

     */

    /*
     BlossomShatterEventData

         GraphFillRegion *blossom_region;
         GraphFillRegion *in_parent_region;
         GraphFillRegion *in_child_region;
     */
} mwpm_event_t;

typedef struct
{
    node_idx_t index;
    region_idx_t region_idx;
    region_idx_t top_region_idx;
    radius_t wrapped_radius_cached;
    node_idx_t reached_from_source;
    obs_mask_t obs_inter;
    radius_t radius_of_arrival;
    node_idx_t neigh[N_NEIGH]; // if node.neigh[2] == 0 -> node hasn't the neigh[2]
    weight_t neigh_weights[N_NEIGH];
    obs_mask_t neigh_obs[N_NEIGH];
} node_data_t;

typedef struct
{
    region_idx_t index;
    region_idx_t blossom_parent_region_idx;
    region_idx_t blossom_parent_top_region_idx;
    altTreeNode_t *alt_tree_node;
    radius_t radius;
    // QueuedEventTracker shrink_event_traker
    // Match match
    node_data_t shell_area[SHELL_AREA_MAX];
    region_edge_t blossom_children[BLOSSOM_CHILDREN_MAX];
} region_data_t;

typedef int altTreeNode_t;

typedef struct
{
    compressed_edge_t edge;
    altTreeNode_t alt_tree_node;
} altTreeEdge_t;

typedef struct
{
    altTreeNode_t index;
    region_idx_t inner_region_idx;
    region_idx_t outer_region_idx;
    compressed_edge_t inner_to_outer_edge;
    altTreeEdge_t parent;
    altTreeEdge_t children[ALTTREEEDGE_MAX];
    bool visited;
} altTreeNode_data_t;

typedef struct
{
    time_t value;
    radius_status_t status;
} radius_t;

enum radius_status_t
{
    GROWING = 1,
    FROZEN = 0,
    SHRINKING = -1
}

typedef struct
{
    int num_nodes;
    int num_obs;
    node_data_t nodes[MAX_N_NODES];
    region_data_t regions[N_REGIONS] = NULL;
    altTreeNode_data_t alttree[ALTTREEEDGE_MAX] = NULL;
} FpgaGraph;

#define RD_ENABLED true
#define WR_ENABLED true
#define PORTS 1                    // number of ports (1 if WR_ENABLED is true).
#define MAIN_SIZE_NODE MAX_N_NODES // size of the original array.
#define MAIN_SIZE_REGION N_REGIONS // size of the original array.
#define MAIN_SIZE_ALT_TREE 10000   // size of the original array.
#define N_SETS 16                  // the number of L2 sets (1 for fully-associative cache).
#define N_WAYS 16                  // the number of L2 ways (1 for direct-mapped cache).
#define N_WORDS_PER_LINE 16        // the size of the cache line, in words.
#define LRU true                   // the replacement policy least-recently used if true, last-in first-out otherwise.
#define N_L1_SETS 0                // the number of L1 sets.
#define N_L1_WAYS 0                // the number of L1 ways.
#define SWAP_TAG_SET false         // the address bits mapping
#define LATENCY 2                  // the request-response distance of the L2 cache

typedef cache<node_data_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE_NODE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, SWAP_TAG_SET, LATENCY> node_cache;
typedef cache<region_data_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE_REGION, N_SETS, N_WAYS, N_WORDS_PER_LINE.LRU, SWAP_TAG_SET, LATENCY> region_cache;
typedef cache<altTreeNode_data_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE_ALT_TREE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, SWAP_TAG_SET, LATENCY> alt_tree_cache;

typedef ap_uint<MAX_N_NODES> syndr_t;
typedef ap_uint<MAX_N_OBS> corrections_t;

enum choice_t
{
    LOAD_GRAPH = 0,
    DECODE = 1
}

extern "C" void
sparse_top(choice_t choice, syndr_t syndrome[], corrections_t corrections[])
{
#pragma HLS INTERFACE m_axi port = a_arr offset = slave bundle = gmem0 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = b_arr offset = slave bundle = gmem1 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = c_arr offset = slave bundle = gmem2 latency = 0 depth = 1024
#pragma HLS INTERFACE ap_ctrl_hs port = return

    node_cache node_lut;
    region_cache region_lut;
    alt_tree_cache alt_tree_lut;

    if (choice == LOAD_GRAPH)
    {
        FpgaGraph graph = load_graph_from_file();
#pragma HLS dataflow disable_start_propagation
        node_lut(graph.nodes);
        region_lut(graph.regions);
        alt_tree_lut(graph.alt_tree);
        cache_wrapper(load_graph<node_cache, region_cache, alt_tree_cache>, node_lut, region_lut, alt_tree_lut);
    }
    else
    {
#pragma HLS dataflow disable_start_propagation
        cache_wrapper(decode<node_cache, region_cache, alt_tree_cache>, node_lut)
    }

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
           b_cache.get_n_hits(0), b_cache.get_n_reqs(0));
    printf("C hit ratio = L1=%d/%d; L2=%d/%d\n",
           c_cache.get_n_l1_hits(0), c_cache.get_n_l1_reqs(0),
           c_cache.get_n_hits(0), c_cache.get_n_reqs(0));
#endif /* PROFILE */
#endif /* __SYNTHESIS__ */
}

int main() //change with real data_types
{
    data_type a_arr[N * M];
    data_type b_arr[M * P];
    data_type c_arr[N * P] = {0};
    data_type c_arr_ref[N * P] = {0};

    for (auto i = 0; i < N * M; i++)
        a_arr[i] = i;
    for (auto i = 0; i < M * P; i++)
        b_arr[i] = i;

    // matrix multiplication with caches
    matmult_top(a_arr, b_arr, c_arr);
    // standard matrix multiplication
    multiply(a_arr, b_arr, c_arr_ref);

    int err = 0;
    for (auto i = 0; i < N * P; i++)
    {
        if (c_arr[i] != c_arr_ref[i])
        {
            err++;
            printf("Mismatch: %d %d\n", c_arr[i], c_arr_ref[i]);
        }
    }

    return err;
}
