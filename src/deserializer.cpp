#include "json.hpp"
#include "sparse_fpga.cpp"

#include <iostream>
#include <fstream>
#include <vector>

using json = nlohmann::json;

struct Edge {
    int src;
    int dest;
    float weight;
    int obs_mask;
};

void from_json(const json& j, Edge& edge) {
    j.at("src").get_to(edge.src);
    j.at("dest").get_to(edge.dest);
    j.at("weight").get_to(edge.weight);
    j.at("obs_mask").get_to(edge.obs_mask);
}

void from_json(const json& j, Graph& graph) {
    j.at("n_nodes").get_to(graph.n_nodes);
    j.at("n_dets").get_to(graph.n_dets);
    j.at("n_obs").get_to(graph.n_obs);
    j.at("n_edges").get_to(graph.n_edges);
    j.at("edges").get_to(graph.edges);
}

int main() {
    std::ifstream file("graph.json");
    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return 1;
    }

    json j;
    file >> j;

    FpgaGraph graph = j.get<FpgaGraph>();

    std::cout << "Graph Information:" << std::endl;
    std::cout << "Number of Nodes: " << graph.n_nodes << std::endl;
    std::cout << "Number of Detectors: " << graph.n_dets << std::endl;
    std::cout << "Number of Observables: " << graph.n_obs << std::endl;
    std::cout << "Number of Edges: " << graph.n_edges << std::endl;

    std::cout << "Edges:" << std::endl;
    for (const auto& edge : graph.edges) {
        std::cout << "Source: " << edge.src << ", Destination: " << edge.dest
                  << ", Weight: " << edge.weight << ", Obs Mask: " << edge.obs_mask << std::endl;
    }

    return 0;
}
