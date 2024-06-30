#include "flooder.hpp"

void no_mwpm_event(mwpm_event_t *mwpm_event){
   mwpm_event->region = 0;
   mwpm_event->region_dst = 0;
   mwpm_event->region_src = 0;
   mwpm_event->blossom_region = 0;
   mwpm_event->in_child_region = 0;
   mwpm_event->in_parent_region = 0;
}


void f_find_next_event(node_idx_t detector_node, node_data_t *next_neigh_node, float *next_best_time, node_data_t *nodes, region_data_t * regions){
    //printf("entrato in f_find_next_event\n");
    int best_neighbor_int = 0;
    
    node_data_t detector_node_data = nodes[detector_node];
    region_data_t region_that_arrived_top_detector_node = regions[detector_node_data.top_region_idx];
    
    radius_t rad1;
    
    if(detector_node_data.top_region_idx == 0){
        rad1.value = 0;
        //rad1.status = region_that_arrived_top_detector_node.radius.status;
    }else{
        rad1.value = detector_node_data.wrapped_radius_cached + region_that_arrived_top_detector_node.radius.value;
        rad1.status = region_that_arrived_top_detector_node.radius.status;
    }
    
    if(rad1.status == GROWING){
        //printf("entrato in f_find_next_event GROWING\n");
        *next_best_time = 2147483647;
        best_neighbor_int = N_NEIGH - 1; //(4-1)
        int start = 0;
        int z = 0;
                for(int i = 0; i < N_NEIGH; i++){
                    if(detector_node_data.neigh[i] != 0){
                        z = 1;
                    }
                }
                if (/*(detector_node_data.neigh[0] != 0 ||
                     detector_node_data.neigh[1] != 0 ||
                     detector_node_data.neigh[2] != 0 ||
                     detector_node_data.neigh[3] != 0)*/ z
                    && detector_node_data.neigh[0] == 0)  {
            
            float weight = detector_node_data.neigh_weights[0];
            float collision_time = (float)(weight - rad1.value);
            if (collision_time < *next_best_time) {
                *next_best_time = (float)collision_time;
                best_neighbor_int = 0;
            }
            start++;
        }
        
        int k = 0;
        for(int i = 0; i < N_NEIGH; i++){
            if(detector_node_data.neigh[i] != 0){
                k++;
            }
        }
        for (int i = start; i < k; i++) {
            float weight = detector_node_data.neigh_weights[i];

            node_idx_t neighbor = detector_node_data.neigh[i];
            node_data_t neighbor_data = nodes[neighbor];
            
            if (detector_node_data.top_region_idx == neighbor_data.top_region_idx) {
                continue;
            }
            
            radius_t rad2;
            
            region_data_t neigh_region_that_arrived_top_detector_node = regions[neighbor_data.top_region_idx];
            
            if(neighbor_data.top_region_idx == 0){
                rad2.value = 0;
            }else{
                rad2.value = neighbor_data.wrapped_radius_cached + neigh_region_that_arrived_top_detector_node.radius.value;
                rad2.status = neigh_region_that_arrived_top_detector_node.radius.status;
            }
            
            if (rad2.status == SHRINKING) {
                continue;
            }

            float collision_time = (float)(weight - rad1.value - rad2.value);
            if (rad2.status == GROWING) {
                collision_time = collision_time/2; //(<<1)
            }
            if (collision_time < *next_best_time) {
                *next_best_time = (float)collision_time;
                best_neighbor_int = i;
                //printf("I de growing =========================== %d\n", i);
            }
        }
        
    }else{
        //printf("NO GROWING\n");
        *next_best_time = 2147483647;
        best_neighbor_int = N_NEIGH - 1; //(4-1)
        int start = 0;
        int z = 0;
                for(int i = 0; i < N_NEIGH; i++){
                    if(detector_node_data.neigh[i] != 0){
                        z = 1;
                    }
                }
                if (/*(detector_node_data.neigh[0] != 0 ||
                     detector_node_data.neigh[1] != 0 ||
                     detector_node_data.neigh[2] != 0 ||
                     detector_node_data.neigh[3] != 0)*/ z
                    && detector_node_data.neigh[0] == 0)  {
            start++;
        }
        //printf("start = %d\n", start);
        int k = 0;
        for(int i = 0; i < /*N_NEIGH*/ 4; i++){
            if(detector_node_data.neigh[i] != 0){
                k++;
            }
        }
        for (int i = start; i < k; i++) {
            float weight = detector_node_data.neigh_weights[i];

            node_idx_t neighbor = detector_node_data.neigh[i];
            //printf("neigh int = %d\n", neighbor);
            node_data_t neighbor_data = nodes[neighbor];
            
            radius_t rad2;
            
            //printf("neighbor_data.index = %d\n", neighbor_data.index);
            //printf("neighbor_data.top_region_idx = %d\n", neighbor_data.top_region_idx);
            region_data_t neigh_region_that_arrived_top_detector_node = regions[neighbor_data.top_region_idx];
            
            if(neighbor_data.top_region_idx == 0){
                rad2.value = 0;
                //(?)
                rad2.status = neigh_region_that_arrived_top_detector_node.radius.status;
            }else{
                rad2.value = neighbor_data.wrapped_radius_cached + neigh_region_that_arrived_top_detector_node.radius.value;
                rad2.status = neigh_region_that_arrived_top_detector_node.radius.status;
            }

            if (rad2.status == GROWING) {
                float collision_time = (float)(weight - rad1.value - rad2.value);
                //printf("collision_time =========================== %f\n", collision_time);
                //printf("*next_best_time =========================== %f\n", *next_best_time);
                if (collision_time < *next_best_time) {
                    *next_best_time = collision_time;
                    //printf("I =========================== %d\n", i);
                    //printf("*next_best_time =========================== %f\n", *next_best_time);
                    best_neighbor_int = i;
                }
            }
        }
        
    }
    
    //printf("*next_best_time =========================== %f\n", *next_best_time);
    //printf("BEST_NEIGH_INT = %d\n", best_neighbor_int);
    *next_neigh_node = nodes[detector_node_data.neigh[best_neighbor_int]];
    //printf("entrato in f_find_next_event\nbest time=%d\nbest neigh idx= %d\n", *next_best_time, detector_node_data.neigh[best_neighbor_int]);
}

