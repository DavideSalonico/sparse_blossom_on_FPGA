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

#include "cmdlineparser.h"
#include <iostream>
#include <cstring>

#include "kernel.hpp"

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

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

    size_t size_in_bytes_in = sizeof(FpgaGraph);
    size_t size_in_bytes_out = sizeof(corrections_t);

    auto krnl = xrt::kernel(device, uuid, "dummy_kernel");

    std::cout << "Allocate Buffer in Global Memory\n";
    auto input_buffer = xrt::bo(device, size_in_bytes_in, krnl.group_id(0));
    auto output_buffer = xrt::bo(device, size_in_bytes_out, krnl.group_id(1));

    // Prepare the input data
    FpgaGraph buffer_in_data;
    for (int i = 1; i < 4; i++){
        node_data_t node;
        node.index = i;
        node.reached_from_source = 10 * i;
        node.region_idx = 1;
        buffer_in_data.nodes[i] = node;
    }

    corrections_t buffer_out_data;

    std::cout << "Write the input data\n";
    input_buffer.write(buff_in_data);

    std::cout << "synchronize input buffer data to device global memory\n";
    input_buffer.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    // First we need to load the graph
    choice_t choice = LOAD_GRAPH; //it means the input_buffer contains the graph
    auto run = krnl(choice, input_buffer, output_buffer);
    run.wait();

    std::cout << "Synchronize the output buffer data from the device" << std::endl;
    output_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    std::cout << "Read the output data\n";
    output_buffer.read(buff_out_data);

    //Prepare expected output
    corrections_t golden_out_data;
    for (int i= 0; i < MAX_N_OBS; i++){
        golden_out_data ^= 0b1 << i;
    }

    // Validate our results
    if (std::memcmp(golden_out_data, buff_out_data, sizeof(corrections_t)))
        throw std::runtime_error("Value read back does not match reference");

    std::cout << "TEST PASSED\n";
    return 0;
}
