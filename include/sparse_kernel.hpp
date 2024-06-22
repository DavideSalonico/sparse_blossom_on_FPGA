#ifndef SPARSE_KERNEL_HPP
#define SPARSE_KERNEL_HPP

#include <math.h>
#include <ap_int.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
//#include "hls_print.h"

#define MAX_N_NODES 500    //??? Comprehend also boundary nodes
#define MAX_N_OBS 500      //???
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
    node_idx_t node; //node index
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
    node_t shell_area[4]; //4 random
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

enum choice_t
{
    LOAD_GRAPH = 0,
    DECODE = 1
};

typedef ap_uint<MAX_N_NODES> syndr_t;
typedef ap_uint<MAX_N_OBS> corrections_t;

//extern "C" void sparse_top(FpgaGraph*, syndr_t, corrections_t *);
extern "C" void sparse_top(choice_t, FpgaGraph*, syndr_t, corrections_t *);

#endif