void f_do_RhB_interaction(node_data_t node_data, mwpm_event_t *mwpm_event, node_data_t *nodes){
   no_mwpm_event(mwpm_event);
    
   mwpm_event->ce.dest = 0;
   
   mwpm_event->ce.src = node_data.reached_from_source;
   mwpm_event->ce.obs_mask = node_data.obs_inter ^ node_data.neigh_obs[0];
   mwpm_event->region = node_data.top_region_idx;
   mwpm_event->type = RegionHitBoundaryEventData;
};

void f_reschedule_events_at_detector_node(node_data_t node_data, node_data_t *nodes, region_data_t *regions){
    //printf("entrato in f_reschedule_events_at_detector_node\n");
   node_data_t next_neigh_node;
   float next_best_time;
   f_find_next_event(node_data.index, &next_neigh_node, &next_best_time, nodes, regions);
   int i = 0;
   while (next_neigh_node.index != node_data.neigh[i]) {
       i = i + 1;
   }
   
   if (i == SIZE_MAX) {
       //detector_node.node_event_tracker.set_no_desired_event();
   } else {
       /*
       detector_node.node_event_tracker.set_desired_event(
           {
               &detector_node,
               cyclic_time_int{x.second},
           },
           queue);
        */
   }
}

void f_do_region_arriving_at_empty_detector_node(region_idx_t region, node_data_t *empty_node, const node_data_t from_node, int from_to_empty_index, node_data_t *nodes, region_data_t *regions){
    //printf("entrato in f_do_region_arriving_at_empty_detector_node\n");
   empty_node->obs_inter =
           (from_node.obs_inter ^ from_node.neigh_obs[from_to_empty_index]);
   empty_node->reached_from_source = from_node.reached_from_source;
    region_data_t region_data = regions[region];
   int k = 0;
   /*
    TODO:
   if (region_data.radius & 1) {
           k = (region_data.radius >> 2) + queue.cur_time;
       } else if (region_data.radius & 2) {
           k = (region_data.radius >> 2) - queue.cur_time;
       } else {
           k = region_data.radius >> 2;
       }
    */
   empty_node->radius_of_arrival = k;
   empty_node->region_idx = region;
   empty_node->region_idx = region_data.blossom_parent_top_region_idx;
   
   int t = 0;
   if (empty_node->reached_from_source == 0) { // punta a NULL
       t = 0;
   }
   int total = 0;
   region_idx_t r = empty_node->region_idx;
    region_data_t r_t = regions[r];
   //region_data_t r_t = region_lut[r];
   while (r != empty_node->top_region_idx) {
       //r_t = region_lut[r];
       total = (int)(total + (r_t.radius.value));
       //total += r->radius.y_intercept();
       r_t = regions[r_t.blossom_parent_region_idx];
       //r_t = region_lut[r_t.blossom_parent_region_idx];
   }
   
   empty_node->wrapped_radius_cached = (total - empty_node->radius_of_arrival);
   
   //TODO: push_back
   //region_data.shell_area.push_back(empty_node.index);
   
   f_reschedule_events_at_detector_node(*empty_node, nodes, regions);
   
}

