
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

//#include "cmdlineparser.h"
#include <cstring>
#include <iostream>
#include <cmath>
#include <random>
#include "kernel.hpp"

// XRT includes
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
/*

int DATA_SIZE = (4 * 1024) / sizeof(uint32_t); //TODO: Change

int main(int argc, char* argv[]) {

    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
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

    // Compute the size of array in bytes
    uint32_t size_in_bytes = DATA_SIZE * sizeof(uint32_t); //TODO: Change

    auto krnl = xrt::kernel(device, uuid, "top");

    xrt::bo::flags flags = xrt::bo::flags::host_only;
    std::cout << "Creating Buffers ...\n";
    auto hostonly_bo0_nodes = xrt::bo(device, size_in_bytes, flags, krnl.group_id(0));
    auto hostonly_bo1_regions = xrt::bo(device, size_in_bytes, flags, krnl.group_id(0));
    auto hostonly_bo_out0_corrections = xrt::bo(device, size_in_bytes, flags, krnl.group_id(0));
    auto hostonly_bo_out1_weights = xrt::bo(device, size_in_bytes, falgs, krnl.group_id(0));


    // Map the contents of the buffer object into host memory
    auto bo0_map = hostonly_bo0.map<int*>();
    auto bo1_map = hostonly_bo1.map<int*>();
    auto bo_out0_map = hostonly_bo_out0.map<int*>();
    auto bo_out1_map = hostony_bo_out1.map<int *>();

    std::fill(bo0_map, bo0_map + DATA_SIZE, 0);
    std::fill(bo1_map, bo1_map + DATA_SIZE, 0);
    std::fill(bo_out0_map, bo_out0_map + DATA_SIZE, 0);
    std::fill(bo_out1_map, bo_out1_map + DATA_SIZE, 0);

    // Create the test data
    for (int i = 0; i < DATA_SIZE; ++i) {
        bo0_map[i] = 1;
        bo1_map[i] = 2;
    }

    std::cout << "Execution of the kernel\n";
    auto run = krnl(hostonly_bo0, hostonly_bo1, hostonly_bo0_out, hostonly_b1_out, DATA_SIZE);
    run.wait();

    // Compare the results of the Device to the simulation
    bool match = true;
    for (int i = 0; i < DATA_SIZE; i++) {
        if (bo_out0_map[i] != 2 || bo_out1_map[i] != 1) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << bo0_map[i] << " " << bo1_map[i]
                      << " Device result = " << bo_out0_map[i] << " " << bo_ou1[i] << std::endl;
            match = false;
            break;
        }
    }
    
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return 0;
}
*/

int main(int argc, char * argv[]){
    int corr_golden[N];
    int corr_hw[N];

    int my_syndrome[N];
    int seed = 1234;
    std::default_random_engine rng(seed);
	std::uniform_int_distribution<int> rng_dist(0, 10);

    //Init random
    for(int i=0; i<N; i++){
        my_syndrome[i] = rng_dist(rng);
    }

    //Compute golden
    for(int i=0; i<N; i++){
        corr_golden[i] = my_syndrome[i] + 1;
    }

    //DUT analysis
    compute_corr(my_syndrome, corr_hw);

    //golden vs dut comparison
	   int errors=0;
	   for(int i=0; i < N; i++){
		   if(corr_golden[i]!=corr_hw[i]){
			   errors++;
			   std::cout << "There is/are " << errors << " error(s)" << std::endl;
		   }
	   }
	   if(errors){
		   return -1;
	   }else{
		   return 0;
	   }
}
