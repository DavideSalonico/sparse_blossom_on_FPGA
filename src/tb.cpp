#ifndef TB_CPP
#define TB_CPP

#include "/home/users/davide.salonico/sparse_blossom_prj/include/SBA_kernel.hpp"
#include "hls_print.h"
#include "/home/dado/sparse_blossom/sparse_blossom_on_FPGA/include/json.hpp"

FpgaGraph graph;
void from_json(const json& j, FpgaGraph& graph) {
    graph.num_obs = j.at("n_dets").get<int>();
    graph.num_nodes = j.at("n_nodes").get<int>();
    
    int node_count = 0;
    for (const auto& node : j.at("nodes")) {
        node_data_t n;
        n.index = node.at("index").get<node_idx_t>();
        n.region_idx = node.at("region_idx").get<region_idx_t>();
        n.top_region_idx = -1; // Assuming default value
        n.wrapped_radius_cached = -1; // Assuming default value
        n.reached_from_source = -1; // Assuming default value
        n.obs_inter = 0; // Assuming default value
        n.radius_of_arrival = -1; // Assuming default value
        
        std::fill(std::begin(n.neigh), std::end(n.neigh), 0);
        std::fill(std::begin(n.neigh_weights), std::end(n.neigh_weights), 0);
        std::fill(std::begin(n.neigh_obs), std::end(n.neigh_obs), 0);
        
        for (size_t i = 0; i < node.at("neighbors").size() && i < 4; ++i) {
            n.neigh[i] = node.at("neighbors")[i].get<node_idx_t>();
        }
        for (size_t i = 0; i < node.at("neigh_weights").size() && i < 4; ++i) {
            n.neigh_weights[i] = node.at("neigh_weights")[i].get<int>();
        }
        for (size_t i = 0; i < node.at("neigh_obs").size() && i < 4; ++i) {
            n.neigh_obs[i] = node.at("neigh_obs")[i].get<obs_int_t>();
        }
        
        graph.nodes[node_count++] = n;
        if (node_count >= MAX_N_NODES) break;
    }
}

void read_graph_from_file(){
    // Open the JSON file
    std::ifstream json_file("nodes.json");
    if (!json_file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return 1;
    }

    // Parse the JSON file
    json j;
    json_file >> j;

    // Create an FpgaGraph instance and populate it using the parsed JSON
    from_json(j);

    // Output the parsed data
    std::cout << "Number of detectors: " << graph.num_obs << std::endl;
    std::cout << "Number of nodes: " << graph.num_nodes << std::endl;
    std::cout << "Nodes:" << std::endl;
    for (int i = 0; i < graph.num_nodes; ++i) {
        const auto& node = graph.nodes[i];
        std::cout << "  Node index: " << node.index << ", Region index: " << node.region_idx << std::endl;
        std::cout << "  Neighbors: ";
        for (int j = 0; j < 4; ++j) {
            if (node.neigh[j] != 0) {
                std::cout << node.neigh[j] << " ";
            }
        }
        std::cout << std::endl;
        std::cout << "  Weights: ";
        for (int j = 0; j < 4; ++j) {
            std::cout << node.neigh_weights[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "  Observables: ";
        for (int j = 0; j < 4; ++j) {
            std::cout << node.neigh_obs[j] << " ";
        }
        std::cout << std::endl;
    }
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
    
    read_graph_from_file();
    //printGraph(graph);

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