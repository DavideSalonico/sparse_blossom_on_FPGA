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

typedef ap_uint<MAX_N_NODES> syndr_t;
typedef ap_uint<MAX_N_OBS> corrections_t;

enum choice_t
{
    LOAD_GRAPH = 0,
    DECODE = 1
};

void trackerDequeue(flood_event_t * fe){
    fe->node = 100;
    fe->time = 100;
    fe->type = NODE;
}

void decode(node_data_t * nodes, region_data_t * regions, altTreeNode_data_t * alt_tree, syndr_t syndrome, corrections_t corrections){
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

extern "C" void sparse_top(choice_t choice, FpgaGraph* graph, syndr_t syndrome, corrections_t corrections)
{
#pragma HLS INTERFACE m_axi port = a_arr offset = slave bundle = gmem0 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = b_arr offset = slave bundle = gmem1 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = c_arr offset = slave bundle = gmem2 latency = 0 depth = 1024
#pragma HLS INTERFACE ap_ctrl_hs port = return

    decode(graph->nodes, graph->regions, graph->alttree, syndrome, corrections);
}



