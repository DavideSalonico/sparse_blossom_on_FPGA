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

#include "kernel.hpp"

extern "C" {
void dummy_kernel(unsigned int* buffer0, unsigned int* buffer1) {
    FpgaGraph graph;
    buffer0.read(graph);

    bool transfer_ok = true;
dummy:
    for(int i = 1; i < 4; i++){
        if(graph.nodes[i].reached_from_source != 10*i || graph.nodes[i].region_idx != 1)
            transfer_ok = false;
    }

    corrections_t corrections = 0;
    if(transfer_ok){
        for(int i = 0; i < MAX_N_OBS; i++){
            corrections ^= 1 << i;
        }
    }
    corrections.write(buffer1);
}
}