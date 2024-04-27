#include "flooder.h"

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



