#include "sparse_fpga.cpp"

void populateGraph(FpgaGraph *graph) {
    // Initialize number of nodes and observations
    graph->num_nodes = 5; // Example value
    graph->num_obs = 3;   // Example value

    // Populate nodes
    for (int i = 1; i <= graph->num_nodes; ++i) {
        graph->nodes[i].index = i;
        graph->nodes[i].region_idx = i * 10; // Example values
        for (int j = 0; j < N_NEIGH; ++j) {
            graph->nodes[i].neigh[j] = (i + j + 1) % graph->num_nodes; // Example neighbor connections
        }
    }

    // Populate regions:
    for (int i = 0; i < N_REGIONS; ++i) {
        graph->regions[i].index = i;
        //graph->regions[i].radius = i * 2; // Example values
    }

    // Populate altTree
    for (int i = 0; i < ALTTREEEDGE_MAX; ++i) {
        graph->alttree[i].index = i;
        graph->alttree[i].inner_region_idx = 30; // Example values
    }
}

/*
void printGraph(FpgaGraph graph){
	std::cout << "Number of Nodes: " << graph.num_nodes << std::endl;
	std::cout << "Number of Observations: " << graph.num_obs << std::endl;

	std::cout << "Nodes:" << std::endl;
	for (int i = 0; i < graph.num_nodes; ++i) {
		std::cout << "Node " << i << ": ID = " << graph.nodes[i].index
				  << ", Region_idx = " << graph.nodes[i].region_idx << ", Neighbors = ";
		for (int j = 0; j < N_NEIGH; ++j) {
			std::cout << graph.nodes[i].neigh[j] << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "Regions:" << std::endl;
	for (int i = 0; i < N_REGIONS; ++i) {
		std::cout << "Region " << i << ": ID = " << graph.regions[i].index
				  // << ", Area = " << graph.regions[i].radius 
                  << std::endl;
	}

	std::cout << "AltTree:" << std::endl;
	for (int i = 0; i < ALTTREEEDGE_MAX; ++i) {
		std::cout << "AltTreeNode " << i << ": ID = " << graph.alttree[i].index
				  << ", Inner_region_idx = " << graph.alttree[i].inner_region_idx << std::endl;
	}
}
*/

int main(int argc, char * argv[]){

	FpgaGraph graph;
    populateGraph(&graph);
    //printGraph(graph);

    sparse_top(LOAD_GRAPH, &graph, 0, 0);

    syndr_t syndrome = 500;
    corrections_t corrections = 0;
    sparse_top(DECODE, NULL, syndrome, &corrections); 
}

/*
    data_type a_arr[N * M];
    data_type b_arr[M * P];
    data_type c_arr[N * P] = {0};
    data_type c_arr_ref[N * P] = {0};

    for (auto i = 0; i < N * M; i++)
        a_arr[i] = i;
    for (auto i = 0; i < M * P; i++)
        b_arr[i] = i;

    // matrix multiplication with caches
    matmult_top(a_arr, b_arr, c_arr);
    // standard matrix multiplication
    multiply(a_arr, b_arr, c_arr_ref);

    int err = 0;
    for (auto i = 0; i < N * P; i++)
    {
        if (c_arr[i] != c_arr_ref[i])
        {
            err++;
            printf("Mismatch: %d %d\n", c_arr[i], c_arr_ref[i]);
        }
    }

    return err;
    */