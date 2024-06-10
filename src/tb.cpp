#include "sparse_fpga.cpp"

void populateGraph(FpgaGraph *graph) {
    // Initialize number of nodes and observations
    graph->num_nodes = 5; // Example value
    graph->num_obs = 3;   // Example value

    // Populate nodes
    for (int i = 0; i < graph->num_nodes; ++i) {
        graph->nodes[i].id = i;
        graph->nodes[i].weight = i * 10; // Example values
        for (int j = 0; j < N_NEIGH; ++j) {
            graph->nodes[i].neighbors[j] = (i + j + 1) % graph->num_nodes; // Example neighbor connections
        }
    }

    // Populate regions
    for (int i = 0; i < N_REGIONS; ++i) {
        graph->regions[i].id = i;
        graph->regions[i].area = i * 2.5f; // Example values
    }

    // Populate altTree
    for (int i = 0; i < ALTTREEEDGE_MAX; ++i) {
        graph->alttree[i].id = i;
        graph->alttree[i].parent_id = (i == 0) ? -1 : (i - 1); // Example values
    }
}

void printGraph(FpgaGraph graph){
	std::cout << "Number of Nodes: " << graph.num_nodes << std::endl;
	std::cout << "Number of Observations: " << graph.num_obs << std::endl;

	std::cout << "Nodes:" << std::endl;
	for (int i = 0; i < graph.num_nodes; ++i) {
		std::cout << "Node " << i << ": ID = " << graph.nodes[i].index
				  << ", Weight = " << graph.nodes[i].wrapped_radius_cached << ", Neighbors = ";
		for (int j = 0; j < N_NEIGH; ++j) {
			std::cout << graph.nodes[i].neighbors[j] << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "Regions:" << std::endl;
	for (int i = 0; i < N_REGIONS; ++i) {
		std::cout << "Region " << i << ": ID = " << graph.regions[i].id
				  << ", Area = " << graph.regions[i].area << std::endl;
	}

	std::cout << "AltTree:" << std::endl;
	for (int i = 0; i < ALTTREEEDGE_MAX; ++i) {
		std::cout << "AltTreeNode " << i << ": ID = " << graph.alttree[i].id
				  << ", Parent ID = " << graph.alttree[i].parent_id << std::endl;
	}
}

int main(int argc, char * argv[]){

	FpgaGraph graph;
    populateGraph(&graph);
    printGraph(graph);

    sparse_top(LOAD_GRAPH, &graph, 0, 0);

    syndrome_t syndrome = 500;
    err_t errors = 0;
    sparse_top(DECODE, null, syndrome, errors);

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
}
