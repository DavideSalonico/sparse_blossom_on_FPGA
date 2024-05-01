void f_find_next_event(node_t node, node_data_t next_neigh_node, int next_best_time){
    // ACCESS TO NODE_LUT (READ ONLY) ACCESS TO REGION_LUT
    
    node_data_t node_data = node_lut[node];
    //NODE_LUT ACCESS: node_data = node -> LUT
    
    region_data_t region_arrived_top = region_lut[node_data.top_region_idx];
    //region_arrived_top = node_data.top_region_idx -> LUT
    
    int rad1 = (node_data.wrapped_radius_cached + region_arrived_top.radius);
    
    if (/*rad1 & (1 << (sizeof(int) * 8 - 1))*/ rad1 & 1) {
        //return find_next_event_at_node_occupied_by_growing_top_region(detector_node, rad1);
        
        int best_time = LLONG_MAX;
        int best_neighbor = N_NEIGH;
        int start = 0;
        int weight;
        //node_data_t neighbor;
        int rad2;
        int collision_time;
        
        if(node_data.neigh[0] == 0 && (node_data.neigh[1] != 0 || node_data.neigh[2] != 0 || node_data.neigh[3] != 0)) {
            weight = node_data.neigh_weights[0];
            collision_time = weight - (rad1/4);
            if (collision_time < best_time) {
                best_time = collision_time;
                best_neighbor = 0;
            }
            start++;
        }
        
        for (int i = start; i < N_NEIGH; i++) {
            weight = node_data.neigh_weights[i];
            node_data_t neighbor;
            //neighbor = LUT with inx node_data.neigh[i];
            if (node_data.top_region_idx == neighbor.top_region_idx) {
                continue;
            }
            
            region_data_t region_arrived_top_neigh;
            //region_arrived_top = neighbor.top_region_idx -> LUT
            rad2 = (neighbor.wrapped_radius_cached + region_arrived_top_neigh.radius);
            
            if (rad2 & 2) {
                continue;
            }

            collision_time = weight - rad1/4 - rad2/4;
        
            if (rad2 & 1) {
                //collision_time >>= 1;
                collision_time = collision_time/2;
            }
            if (collision_time < best_time) {
                best_time = collision_time;
                best_neighbor = i;
            }
        }
        next_best_time = best_time;
        next_neigh_node = node_lut[node_data.neigh[best_neighbor]];
    } else {
        //return find_next_event_at_node_not_occupied_by_growing_top_region(detector_node, rad1);
        int best_time = LLONG_MAX;
        int best_neighbor = N_NEIGH;
        int weight;
        int rad2;
        int collision_time;

        int start = 0;
        if (node_data.neigh[0] == 0 && (node_data.neigh[1] != 0 || node_data.neigh[2] != 0 || node_data.neigh[3] != 0))
        start++;

        // Gestisce i vicini non di confine.
        for (int i = start; i < N_NEIGH; i++) {
            weight = node_data.neigh_weights[i];

            node_data_t neighbor = node_lut[node_data.neigh[i]];
            //neighbor = LUT with inx node_data.neigh[i];
            region_data_t region_arrived_top_neigh = node_lut[neighbor.top_region_idx];
            //region_arrived_top = neighbor.top_region_idx -> LUT
            rad2 = (neighbor.wrapped_radius_cached + region_arrived_top_neigh.radius);

            if (rad2 & 1) {
                collision_time = weight - rad1/4 - rad2/4;
                if (collision_time < best_time) {
                    best_time = collision_time;
                    best_neighbor = i;
                }
            }
        }
        next_best_time = best_time;
        next_neigh_node = node_lut[node_data.neigh[best_neighbor]];
    }
}

void f_do_RhB_interaction(node_data_t node_data, mwpm_event_t mwpm_event){};

void f_do_N_interaction(node_data_t data1, node_data_t data2, int relative_idx, mwpm_event_t mpwm_event){};


void f_do_look_at_node(flood_event_t event) {
    // auto next = find_next_event_at_node_returning_neighbor_index_and_time(event.node);
    node_data_t next_neigh_node;
    int next_best_time = 0;
    f_find_next_event(event.node, next_neigh_node, next_best_time);
    mwpm_event_t mwpm_event;
    
    
    if (next_best_time == queue.cur_time) {
        node_data_t node_data = node_lut[event.node];
        //TODO: tracker call
        //node.node_event_tracker.set_desired_event(&node, (cyclic_time_int){queue.cur_time}, queue);

        if (node_data.neigh[next_neigh_node.index] == 0) {
            f_do_RhB_interaction(node_data, mwpm_event);
        } else {
            node_data_t neighbor;
            f_do_N_interaction(node_data, neighbor, next_neigh_inx, event);
        }
    } else if (next_neigh_node.index != 4) {
        //TODO: tracker call
        //node.node_event_tracker.set_desired_event(&node, (cyclic_time_int){queue.cur_time}, queue);
    }

}

void f_do_blossom_shattering(region_data_t region_data, mwpm_event_t mpwm_event){}

void f_do_generate_implosion(region_data_t region_data, mwpm_event_t mpwm_event){}

void f_do_leave_node(region_data_t region_data, node_data_t node_data, region_data_shrink_t region_shrink_data){}

void f_do_region_shrinking(flood_event_t event, region_data_t region_data){
    if (/*region_data.shell_area[0] && region_data.shell_area[1] && ... = 0*/){
        f_do_blossom_shattering(region_data, event);
    } else if (/*region.shell_area.size() == 1 &&*/ /*region_data.blossom_children.empty()*/) {
        f_do_generate_implosion(region_data, event);
    } else {
        f_do_leave_node(region_data, node_data, region_shrink_data);
    }
}



void f_dispatcher(flood_event_t tentative_event, flood_event_t out_event) {
    switch (tentative_event.type) {
        case NODE: {
            f_do_look_at_node(tentative_event);
            //to save 1 bit we can have:
            //f_do_look_at_node(tentative_event.node, tentative_event.time)
            break;
        }
        case REGION_SHR: {
            f_do_region_shrinking(tentative_event);
            //to save 1 bit we can have:
            //f_do_look_at_node(tentative_event.region, tentative_event.time)
            break;
        }
        default:
            break;
    }
    
    out_event = tentative_event;
}

