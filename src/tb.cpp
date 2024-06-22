#ifndef TB_CPP
#define TB_CPP

#include "/home/users/davide.salonico/sparse_blossom_prj/include/SBA_kernel.hpp"
#include "hls_print.h"
#include "json.hpp"

FpgaGraph graph;

void read_graph_from_file(){
    // code to read the graph from json
    std::ifstream json_file("graph.json");
    if (!json_file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return 1;
    }

    // Parse the JSON file
    json j;
    json_file >> j;

    from_json(j, graph);

    //print to cjeck the values
}

void printGraph(FpgaGraph graph){
    hls::print("Number of Nodes: %d\n", graph.num_nodes);
    hls::print("Number of Observations: %d\n", graph.num_obs);

	hls::print("Nodes:\n");
	for (int i = 0; i < graph.num_nodes; ++i) {
        hls::print("Node %d: Neighbors = ", i);
        hls::print("ID = %d, ", graph.nodes[i].index);
        hls::print("Region_idx = %d, ",  graph.nodes[i].region_idx);
        hls::print("Neighbors: ");
		for (int j = 0; j < N_NEIGH; ++j) {
            hls::print("%d ", graph.nodes[i].neigh[j]);
		}
		hls::print("\n");
	}
}

int main(){
    //populateGraph_byHand(&graph);
    //printGraph(graph);
    read_graph_from_file();

    sparse_top(LOAD_GRAPH, &graph, 0, 0);

    syndr_t syndrome = 500;
    corrections_t corrections = 0;
    sparse_top(DECODE, NULL, syndrome, &corrections);

    hls::print("Corrections: %d\n", corrections);
}

#endif //TB_CPP

/*
void populateGraph_byHand(FpgaGraph *graph) {
    // Initialize number of nodes and observations
    graph->num_nodes = 5; // Example value
    graph->num_obs = 3;   // Example value

    // Populate nodes
    for (int i = 1; i <= graph->num_nodes; ++i) {
        graph->nodes[i].index = i;
        graph->nodes[i].region_idx = i * 10; // Example values
        for (int j = 0; j < N_NEIGH; ++j) {
            graph->nodes[i].neigh[j] = (i + j + 1) % graph->num_nodes; // Example neighbor connections
        }
    }

    // Populate regions:
    for (int i = 0; i < N_REGIONS; ++i) {
        graph->regions[i].index = i;
        //graph->regions[i].radius = i * 2; // Example values
    }

    // Populate altTree
    for (int i = 0; i < ALTTREEEDGE_MAX; ++i) {
        graph->alttree[i].index = i;
        graph->alttree[i].inner_region_idx = 30; // Example values
    }
}
*/