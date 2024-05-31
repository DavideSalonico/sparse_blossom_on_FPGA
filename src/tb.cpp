#include "sparse_fpga.cpp"

int main(int argc, char * argv[]) //change with real data_types
{
    FpgaGraph graph = load_graph_from_file();

    node_cache node_lut;
    region_cache region_lut;
    alt_tree_cache alt_tree_lut;

    sparse_top(0, NULL, NULL, NULL);

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
