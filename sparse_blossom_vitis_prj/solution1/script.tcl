############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2023 Xilinx, Inc. All Rights Reserved.
############################################################
open_project sparse_blossom_vitis_prj
add_files flooder.cpp
add_files flooder.h
open_solution "solution1" -flow_target vivado
set_part {xcvu11p-flga2577-1-e}
create_clock -period 3 -name default
#source "./sparse_blossom_vitis_prj/solution1/directives.tcl"
#csim_design
csynth_design
#cosim_design
export_design -format ip_catalog
