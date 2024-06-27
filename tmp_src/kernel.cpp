#include "flooder.hpp"
#include "SBA_kernel.hpp"


void trackerDequeue(flood_event_t * fe){
    fe->node = 100;
    fe->time = 100;
    fe->type = NODE;
}


void decode(FpgaGraph* graph, syndr_t syndrome, corrections_t *corrections){
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

extern "C" void sparse_top(FpgaGraph* graph, syndr_t syndrome, corrections_t *corrections)
{
#pragma HLS INTERFACE m_axi port = graph offset = slave bundle = gmem0 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = corrections offset = slave bundle = gmem1 latency = 0 depth = 1024

#pragma HLS INTERFACE s_axilite port=graph bundle=control
#pragma HLS INTERFACE s_axilite port=syndrome bundle=control
#pragma HLS INTERFACE s_axilite port=corrections bundle=control

#pragma HLS INTERFACE s_axilite port = return bundle = control


    decode(graph, syndrome, corrections);
}
