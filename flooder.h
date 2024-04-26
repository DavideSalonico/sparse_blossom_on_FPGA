#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "cache.h"

//insert define
#define N_DEC_NODES 1000
#define NODE_IDX_BIT log(2, N_DEC_NODES)
#define N_NEIGH 4
#define N_REGIONS N_DEC_NODES
#define REGION_IDX_BIT log(2, N_REGIONS)
#define N_OBS 1400

//typedefs
typedef int flood_type_t;
typedef int node_t; 		//index of detector node
typedef int time_t;
typedef int region_t;
typedef int obs_mask_t[N_OBS];

typedef struct {
	flood_type_t type;
	time_t time;
	node_t node;
} flood_event_t;

typedef struct{
	node_t src;
	node_t dest;
	obs_mask_t obs_mask;
} compressed_edge_t;

typedef struct{
	region_t region;
	compressed_edge_t ce;
} mwpm_event_t;

typedef struct{ //TODO: change types to be more adaptable
	node_t index;
	region_t region_idx;
	region_t top_region_idx;
	int wrapped_radius_cached;
	node_t reached_from_source;
	obs_mask_t obs_inter;
	int radius_of_arrival;
	node_t neigh[4];
	int neigh_weights[4];
	obs_mask_t neigh_obs[4];
} node_data_t;

typedef enum f_event_type{
	NODE = 0,
	REGION_SHR = 1
};

//typedef region_data

extern "C" {
	init();
	decode(syndrome, corrections, weights);
}
