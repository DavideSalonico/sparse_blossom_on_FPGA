#ifndef TB_CPP
#define TB_CPP

#include "SBA_kernel.hpp"
#include "hls_print.h"
#include "json.hpp"

using json = nlohmann::json;

FpgaGraph graph;
void from_json(const json& j) {
    graph.num_obs = j.at("n_dets").get<int>();
    graph.num_nodes = j.at("n_nodes").get<int>();
    
    int node_count = 0;
    for (const auto& node : j.at("nodes")) {
        node_data_t n;
        n.index = (node_idx_t) node.at("index").get<int>();
        
        std::fill(std::begin(n.neigh), std::end(n.neigh), 0);
        std::fill(std::begin(n.neigh_weights), std::end(n.neigh_weights), 0);
        std::fill(std::begin(n.neigh_obs), std::end(n.neigh_obs), 0);
        
        for (size_t i = 0; i < N_NEIGH; ++i) {
            n.neigh[i] = i < node.at("neighbors").size() ? (node_idx_t) node.at("neighbors")[i].get<int>() : (node_idx_t)0;
        }
        for (size_t i = 0; i < N_NEIGH; ++i) {
            n.neigh_weights[i] = i < node.at("neigh_weights").size() ? node.at("neigh_weights")[i].get<float>() : (float) 0;
        }
        for (size_t i = 0; i < N_NEIGH; ++i) {
            n.neigh_obs[i] = i < node.at("neigh_obs").size() ? (obs_mask_t) node.at("neigh_obs")[i].get<int>() : (obs_mask_t) 0;
        }
        
        graph.nodes[node_count++] = n;
        if (node_count >= MAX_N_NODES) break;
    }
}

void read_graph_from_file(){
    // Open the JSON file
    std::ifstream json_file("/home/users/davide.salonico/nodes.json");
    if (!json_file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return;
    }

    // Parse the JSON file
    json j;
    json_file >> j;

    // Create an FpgaGraph instance and populate it using the parsed JSON
    from_json(j);
}

void printGraph(FpgaGraph graph){
    #ifndef __SYNTHESYS__
   // Output the parsed data
    std::cout << "Number of detectors: " << graph.num_obs << std::endl;
    std::cout << "Number of nodes: " << graph.num_nodes << std::endl;
    std::cout << "Nodes:" << std::endl;
    for (int i = 0; i < graph.num_nodes; ++i) {
        const auto& node = graph.nodes[i];
        std::cout << "  Node index: " << node.index << ", Region index: " << node.region_idx << std::endl;
        std::cout << "  Neighbors: ";
        for (int j = 0; j < N_NEIGH; ++j) {
            if (node.neigh[j] != 0) {
                std::cout << node.neigh[j] << " ";
            }
        }
        std::cout << std::endl;
        std::cout << "  Weights: ";
        for (int j = 0; j < N_NEIGH; ++j) {
            if(node.neigh[j] != 0){
                std::cout << node.neigh_weights[j] << " ";
            }
        }
        std::cout << std::endl;
        std::cout << "  Observables: ";
        for (int j = 0; j < N_NEIGH; ++j) {
            if(node.neigh[j] != 0){
                std::cout << node.neigh_obs[j] << " ";
            }
        }
        std::cout << std::endl;
    }
    #endif //__SYTHENSYS__
}

int main(){
    
    read_graph_from_file();
    printGraph(graph);

    sparse_top(LOAD_GRAPH, &graph, 0, 0);

    syndr_t syndrome = 500;
    corrections_t corrections = 0;
    sparse_top(DECODE, NULL, syndrome, &corrections);

    //hls::print("Corrections: %b\n", corrections);
}

#endif //TB_CPP
