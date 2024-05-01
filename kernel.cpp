#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
//#include "DaCH/src/cache.h" // include DaCH src path in the cflags

/*
#define RD_ENABLED true;
#define WR_ENABLED true;
//suggested from github
#define LATENCY 2;
#define LRU true;

typedef cache<node_data_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, SWAP_TAG_SET, LATENCY> node_cache;
typedef cache<region_data_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, SWAP_TAG_SET, LATENCY> region_cache;

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

typedef struct{
	region_t index;
	int data;
} region_data_t;

typedef enum f_event_type{
	NODE = 0,
	REGION_SHR = 1
};

template <typename T>
void init_a(T &a){
	for(int i=0; i<N_DEC_NODES; i++){
#pragma HLS pipeline
		a[i] = i;
	}
}

template <typename T>
void init_a(T &b){
	for(int i=0; i<N_DEC_NODES; i++){
#pragma HLS pipeline
		b[i] = i;
	}
}



int DATA_SIZE = (4 * 1024) / sizeof(uint32_t); //TODO: Change

extern "C" void decode(int *in0, int *in1, int *out0, int *out1){
#pragma HLS interface m_axi port=a bundle=gmem0
#pragma HLS interface m_axi port=b bundle=gmem1

	for(int i=0; i<DATA_SIZE; i++){
#pragma HLS pipeline
		out1[i] = in0[i];
		out0[i] = in1[i];
	}
}

extern "C" void decode(int *in0, int *in1, int *out0, int *out1){
#pragma HLS interface m_axi port=a bundle=gmem0
#pragma HLS interface m_axi port=b bundle=gmem1

#pragma HLS dataflow
	node_cache node_lut(a);
	cache_wrapper(init_a<node_cache>, a);

#pragma HLS dataflow
	region_cache region_lut(b);
	cache_wrapper(init_b<region_cache>, b);
}
*/

/*
void decode(syndr_t syndrome, int* corrections, int* weights){

};

void f_dispatcher(flood_event_t* event, flood_event_t* out_event){
	switch(event->type){
		case NODE:
			// do_look_at_node;
		case REGION_SHR:
			// do_region_shrinking;
		default:
			// no_event;
	}
	out_event = event;
}

void f_do_look_at_node(flood_event_t event, node_data_t node_data1, node_data_t node_data2, int relative_idx){
	// call to find_next_event
}
void f_find_next_event(node_t node, node_data_t node_data1, node_data_t node_data2, int relative_idx){
	// ACCESS TO NODE_LUT (READ ONLY) ACCESS TO REGION_LUT
}

void f_do_RhB_interaction(node_data_t node_data, mwpm_event_t mwpm_event){}

void f_do_N_interaction(node_data_t data1, node_data_t data2, int relative_idx, region_t region_arrived_top, mwpm_event_t mpwm_event){};

void f_do_region_arriving_at_empty_detector_node(region_t region_top, node_data_t data1, node_data_t data2, int relative_idx, node_t out_node){
	//TODO: ACCESS TO REGION_LUT (RW mode)
}

void f_reschedule_events_at_detector_node(node_t node, time_t time, flood_event_t out_event){} //Event type is Node in this case

void f_do_region_shrinking(flood_event_t event, region_data_t region_data){}

//all the three following access to Node LUT and Region LUT
void f_do_blossom_shattering(region_data_t region_data, mwpm_event_t mpwm_event){}

void f_do_generate_implosion(region_data_t region_data, mwpm_event_t mpwm_event){}

void f_do_leave_node(region_data_t region_data, node_data_t node_data, region_data_shrink_t region_shrink_data){}

void f_schedule_tentative_shrink_event(region_data_shrink_t region_shrink_data, flood_event_t event){} //Event type is Region in this case

//Access to Region LUT (RW)
void f_set_region_frozen(region_t region_idx, node_t node_idx){}

void f_set_region_growing(region_t region_idx, flood_event_t* out_events){}

void f_set_region_shrinking(region_t region_idx, region_shrink_data_t region_shrink_data){}

void read_syndrome(int syndrome[N_DEC_NODES], hls::stream<int>&syndr_stream){
	read: for(int i=0; i<N_DEC_NODES; i++){
#pragma HLS  PIPELINE II=1
		int tmp = syndrome[i];
		syndr_stream.write(tmp);
	}
}

void compute_s(hls::stream<int>&syndr_stream, hls:stream<int>&corr_stream){
	static int corr_internal[N_OBS]={0};
	// TODO
}

void write_corrections(hls::stream<int>&corr_stream, int corrections[N_OBS]){
	write: for(int i =0; i<N_OBS; i++){
#pragma HLS  PIPELINE II=1
		int tmp = corr_stream.read();
		corrections[i] = tmp;
	}
}

void f_decode(int syndrome[N_DEC_NODES], int corrections[N_OBS]){
	//#pragma HLS INTERFACE ...

	hls::stream<int>syndr_stream;
	hls::stream<int>corr_stream;

#pragma HLS dataflow
	read_syndrome(syndrome, syndr_stream);
	compute_s(syndr_stream, corr_stream);
	write_corrections(corr_stream, corrections);
}

*/

#include "kernel.hpp"

void compute_corr(int syndrome[N], int corrections[N]){

	static int corr_internal[N] = { 0 };

	compute: for(int i=0; i<N; i++){
#pragma HLS PIPELINE II=1
		corr_internal[i] = syndrome[i] + 1;
	}

	write_and_reset: for(int i=0; i<N; i++){
#pragma HLS PIPELINE II=1;
		corrections[i] = corr_internal[i];
	}
	return;
}

void top(int *nodes, int *regions, int *alt_tree, syndr_t syndrome[MAX_N_NODES], corr_t corrections[MAX_N_OBS]){
#pragma HLS interface m_axi port=nodes bundle=gmem0
#pragma HLS interface m_axi port=regions bundle=gmem0
#pragma HLS interface m_axi port=alt_tree bundle=gmem0

#pragma HLS dataflow
	node_cache node_lut(nodes);
	region_cache region_lut(regions);
	alt_tree_cache alt_tree_lut(alt_tree);
	cache_wrapper(compute_corr<node_cache, region_cache, alt_tree_cache>, node_lut, region_lut, alt_tree_lut);
}




