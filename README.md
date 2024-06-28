# Sparse Blossom on FPGA - HPPS Project @ Polimi
High Performance Processors and Systems course project at Politecnico di Milano

## Abstract
Quantum Computing is a new paradigm of computation that allows for an exponential speedup with respect to classical computing, over a noticeable number of scenarios. Unfortunately, current quantum hardware is affected by noise, which reduces performance. In this work, we aim to accelerate one of the state-of-the-art techniques for error correction: the Minimum-Weight Perfect Matching. We target FPGAs as a heterogeneous architecture that exploits task-level and data-level parallelism to reduce execution time.

## File structure
The repository is structured as follows:
- **src** folder contains all the source files used in the project. Names are self-explanatory and code for the kernel and the host is provided. It also contains additional utility header files.
- **delieveries** contains the documentation incuding a report and slides that will be used in the oral presentation
- **utils** contains a python script for generating a nodes.json file (there is also one provided as an example)

## A note about the util folder
The util folder contains a python script that generates a JSON file that contains the data of the graph provided as input.
In order to create a new graph it's sufficient to change the following parameters in the script:
- family of error correction codes
- distance
- rounds
- after_clifford_depolarization (that represents the error probability)

To run the script it's necessary to have installed all the depencencies (numpy, stim, pymatching, json).
Plese pay attention to specify the path where you want the json file to be written.
For a detailed documentation of stim circuits we indicate [Stim API Reference](https://github.com/quantumlib/Stim/blob/main/doc/python_api_reference_vDev.md).

## Full Report
The full report is available [here](./path_to_report).
