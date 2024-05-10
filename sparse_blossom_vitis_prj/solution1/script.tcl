############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2023 Xilinx, Inc. All Rights Reserved.
############################################################
open_project sparse_blossom_vitis_prj
set_top compute_corr
add_files src/kernel.cpp
add_files src/kernel.hpp
add_files src/pqueue.h
add_files -tb src/testbench.cpp
open_solution "solution1" -flow_target vivado
set_part {xcvu11p-flga2577-1-e}
create_clock -period 10 -name default
#source "./sparse_blossom_vitis_prj/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
