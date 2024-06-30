#include "SBA_kernel.hpp"
#include "hls_print.h"
#include "json.hpp"

#include <chrono>

using json = nlohmann::json;

int num_nodes;
int num_obs;
node_data_t nodes[MAX_N_NODES];

void from_json(const json &j)
{
    num_obs = j.at("n_dets").get<int>();
    num_nodes = j.at("n_nodes").get<int>();

    int node_count = 0;
    for (const auto &node : j.at("nodes"))
    {
        node_data_t n;
        n.index = (node_idx_t)node.at("index").get<int>();

        std::fill(std::begin(n.neigh), std::end(n.neigh), 0);
        std::fill(std::begin(n.neigh_weights), std::end(n.neigh_weights), 0);
        std::fill(std::begin(n.neigh_obs), std::end(n.neigh_obs), 0);

        for (size_t i = 0; i < N_NEIGH; ++i)
        {
            n.neigh[i] = i < node.at("neighbors").size() ? (node_idx_t)node.at("neighbors")[i].get<int>() : (node_idx_t)0;
        }
        for (size_t i = 0; i < N_NEIGH; ++i)
        {
            n.neigh_weights[i] = i < node.at("neigh_weights").size() ? node.at("neigh_weights")[i].get<float>() : 0;
        }
        for (size_t i = 0; i < N_NEIGH; ++i)
        {
            n.neigh_obs[i] = i < node.at("neigh_obs").size() ? (obs_mask_t)node.at("neigh_obs")[i].get<int>() : (obs_mask_t)0;
        }

        nodes[node_count++] = n;
        if (node_count >= MAX_N_NODES)
            break;
    }
}

void read_graph_from_file()
{
    // Path where json file containing informations about nodes is stored
    auto path = "/home/users/davide.salonico/nodes.json";

        // Open the JSON file
        std::ifstream json_file(path);
    if (!json_file.is_open())
    {
        std::cerr << "Could not open the file!" << std::endl;
        return;
    }

    // Parse the JSON file
    json j;
    json_file >> j;

    // Create nodes list and populate it using the parsed JSON
    from_json(j);
}

void printGraph()
{
#ifndef __SYNTHESYS__

    // Output the parsed data
    std::cout << "Number of detectors: " << num_obs << std::endl;
    std::cout << "Number of nodes: " << num_nodes << std::endl;
    std::cout << "Nodes:" << std::endl;
    for (int i = 0; i < num_nodes; ++i)
    {
        const auto &node = nodes[i];
        std::cout << "  Node index: " << node.index << ", Region index: " << node.region_idx << std::endl;
        std::cout << "  Neighbors: ";
        for (int j = 0; j < N_NEIGH; ++j)
        {
            if (node.neigh[j] != 0)
            {
                std::cout << node.neigh[j] << " ";
            }
        }
        std::cout << std::endl;
        std::cout << "  Weights: ";
        for (int j = 0; j < N_NEIGH; ++j)
        {
            if (node.neigh[j] != 0)
            {
                std::cout << node.neigh_weights[j] << " ";
            }
        }
        std::cout << std::endl;
        std::cout << "  Observables: ";
        for (int j = 0; j < N_NEIGH; ++j)
        {
            if (node.neigh[j] != 0)
            {
                std::cout << node.neigh_obs[j] << " ";
            }
        }
        std::cout << std::endl;
    }
#endif //__SYTHENSYS__
}

int main()
{

    // Take start time of the algorithm
    auto start_time = std::chrono::system_clock::now();
    auto nano_start_time = std::chrono::duration_cast<std::chrono::nanoseconds>(start_time.time_since_epoch()).count();
    std::cout << "Start time (nanoseconds): " << nano_start_time << std::endl;

    read_graph_from_file();
    printGraph();

    syndr_t syndrome = 501;
    corrections_t corrections = 0;
    sparse_top(nodes, syndrome, &corrections);

    // Take end time of the algorithm
    auto end_time = std::chrono::system_clock::now();
    auto nano_end_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time.time_since_epoch()).count();

    // Just a trivial verification to check whether computions have been changed or not
    // Refer to the paper for the discussion about this
    if (corrections == syndrome % 2)
    {
        std::cout << "End time (nanoseconds): " << nano_end_time << std::endl;
        // Compute time difference
        std::cout << "Total execution time: " << nano_end_time - nano_start_time << " nanoseconds" << std::endl;
        std::cout << "Test passed !" << std::endl;
    }
    else
    {
        std::cout << "Test failed  !!!" << std::endl;
    }
}
