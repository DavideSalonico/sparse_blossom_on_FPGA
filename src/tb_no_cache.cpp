#include "sparse_fpga_no_cache.cpp"

int main(int argc, char * argv[]){
    //FpgaGraph graph = load_graph_from_file();
    FpgaGraph graph;

    //populate graph by hand
    // ...
    
    syndr_t syndrome = 111;
    corrections_t corrections = 1456;

    sparse_top(0, graph, syndrome, corrections);
}