void f_do_N_interaction(node_data_t src,int src_to_dst_idx, node_data_t dst, mwpm_event_t *mwpm_event, node_data_t *nodes, region_data_t *regions){
    //printf("entrato in f_do_N_interaction\n");
    //printf("src.region_idx = %d\ndst.region_idx = %d\n", src.region_idx, dst.region_idx);
   if (src.region_idx && !dst.region_idx) {
       //printf("entrato in f_do_N_interaction PRIMO\n");
       f_do_region_arriving_at_empty_detector_node(src.region_idx, &dst, src, src_to_dst_idx, nodes, regions);
       //return MwpmEvent::no_event();
       no_mwpm_event(mwpm_event);
       //printf("NO_Event\n");
   } else if (dst.region_idx && !src.region_idx) {
       //printf("entrato in f_do_N_interaction SECONDO\n");
       int i = 0;
       while(dst.neigh[i] == src.index){
           i = i + 1;
       }
       f_do_region_arriving_at_empty_detector_node(dst.region_idx, &src, dst, i, nodes, regions);
       //return MwpmEvent::no_event();
       no_mwpm_event(mwpm_event);
       //printf("NO_Event\n");
   } else {
       //printf("entrato in f_do_N_interaction TERZO\n");
       no_mwpm_event(mwpm_event);
       mwpm_event->region_src = src.top_region_idx;
       mwpm_event->region_dst = dst.top_region_idx;
       mwpm_event->ce.src = src.reached_from_source;
       mwpm_event->ce.dest = dst.reached_from_source;
       mwpm_event->ce.obs_mask = src.obs_inter ^ dst.obs_inter ^ src.neigh_obs[src_to_dst_idx];
       mwpm_event->type = RegionHitRegionEventData;
       //printf("RegionHitRegionEvent\n");
   }
};


void f_do_look_at_node(flood_event_t event, mwpm_event_t *mwpm_event, node_data_t *nodes, region_data_t *regions) {
    //printf("entrato in f_do_look_at_node\n");
   // auto next = find_next_event_at_node_returning_neighbor_index_and_time(event.node);
   node_data_t next_neigh_node;
   float next_best_time = 0;
    f_find_next_event(event.node, &next_neigh_node, &next_best_time, nodes, regions);
    //printf("uscito da f_find_next_event\nbest time=%f\nbest neigh idx= %d\n", next_best_time, next_neigh_node.index);
    float cur_time = 5.007308;
    float epsilon = 0.0001f;
    float second_cur_time = 0;
   if (/*next_best_time == queue.cur_time*/
       fabsf(next_best_time - cur_time) < epsilon ||
             next_best_time == 0) {
       //printf("entrato in f_do_look_at_node IF CUR TIME\n");
       node_data_t node_data = nodes[event.node];
       //node_data_t node_data = node_lut[event.node];
       //TODO: tracker call
       //node.node_event_tracker.set_desired_event(&node, (cyclic_time_int){queue.cur_time}, queue);

       //if (node_data.neigh[next_neigh_node.index] == 0) {
       if (next_neigh_node.index == 0) {
           //printf("D_RhB_I\n");
           f_do_RhB_interaction(node_data, mwpm_event, nodes);
       } else {
           //printf("D_N_I\n");
           node_data_t neighbor;
           neighbor = nodes[0];
           f_do_N_interaction(node_data, next_neigh_node.index, neighbor, mwpm_event, nodes, regions);
       }
   }/*else if (next_neigh_node.index != N_NEIGH) {
       //TODO: tracker call
       //node.node_event_tracker.set_desired_event(&node, (cyclic_time_int){queue.cur_time}, queue);
   }*/else{
    //printf("NO_Event\n");
   }

}

