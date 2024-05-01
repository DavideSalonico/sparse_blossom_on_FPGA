//#include <fstream>
//#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "cache.h" // include DaCH src path in the cflags

//insert define
#define N_DEC_NODES 1000
#define NODE_IDX_BIT log(2, N_DEC_NODES)
#define N_NEIGH 4
#define N_REGIONS N_DEC_NODES
#define REGION_IDX_BIT log(2, N_REGIONS)
#define N_OBS 1400
#define LLONG_MAX 2147483647

//typedefs
typedef int flood_type_t;
typedef int node_t;         //index of detector node
typedef int time_t;
typedef int region_t;
typedef int obs_mask_t[N_OBS];


//TODO: change when merging cache implementation
node_data_t node_lut[1000000];
region_data_t region_lut[100000];

typedef struct {
    flood_type_t type;
    time_t time;
    node_t node; //node index
} flood_event_t;

typedef struct{
    node_t src;
    node_t dest;
    obs_mask_t obs_mask;
} compressed_edge_t;

typedef struct{
    region_t region;
    compressed_edge_t ce;
} region_edge_t;

typedef struct{
    region_t region;
    compressed_edge_t ce;
    //?
} mwpm_event_t;

typedef struct{ //TODO: change types to be more adaptable
    node_t index;
    region_t region_idx;
    region_t top_region_idx;
    int wrapped_radius_cached;
    node_t reached_from_source;
    obs_mask_t obs_inter;
    int radius_of_arrival;
    node_t neigh[4]; //if node.neigh[2] == 0 -> node hasn't the neigh[2]
    int neigh_weights[4];
    obs_mask_t neigh_obs[4];
} node_data_t;

typedef struct{ //TODO: change types to be more adaptable
    region_t blossom_parent_region_idx;
    region_t blossom_parent_top_region_idx;
    //AltTreeNode *alt_tree_node
    int radius;
    //QueuedEventTracker shrink_event_traker
    //Match match
    node_data_t shell_area[4]; //4 random
    region_edge_t blossom_children[4]; //4 random
} region_data_t;


typedef enum f_event_type{
    NODE = 0,
    REGION_SHR = 1
};
