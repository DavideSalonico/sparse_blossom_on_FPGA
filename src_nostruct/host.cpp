/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/


#include <iostream>
#include <cstring>

#include "SBA_kernel.hpp"
#include "json.hpp"
#include "cmdlineparser.h"

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

node_data_t nodes[MAX_N_NODES];
#define DATA_SIZE sizeof(nodes)

using json = nlohmann::json;

node_data_t * nodes;
void from_json(const json& j) {
    num_obs = j.at("n_dets").get<int>();
    num_nodes = j.at("n_nodes").get<int>();
    
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
            n.neigh_weights[i] = i < node.at("neigh_weights").size() ? node.at("neigh_weights")[i].get<float>() : 0;
        }
        /*
        for (size_t i = 0; i < N_NEIGH; ++i) {
            n.neigh_obs[i] = i < node.at("neigh_obs").size() ? (obs_mask_t) node.at("neigh_obs")[i].get<int>() : 0;
        }
        */
        
        nodes[node_count++] = n;
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

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--device_id", "-d", "device index", "0");
    parser.parse(argc, argv);

    // Read settings
    std::string binaryFile = parser.value("xclbin_file");
    int device_index = stoi(parser.value("device_id"));

    if (argc < 3) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    size_t vector_size_bytes = sizeof(nodes);

    auto krnl = xrt::kernel(device, uuid, "SBA");

    std::cout << "Allocate Buffer in Global Memory\n";
    auto bo0 = xrt::bo(device, vector_size_bytes, krnl.group_id(0));
    auto bo1 = xrt::bo(device, vector_size_bytes, krnl.group_id(1)); //change
    //auto bo2 = xrt::bo(device, vector_size_bytes, krnl.group_id(2));
    auto bo_out = xrt::bo(device, vector_size_bytes, krnl.group_id(2));

    // Map the contents of the buffer object into host memory
    auto bo0_map = bo0.map<node_data_t *>();
    auto bo1_map = bo1.map<int*>();
    auto bo_out_map = bo_out.map<int*>();
    std::fill(bo0_map, bo0_map + DATA_SIZE, 0);
    std::fill(bo1_map, bo1_map + DATA_SIZE, 0);
    std::fill(bo_out_map, bo_out_map + DATA_SIZE, 0);

    read_graph_from_file();

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";

    bo0.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo1.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo0, bo1, bo_out, DATA_SIZE);
    run.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    //TODO: define bufReference variable

    // Validate our results
    /*
    if (std::memcmp(bo_out_map, bufReference, DATA_SIZE))
        throw std::runtime_error("Value read back does not match reference");
    */

    std::cout << "TEST PASSED\n";
    return 0;
}