void f_heir_region_on_shatter(node_data_t node_data, region_data_t r, node_data_t *nodes, region_data_t *regions){
    //printf("entrato in f_heir_region_on_shatter\n");
    region_data_t rr = r;
    while(1){
        region_data_t p = regions[rr.blossom_parent_region_idx];
        if(p.index == node_data.top_region_idx){
            break;
        }
        rr = p;
    }
}

void f_do_blossom_shattering(region_data_t region_data, mwpm_event_t *mwpm_event, node_data_t *nodes, region_data_t *regions, altTreeNode_data_t *alt_tree){
    //printf("entrato in f_do_blossom_shattering\n");
   no_mwpm_event(mwpm_event);
   mwpm_event->type = BlossomShatterEventData;
   mwpm_event->blossom_region = region_data.index;
    altTreeNode_data_t altTreeNode_data = alt_tree[region_data.alt_tree_node];
   //altTreeNode_data_t altTreeNode_data = altTreeNode_lut[region_data.alt_tree_node];
    node_data_t node_data_1 = nodes[altTreeNode_data.parent.edge.src];
   //node_data_t node_data_1 = node_lut[altTreeNode_data.parent.edge.src];
    node_data_t node_data_2 = nodes[altTreeNode_data.inner_to_outer_edge.src];
   //node_data_t node_data_2 = node_lut[altTreeNode_data.inner_to_outer_edge.src];
    region_data_t in_parent_region_data = regions[mwpm_event->in_parent_region];
   //region_data_t in_parent_region_data = region_lut[mwpm_event->in_parent_region];
    region_data_t in_child_region_data = regions[mwpm_event->in_child_region];
   //region_data_t in_child_region_data = region_lut[mwpm_event->in_child_region];
   f_heir_region_on_shatter(node_data_1, in_parent_region_data, nodes, regions);
   f_heir_region_on_shatter(node_data_2, in_child_region_data, nodes, regions);
}

void f_do_degenerate_implosion(region_data_t region_data, mwpm_event_t *mwpm_event, node_data_t *nodes, altTreeNode_data_t *alt_tree){
    //printf("entrato in f_do_degenerate_implosion\n");
   no_mwpm_event(mwpm_event);
    altTreeNode_data_t altTreeNode_data = alt_tree[region_data.alt_tree_node];
   //altTreeNode_data_t altTreeNode_data = altTreeNode_lut[region_data.alt_tree_node];
    altTreeNode_data_t altTreeNode_data_1 = alt_tree[altTreeNode_data.parent.alt_tree_node];
   //altTreeNode_data_t altTreeNode_data_1 = altTreeNode_lut[altTreeNode_data.parent.alt_tree_node];
   mwpm_event->region_src= altTreeNode_data_1.outer_region_idx;
   mwpm_event->region_dst = altTreeNode_data.outer_region_idx;
   mwpm_event->ce.src = altTreeNode_data.parent.edge.dest;
   mwpm_event->ce.dest = altTreeNode_data.inner_to_outer_edge.dest;
   mwpm_event->ce.obs_mask = altTreeNode_data.inner_to_outer_edge.obs_mask ^ altTreeNode_data.parent.edge.obs_mask;
   mwpm_event->type = RegionHitRegionEventData;
}

