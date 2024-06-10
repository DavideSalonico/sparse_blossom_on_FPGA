#include "sparse_fpga_no_cache.cpp"

int main(int argc, char * argv[]){
    //FpgaGraph graph = load_graph_from_file();
    FpgaGraph graph;

    //populate graph by hand
    // ...
    //cache population
    graph.nodes[1].index = 1;
    graph.nodes[1].region_idx = 1;
    graph.nodes[1].top_region_idx = 1;
    graph.nodes[1].wrapped_radius_cached = 1;
    graph.nodes[1].reached_from_source = 1;
    graph.nodes[1].obs_inter = 1;
    graph.nodes[1].radius_of_arrival = 2;
    graph.nodes[1].neigh[0] = 2;//idx
    graph.nodes[1].neigh[1] = 3;
    graph.nodes[1].neigh[2] = 0;
    graph.nodes[1].neigh[3] = 0;
    graph.nodes[1].neigh_weights[0] = 1;
    graph.nodes[1].neigh_weights[1] = 1;
    graph.nodes[1].neigh_weights[2] = 0;
    graph.nodes[1].neigh_weights[3] = 0;
    graph.nodes[1].neigh_obs[0] = 1;
    graph.nodes[1].neigh_obs[1] = 1;
    graph.nodes[1].neigh_obs[2] = 0;
    graph.nodes[1].neigh_obs[3] = 0;


    graph.nodes[2].index = 2;
    graph.nodes[2].region_idx = 1;
    graph.nodes[2].top_region_idx = 1;
    graph.nodes[2].wrapped_radius_cached = 1;
    graph.nodes[2].reached_from_source = 1;
    graph.nodes[2].obs_inter = 1;
    graph.nodes[2].radius_of_arrival = 2;
    graph.nodes[2].neigh[0] = 1;//idx
    graph.nodes[2].neigh[1] = 4;
    graph.nodes[2].neigh[2] = 5;
    graph.nodes[2].neigh[3] = 0;
    graph.nodes[2].neigh_weights[0] = 1;
    graph.nodes[2].neigh_weights[1] = 1;
    graph.nodes[2].neigh_weights[2] = 1;
    graph.nodes[2].neigh_weights[3] = 0;
    graph.nodes[2].neigh_obs[0] = 1;
    graph.nodes[2].neigh_obs[1] = 1;
    graph.nodes[2].neigh_obs[2] = 1;
    graph.nodes[2].neigh_obs[3] = 0;


    graph.nodes[3].index = 3;
    graph.nodes[3].region_idx = 1;
    graph.nodes[3].top_region_idx = 1;
    graph.nodes[3].wrapped_radius_cached = 1;
    graph.nodes[3].reached_from_source = 1;
    graph.nodes[3].obs_inter = 1;
    graph.nodes[3].radius_of_arrival = 2;
    graph.nodes[3].neigh[0] = 1;//idx
    graph.nodes[3].neigh[1] = 0;
    graph.nodes[3].neigh[2] = 0;
    graph.nodes[3].neigh[3] = 0;
    graph.nodes[3].neigh_weights[0] = 1;
    graph.nodes[3].neigh_weights[1] = 0;
    graph.nodes[3].neigh_weights[2] = 0;
    graph.nodes[3].neigh_weights[3] = 0;
    graph.nodes[3].neigh_obs[0] = 1;
    graph.nodes[3].neigh_obs[1] = 0;
    graph.nodes[3].neigh_obs[2] = 0;
    graph.nodes[3].neigh_obs[3] = 0;


    graph.nodes[4].index = 4;
    graph.nodes[4].region_idx = 2;
    graph.nodes[4].top_region_idx = 2;
    graph.nodes[4].wrapped_radius_cached = 1;
    graph.nodes[4].reached_from_source = 1;
    graph.nodes[4].obs_inter = 1;
    graph.nodes[4].radius_of_arrival = 3;
    graph.nodes[4].neigh[0] = 2;//idx
    graph.nodes[4].neigh[1] = 0;
    graph.nodes[4].neigh[2] = 0;
    graph.nodes[4].neigh[3] = 0;
    graph.nodes[4].neigh_weights[0] = 1;
    graph.nodes[4].neigh_weights[1] = 0;
    graph.nodes[4].neigh_weights[2] = 0;
    graph.nodes[4].neigh_weights[3] = 0;
    graph.nodes[4].neigh_obs[0] = 1;
    graph.nodes[4].neigh_obs[1] = 0;
    graph.nodes[4].neigh_obs[2] = 0;
    graph.nodes[4].neigh_obs[3] = 0;


    graph.nodes[5].index = 5;
    graph.nodes[5].region_idx = 2;
    graph.nodes[5].top_region_idx = 2;
    graph.nodes[5].wrapped_radius_cached = 1;
    graph.nodes[5].reached_from_source = 1;
    graph.nodes[5].obs_inter = 1;
    graph.nodes[5].radius_of_arrival = 2;
    graph.nodes[5].neigh[0] = 2;//idx
    graph.nodes[5].neigh[1] = 0;
    graph.nodes[5].neigh[2] = 0;
    graph.nodes[5].neigh[3] = 0;
    graph.nodes[5].neigh_weights[0] = 1;
    graph.nodes[5].neigh_weights[1] = 0;
    graph.nodes[5].neigh_weights[2] = 0;
    graph.nodes[5].neigh_weights[3] = 0;
    graph.nodes[5].neigh_obs[0] = 1;
    graph.nodes[5].neigh_obs[1] = 0;
    graph.nodes[5].neigh_obs[2] = 0;
    graph.nodes[5].neigh_obs[3] = 0;


    graph.regions[1].index = 1;
    graph.regions[1].blossom_parent_region_idx = 1;
    graph.regions[1].blossom_parent_top_region_idx = 1;
    graph.regions[1].alt_tree_node = 1; //idx
    graph.regions[1].radius.value = 3;
    graph.regions[1].radius.status = GROWING;
    graph.regions[1].match.region = 0;
    graph.regions[1].match.edge.src = 0;
    graph.regions[1].match.edge.dest = 0;
    graph.regions[1].match.edge.obs_mask = 0;
    graph.regions[1].shell_area[0] = 0; //idx node_t
    graph.regions[1].shell_area[1] = 0;
    graph.regions[1].shell_area[2] = 0;
    graph.regions[1].shell_area[3] = 0;
    graph.regions[1].blossom_children[0].region = 0;
    graph.regions[1].blossom_children[0].ce.src = 0;
    graph.regions[1].blossom_children[0].ce.dest = 0;
    graph.regions[1].blossom_children[0].ce.obs_mask = 0;
    graph.regions[1].blossom_children[1].region = 0;
    graph.regions[1].blossom_children[1].ce.src = 0;
    graph.regions[1].blossom_children[1].ce.dest = 0;
    graph.regions[1].blossom_children[1].ce.obs_mask = 0;
    graph.regions[1].blossom_children[2].region = 0;
    graph.regions[1].blossom_children[2].ce.src = 0;
    graph.regions[1].blossom_children[2].ce.dest = 0;
    graph.regions[1].blossom_children[2].ce.obs_mask = 0;
    graph.regions[1].blossom_children[3].region = 0;
    graph.regions[1].blossom_children[3].ce.src = 0;
    graph.regions[1].blossom_children[3].ce.dest = 0;
    graph.regions[1].blossom_children[3].ce.obs_mask = 0;


    graph.regions[2].index = 2;
    graph.regions[2].blossom_parent_region_idx = 1;
    graph.regions[2].blossom_parent_top_region_idx = 1;
    graph.regions[2].alt_tree_node = 1; //idx
    graph.regions[2].radius.value = 3;
    graph.regions[2].radius.status = GROWING;
    graph.regions[2].match.region = 0;
    graph.regions[2].match.edge.src = 0;
    graph.regions[2].match.edge.dest = 0;
    graph.regions[2].match.edge.obs_mask = 0;
    graph.regions[2].shell_area[0] = 0; //idx node_t
    graph.regions[2].shell_area[1] = 0;
    graph.regions[2].shell_area[2] = 0;
    graph.regions[2].shell_area[3] = 0;
    graph.regions[2].blossom_children[0].region = 0;
    graph.regions[2].blossom_children[0].ce.src = 0;
    graph.regions[2].blossom_children[0].ce.dest = 0;
    graph.regions[2].blossom_children[0].ce.obs_mask = 0;
    graph.regions[2].blossom_children[1].region = 0;
    graph.regions[2].blossom_children[1].ce.src = 0;
    graph.regions[2].blossom_children[1].ce.dest = 0;
    graph.regions[2].blossom_children[1].ce.obs_mask = 0;
    graph.regions[2].blossom_children[2].region = 0;
    graph.regions[2].blossom_children[2].ce.src = 0;
    graph.regions[2].blossom_children[2].ce.dest = 0;
    graph.regions[2].blossom_children[2].ce.obs_mask = 0;
    graph.regions[2].blossom_children[3].region = 0;
    graph.regions[2].blossom_children[3].ce.src = 0;
    graph.regions[2].blossom_children[3].ce.dest = 0;
    graph.regions[2].blossom_children[3].ce.obs_mask = 0;


    graph.alttree[1].index = 1;
    graph.alttree[1].inner_region_idx = 0;
    graph.alttree[1].outer_region_idx = 0;
    graph.alttree[1].inner_to_outer_edge.src = 0;
    graph.alttree[1].inner_to_outer_edge.dest = 0;
    graph.alttree[1].inner_to_outer_edge.obs_mask = 0;
    graph.alttree[1].parent.alt_tree_node = 0;
    graph.alttree[1].parent.edge.src = 0;
    graph.alttree[1].parent.edge.dest = 0;
    graph.alttree[1].parent.edge.obs_mask = 0;
    graph.alttree[1].children[0].alt_tree_node = 2;
    graph.alttree[1].children[0].edge.src = 1;
    graph.alttree[1].children[0].edge.dest = 2;
    graph.alttree[1].children[0].edge.obs_mask = 0;
    graph.alttree[1].children[1].alt_tree_node = 0;
    graph.alttree[1].children[1].edge.src = 0;
    graph.alttree[1].children[1].edge.dest = 0;
    graph.alttree[1].children[1].edge.obs_mask = 0;
    graph.alttree[1].visited = false;
    graph.alttree[1].state = 0;


    graph.alttree[2].index = 2;
    graph.alttree[2].inner_region_idx = 0;
    graph.alttree[2].outer_region_idx = 0;
    graph.alttree[2].inner_to_outer_edge.src = 0;
    graph.alttree[2].inner_to_outer_edge.dest = 0;
    graph.alttree[2].inner_to_outer_edge.obs_mask = 0;
    graph.alttree[2].parent.alt_tree_node = 1;
    graph.alttree[2].parent.edge.src = 1;
    graph.alttree[2].parent.edge.dest = 2;
    graph.alttree[2].parent.edge.obs_mask = 0;
    graph.alttree[2].children[0].alt_tree_node = 0;
    graph.alttree[2].children[0].edge.src = 0;
    graph.alttree[2].children[0].edge.dest = 0;
    graph.alttree[2].children[0].edge.obs_mask = 0;
    graph.alttree[2].children[1].alt_tree_node = 0;
    graph.alttree[2].children[1].edge.src = 0;
    graph.alttree[2].children[1].edge.dest = 0;
    graph.alttree[2].children[1].edge.obs_mask = 0;
    graph.alttree[2].visited = false;
    graph.alttree[2].state = 0;
    
    syndr_t syndrome = 111;
    corrections_t corrections = 1456;

    sparse_top(0, graph, syndrome, corrections);
}
