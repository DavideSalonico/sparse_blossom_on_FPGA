#include "flooder_no_cache.h"


void trackerDequeue(flood_event_t * fe){
    fe->node = 100;
    fe->time = 100;
    fe->type = NODE;
}


void decode(FpgaGraph* graph, syndr_t syndrome, corrections_t corrections){
   //PriorityQueue queue;

   // Read syndrome and create right events

	/*
   bool done;
   while(!done){
       flood_event_t fe = NULL;
       trackerDequeue(&fe);
       f_dispatcher(&fe, done);
       if(done) break;
   }*/
	flood_event_t fe;
	fe.node = 2;
	fe.time = 10;
	fe.type = NODE;
	f_dispatcher(fe, fe, graph);

   //Compute errors from alt_tree
   //errors = ...
}

extern "C" void sparse_top(choice_t choice, FpgaGraph* graph, syndr_t syndrome, corrections_t corrections)
{
#pragma HLS INTERFACE m_axi port = a_arr offset = slave bundle = gmem0 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = b_arr offset = slave bundle = gmem1 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = c_arr offset = slave bundle = gmem2 latency = 0 depth = 1024
#pragma HLS INTERFACE ap_ctrl_hs port = return

    decode(graph, syndrome, corrections);
}