void f_schedule_tentative_shrink_event(region_data_t region_data, node_data_t *nodes, region_data_t *regions){
    //printf("entrato in f_schedule_tentative_shrink_event\n");
   /*potrei anche far passare meno info: region_shrink_data, ovvero
    region_data.index
    region_data.shell_area[SHELL_AREA_MAX]
    al massimo poi cambio
    */
   
   
   //attenzione al cumulative_time_int t
   int t;
   int k = 0;
   //TODO: empty()
   for(int i = 0; i < SHELL_AREA_MAX; i++){
       if(region_data.shell_area[i] != 0){
           k = 1;
       }
   }
   if (k == 0) { //k = 0 -> region empty
       t = (int)(region_data.radius.value);
   } else {
       node_data_t node_data = nodes[region_data.shell_area[SHELL_AREA_MAX-1]];
       //node_data_t node_data = node_lut[region_data.shell_area[SHELL_AREA_MAX-1]];
       region_data_t top_region_data = regions[node_data.top_region_idx];
       //region_data_t top_region_data = region_lut[node_data.top_region_idx];
       t = (int)(top_region_data.radius.value + node_data.wrapped_radius_cached);
   }
   
   /*TODO: set_desired_event
   region.shrink_event_tracker.set_desired_event(
       {
           &region,
           cyclic_time_int{t},
       },
       queue);
    */
}

void f_do_leave_node(region_data_t region_data, /*region_data_shrink_t region_shrink_data,*/ mwpm_event_t *mwpm_event, node_data_t *nodes, region_data_t *regions){
    //printf("entrato in f_do_leave_node\n");
    node_data_t leaving_node_data = nodes[region_data.shell_area[SHELL_AREA_MAX - 1]];
   //node_data_t leaving_node_data = node_lut[region_data.shell_area[SHELL_AREA_MAX - 1]]; //leaving_node = region.shell_area.back();
    
    leaving_node_data.index = 0;
   region_data.shell_area[SHELL_AREA_MAX - 1] = 0; //pop_back()
   leaving_node_data.region_idx = 0;
   leaving_node_data.top_region_idx = 0;
   leaving_node_data.wrapped_radius_cached = 0;
   leaving_node_data.reached_from_source = 0;
   leaving_node_data.radius_of_arrival = 0;
   leaving_node_data.obs_inter = 0;
   f_reschedule_events_at_detector_node(leaving_node_data, nodes, regions);
   f_schedule_tentative_shrink_event(region_data, nodes, regions);
   no_mwpm_event(mwpm_event);
}

void f_do_region_shrinking(flood_event_t event, mwpm_event_t *mwpm_event, node_data_t *nodes, region_data_t *regions, altTreeNode_data_t *alt_tree){
    region_idx_t region = event.node;
    region_data_t region_data = regions[region];
   
   //TODO: empty()
   //TODO: size()
   int s = 0; //s = 0 if region_data.shell_area.empty() and s = 1 if region.shell_area.size() == 1
   for(int i = 0; i < SHELL_AREA_MAX; i++){
       if(region_data.shell_area[i] != 0){
           s = s + 1;
       }
   }
   int b = 0; //b = 0 if region_data.blossom_children.empty()
   for (int i = 0; i < BLOSSOM_CHILDREN_MAX; i++){
       if(region_data.blossom_children[i].region != 0){
           b = 1;
       }
   }
   
   if (s == 0){
       f_do_blossom_shattering(region_data, mwpm_event, nodes, regions, alt_tree);
   } else if (s == 1 && b == 0) {
       f_do_degenerate_implosion(region_data, mwpm_event, nodes, alt_tree);
   } else {
       f_do_leave_node(region_data, mwpm_event, nodes, regions);
   }
}



void f_dispatcher(flood_event_t tentative_event, flood_event_t out_event, node_data_t *nodes, region_data_t *regions, altTreeNode_data_t *alt_tree) {
    //printf("entrato in f_dispatcher\n");
   mwpm_event_t mwpm_event;
   switch (tentative_event.type) {
       case NODE: {
           f_do_look_at_node(tentative_event, &mwpm_event, nodes, regions);
           break;
       }
       case REGION_SHR: {
           f_do_region_shrinking(tentative_event, &mwpm_event, nodes, regions, alt_tree);
           break;
       }
       default:
           break;
   }
   
   out_event = tentative_event;
}

void f_set_region_frozen(region_idx_t region){
   // MATCHER
}

void f_set_region_growing(region_idx_t region){
   // MATCHER
}

void f_set_region_shrinking(region_idx_t region){
   // MATCHER
}
