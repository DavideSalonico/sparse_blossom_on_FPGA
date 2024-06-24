#ifndef SBA_KERNEL_CPP
#define SBA_KERNEL_CPP

#include "SBA_kernel.hpp"
#include "cache.h"


#define RD_ENABLED true
#define WR_ENABLED true
#define PORTS 1                    // number of ports (1 if WR_ENABLED is true).
#define MAIN_SIZE_NODE (unsigned int)(1 << ceillog2(MAX_N_NODES)) // size of the original array.
#define MAIN_SIZE_REGION (unsigned int)(1 << ceillog2(N_REGIONS)) // size of the original array.
#define MAIN_SIZE_ALT_TREE (unsigned int)(1 << ceillog2(ALTTREEEDGE_MAX))// size of the original array.
#define N_SETS 8                  // the number of L2 sets (1 for fully-associative cache).
#define N_WAYS 8                  // the number of L2 ways (1 for direct-mapped cache).
#define N_WORDS_PER_LINE 4        // the size of the cache line, in words.
#define LRU true                   // the replacement policy least-recently used if true, last-in first-out otherwise.
#define N_L1_SETS 0                // the number of L1 sets.
#define N_L1_WAYS 0                // the number of L1 ways.
#define SWAP_TAG_SET false         // the address bits mapping
#define LATENCY 2                  // the request-response distance of the L2 cache

// typedef cache<data_type, true, false, RD_PORTS, N * M, A_L2_SETS, A_L2_WAYS, A_WORDS, false, A_L1_SETS, A_L1_WAYS, false, A_L2_LATENCY> cache_a
typedef cache<node_data_t, RD_ENABLED, WR_ENABLED, 1, N_SETS*N_WAYS*N_WORDS_PER_LINE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, 1, 1, SWAP_TAG_SET, LATENCY> node_cache;
typedef cache<region_data_t, RD_ENABLED, WR_ENABLED, 1, MAIN_SIZE_REGION, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, 1, 1, SWAP_TAG_SET, LATENCY> region_cache;
typedef cache<altTreeNode_data_t, RD_ENABLED, WR_ENABLED, 1, N_SETS*N_WAYS*N_WORDS_PER_LINE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, 1, 1, SWAP_TAG_SET, LATENCY> alt_tree_cache;

FpgaGraph init_graph;

int n_nodes;
int n_obs;
node_cache node_lut{static_cast<node_data_t * const>(init_graph.nodes)};
region_cache region_lut{static_cast<region_data_t * const>(init_graph.regions)};
alt_tree_cache alt_tree_lut{static_cast<altTreeNode_data_t * const>(init_graph.alttree)};


void no_mwpm_event(mwpm_event_t *mwpm_event){
   mwpm_event->region = 0;
   mwpm_event->region_dst = 0;
   mwpm_event->region_src = 0;
   mwpm_event->blossom_region = 0;
   mwpm_event->in_child_region = 0;
   mwpm_event->in_parent_region = 0;
   //oppure magari mettere semplicemente nella enum un tipo in più: noEvent
}


void f_find_next_event(node_idx_t detector_node, node_data_t *next_neigh_node, int *next_best_time){
    //printf("entrato in f_find_next_event\n");
    int best_neighbor_int = 0;
    
    node_data_t detector_node_data = node_lut[detector_node];
    region_data_t region_that_arrived_top_detector_node = region_lut[detector_node_data.top_region_idx];
    
    radius_t rad1;
    
    if(detector_node_data.top_region_idx == 0){
        rad1.value = 0;
    }else{
        rad1.value = detector_node_data.wrapped_radius_cached + region_that_arrived_top_detector_node.radius.value;
        rad1.status = region_that_arrived_top_detector_node.radius.status;
    }
    
    if(rad1.status == GROWING){
        //printf("entrato in f_find_next_event GROWING\n");
        *next_best_time = (int)LLONG_MAX;
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
            && detector_node_data.neigh[0] == 0) {
            
            int weight = detector_node_data.neigh_weights[0];
            int collision_time = (int)(weight - rad1.value);
            if (collision_time < *next_best_time) {
                *next_best_time = (int)collision_time;
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
            int weight = detector_node_data.neigh_weights[i];

            node_idx_t neighbor = detector_node_data.neigh[i];
            node_data_t neighbor_data = node_lut[neighbor];
            
            if (detector_node_data.top_region_idx == neighbor_data.top_region_idx) {
                continue;
            }
            
            radius_t rad2;
            
            region_data_t neigh_region_that_arrived_top_detector_node = region_lut[neighbor_data.top_region_idx];
            
            if(neighbor_data.top_region_idx == 0){
                rad2.value = 0;
            }else{
                rad2.value = neighbor_data.wrapped_radius_cached + neigh_region_that_arrived_top_detector_node.radius.value;
                rad2.status = neigh_region_that_arrived_top_detector_node.radius.status;
            }
            
            if (rad2.status == SHRINKING) {
                continue;
            }

            int collision_time = (int)(weight - rad1.value - rad2.value);
            if (rad2.status == GROWING) {
                collision_time >>= 1;
            }
            if (collision_time < *next_best_time) {
                *next_best_time = (int)collision_time;
                best_neighbor_int = i;
            }
        }
        
    }else{
        *next_best_time = (int)LLONG_MAX;
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
            && detector_node_data.neigh[0] == 0) {
            start++;
        }
                int k = 0;
                for(int i = 0; i < N_NEIGH; i++){
                    if(detector_node_data.neigh[i] != 0){
                        k++;
                    }
                }
                for (int i = start; i < k; i++) {
                    int weight = detector_node_data.neigh_weights[i];

                    node_idx_t neighbor = detector_node_data.neigh[i];
                    node_data_t neighbor_data = node_lut[neighbor];

                    radius_t rad2;

                    region_data_t neigh_region_that_arrived_top_detector_node = region_lut[neighbor_data.top_region_idx];

                    if(neighbor_data.top_region_idx == 0){
                        rad2.value = 0;
                    }else{
                        rad2.value = neighbor_data.wrapped_radius_cached + neigh_region_that_arrived_top_detector_node.radius.value;
                        rad2.status = neigh_region_that_arrived_top_detector_node.radius.status;
                    }


                    if (rad2.status == GROWING) {
                        int collision_time = (int)(weight - rad1.value - rad2.value);
                        if (collision_time < *next_best_time) {
                            *next_best_time = collision_time;
                            best_neighbor_int = i;
                        }
                    }
                }

            }
            *next_neigh_node = node_lut[detector_node_data.neigh[best_neighbor_int]];
            //printf("entrato in f_find_next_event\nbest time=%d\nbest neigh idx= %d\n", *next_best_time, detector_node_data.neigh[best_neighbor_int]);
            /*
            #ifndef __SYTHESYS__
            printf("SET_DESIRED_EVENT NODE\n");
            #endif //__SYTHESYS__
            */
        }

        void f_do_RhB_interaction(node_data_t node_data, mwpm_event_t *mwpm_event){
            //printf("entrato in f_do_RhB_interaction\n");
            /*
            #ifndef __SYTHESYS__
            printf("MATCHER DO_RHB_INTERACTION\n");
            #endif //__SYTHESYS__
            */
            no_mwpm_event(mwpm_event);

            mwpm_event->ce.dest = 0;

            mwpm_event->ce.src = node_data.reached_from_source;
            mwpm_event->ce.obs_mask = node_data.obs_inter ^ node_data.neigh_obs[0];
            mwpm_event->region = node_data.top_region_idx;
            mwpm_event->type = RegionHitBoundaryEventData;
        };

        void f_reschedule_events_at_detector_node(node_data_t node_data){
            //printf("entrato in f_reschedule_events_at_detector_node\n");
           node_data_t next_neigh_node;
           int next_best_time;
           f_find_next_event(node_data.index, &next_neigh_node, &next_best_time);
           int i = 0;
           while (next_neigh_node.index != node_data.neigh[i]) {
               i = i + 1;
           }

           if (i == SIZE_MAX) {
               //detector_node.node_event_tracker.set_no_desired_event();
           } else {
            /*
            #ifndef __SYTHESYS__
            printf("SET_DESIRED_EVENT NODE\n");
            #endif //__SYTHESYS__
            */
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

        void f_do_region_arriving_at_empty_detector_node(region_idx_t region, node_data_t *empty_node, const node_data_t from_node, int from_to_empty_index){
            //printf("entrato in f_do_region_arriving_at_empty_detector_node\n");
           empty_node->obs_inter =
                   (from_node.obs_inter ^ from_node.neigh_obs[from_to_empty_index]);
           empty_node->reached_from_source = from_node.reached_from_source;
            region_data_t region_data = region_lut[region];
           //region_data_t region_data = region_lut[region];
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
               t = 0;}
           int total = 0;
           region_idx_t r = empty_node->region_idx;
            region_data_t r_t = region_lut[r];
           //region_data_t r_t = region_lut[r];
           while (r != empty_node->top_region_idx) {
               //r_t = region_lut[r];
               total = (int)(total + (r_t.radius.value));
               //total += r->radius.y_intercept();
               r_t = region_lut[r_t.blossom_parent_region_idx];
               //r_t = region_lut[r_t.blossom_parent_region_idx];
           }

           empty_node->wrapped_radius_cached = (total - empty_node->radius_of_arrival);

           //TODO: push_back
           //region_data.shell_area.push_back(empty_node.index);

           f_reschedule_events_at_detector_node(*empty_node);

        }

        void f_do_N_interaction(node_data_t src,int src_to_dst_idx, node_data_t dst, mwpm_event_t *mwpm_event){
            //printf("entrato in f_do_N_interaction\n");
           if (src.region_idx && !dst.region_idx) {
               //printf("entrato in f_do_N_interaction PRIMO\n");
               f_do_region_arriving_at_empty_detector_node(src.region_idx, &dst, src, src_to_dst_idx);
               //return MwpmEvent::no_event();
               no_mwpm_event(mwpm_event);
           } else if (dst.region_idx && !src.region_idx) {
               //printf("entrato in f_do_N_interaction SECONDO\n");
               int i = 0;
               while(dst.neigh[i] == src.index){
                   i = i + 1;
               }
               f_do_region_arriving_at_empty_detector_node(dst.region_idx, &src, dst, i);
               //return MwpmEvent::no_event();
               no_mwpm_event(mwpm_event);
           } else {
               //printf("entrato in f_do_N_interaction TERZO\n");
               /*
               #ifndef __SYTHESYS__
                printf("MATCHER DO_NEIGH_INTERACTION\n");
                #endif //__SYTHESYS__
                */
               no_mwpm_event(mwpm_event);
               mwpm_event->region_src = src.top_region_idx;
               mwpm_event->region_dst = dst.top_region_idx;
               mwpm_event->ce.src = src.reached_from_source;
               mwpm_event->ce.dest = dst.reached_from_source;
               mwpm_event->ce.obs_mask = src.obs_inter ^ dst.obs_inter ^ src.neigh_obs[src_to_dst_idx];
               mwpm_event->type = RegionHitRegionEventData;
           }
        };


        void f_do_look_at_node(flood_event_t event, mwpm_event_t *mwpm_event) {
            //printf("entrato in f_do_look_at_node\n");
           // auto next = find_next_event_at_node_returning_neighbor_index_and_time(event.node);
           node_data_t next_neigh_node;
           int next_best_time = 0;
            f_find_next_event(event.node, &next_neigh_node, &next_best_time);
            //printf("uscito da f_find_next_event\nbest time=%d\nbest neigh idx= %d\n", next_best_time, next_neigh_node.index);

           if (next_best_time == /*queue.cur_time*/ -4 /*1 valore casuale*/) {
               //printf("entrato in f_do_look_at_node IF CUR TIME\n");
               node_data_t node_data = node_lut[event.node];
               //node_data_t node_data = node_lut[event.node];
               //TODO: tracker call
               //node.node_event_tracker.set_desired_event(&node, (cyclic_time_int){queue.cur_time}, queue);

               if (node_data.neigh[next_neigh_node.index] == 0) {
                   f_do_RhB_interaction(node_data, mwpm_event);
               } else {
                   node_data_t neighbor;
                   f_do_N_interaction(node_data, next_neigh_node.index, neighbor, mwpm_event);
               }
           } else if (next_neigh_node.index != 4) {
               //TODO: tracker call
               //node.node_event_tracker.set_desired_event(&node, (cyclic_time_int){queue.cur_time}, queue);
                /*
                #ifndef __SYTHESYS__
                printf("SET_DESIRED_EVENT NODE\n");
                #endif //__SYTHESYS__
                */
           }

        }

        void f_heir_region_on_shatter(node_data_t node_data, region_data_t r){
            //printf("entrato in f_heir_region_on_shatter\n");
            region_data_t rr = r;
            while(1){
                region_data_t p = region_lut[rr.blossom_parent_region_idx];
                if(p.index == node_data.top_region_idx){
                    break;
                }
                rr = p;
            }
        }

        void f_do_blossom_shattering(region_data_t region_data, mwpm_event_t *mwpm_event){
            /*
            #ifndef __SYTHESYS__
            printf("MATCHER DO_BLOSSOM_SHATTERING\n");
            #endif //__SYTHESYS__
            */
            //printf("entrato in f_do_blossom_shattering\n");
           no_mwpm_event(mwpm_event);
           mwpm_event->type = BlossomShatterEventData;
           mwpm_event->blossom_region = region_data.index;
            altTreeNode_data_t altTreeNode_data = alt_tree_lut[region_data.alt_tree_node];
           //altTreeNode_data_t altTreeNode_data = altTreeNode_lut[region_data.alt_tree_node];
            node_data_t node_data_1 = node_lut[altTreeNode_data.parent.edge.src];
           //node_data_t node_data_1 = node_lut[altTreeNode_data.parent.edge.src];
            node_data_t node_data_2 = node_lut[altTreeNode_data.inner_to_outer_edge.src];
            //node_data_t node_data_2 = node_lut[altTreeNode_data.inner_to_outer_edge.src];
             region_data_t in_parent_region_data = region_lut[mwpm_event->in_parent_region];
            //region_data_t in_parent_region_data = region_lut[mwpm_event->in_parent_region];
             region_data_t in_child_region_data = region_lut[mwpm_event->in_child_region];
            //region_data_t in_child_region_data = region_lut[mwpm_event->in_child_region];
            f_heir_region_on_shatter(node_data_1, in_parent_region_data);
            f_heir_region_on_shatter(node_data_2, in_child_region_data);
         }

         void f_do_degenerate_implosion(region_data_t region_data, mwpm_event_t *mwpm_event){
            /*
            #ifndef __SYTHESYS__
            printf("MATCHER DO_DEGENERATE_IMPLOSION\n");
            #endif //__SYTHESYS__
            */
             //printf("entrato in f_do_degenerate_implosion\n");
            no_mwpm_event(mwpm_event);
             altTreeNode_data_t altTreeNode_data = alt_tree_lut[region_data.alt_tree_node];
            //altTreeNode_data_t altTreeNode_data = altTreeNode_lut[region_data.alt_tree_node];
             altTreeNode_data_t altTreeNode_data_1 = alt_tree_lut[altTreeNode_data.parent.alt_tree_node];
            //altTreeNode_data_t altTreeNode_data_1 = altTreeNode_lut[altTreeNode_data.parent.alt_tree_node];
            mwpm_event->region_src= altTreeNode_data_1.outer_region_idx;
            mwpm_event->region_dst = altTreeNode_data.outer_region_idx;
            mwpm_event->ce.src = altTreeNode_data.parent.edge.dest;
            mwpm_event->ce.dest = altTreeNode_data.inner_to_outer_edge.dest;
            mwpm_event->ce.obs_mask = altTreeNode_data.inner_to_outer_edge.obs_mask ^ altTreeNode_data.parent.edge.obs_mask;
            mwpm_event->type = RegionHitRegionEventData;
         }

         void f_schedule_tentative_shrink_event(region_data_t region_data){
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
                node_data_t node_data = node_lut[region_data.shell_area[SHELL_AREA_MAX-1]];
                //node_data_t node_data = node_lut[region_data.shell_area[SHELL_AREA_MAX-1]];
                region_data_t top_region_data = region_lut[node_data.top_region_idx];
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
            /*
            #ifndef __SYTHESYS__
            printf("SET_DESIRED_EVENT REGION\n");
            #endif //__SYTHESYS__
            */
         }

         void f_do_leave_node(region_data_t region_data, /*region_data_shrink_t region_shrink_data,*/ mwpm_event_t *mwpm_event){
             //printf("entrato in f_do_leave_node\n");
             node_data_t leaving_node_data = node_lut[region_data.shell_area[SHELL_AREA_MAX - 1]];
            //node_data_t leaving_node_data = node_lut[region_data.shell_area[SHELL_AREA_MAX - 1]]; //leaving_node = region.shell_area.back();

             leaving_node_data.index = 0;
            region_data.shell_area[SHELL_AREA_MAX - 1] = 0; //pop_back()
            leaving_node_data.region_idx = 0;
            leaving_node_data.top_region_idx = 0;
            leaving_node_data.wrapped_radius_cached = 0;
            leaving_node_data.reached_from_source = 0;
            leaving_node_data.radius_of_arrival = 0;
            leaving_node_data.obs_inter = 0;
            f_reschedule_events_at_detector_node(leaving_node_data);
            f_schedule_tentative_shrink_event(region_data);
            no_mwpm_event(mwpm_event);

            /*
            #ifndef __SYTHESYS__
            printf("MATCHER DO_LEAVE_NODE\n");
            #endif //__SYTHESYS__
            */
         }

         void f_do_region_shrinking(flood_event_t event, mwpm_event_t *mwpm_event){
             //printf("entrato in f_do_region_shrinking\n");
            region_idx_t region = event.node;
             region_data_t region_data = region_lut[region];
            //region_data_t region_data = region_lut[region];

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
                f_do_blossom_shattering(region_data, mwpm_event);
            } else if (s == 1 && b == 0) {
                f_do_degenerate_implosion(region_data, mwpm_event);
            } else {
                //f_do_leave_node(region_shrink_data, mwpm_event);
                f_do_leave_node(region_data, mwpm_event);
            }
         }

         void f_dispatcher(flood_event_t tentative_event) {
             //printf("entrato in f_dispatcher\n");
            mwpm_event_t mwpm_event;
            switch (tentative_event.type) {
                case NODE: {
                    f_do_look_at_node(tentative_event, &mwpm_event);
                    //to save 1 bit we can have:
                    //f_do_look_at_node(tentative_event.node, tentative_event.time)
                    break;
                }
                case REGION_SHR: {
                    f_do_region_shrinking(tentative_event, &mwpm_event);
                    //to save 1 bit we can have:
                    //f_do_look_at_node(tentative_event.region, tentative_event.time)
                    break;
                }
                default:
                    break;
            }

            //out_event = tentative_event;
         }

         void f_set_region_frozen(region_idx_t region){
            //TODO: f_do_region_frozen
            //attenzione, ricorsiva causa do_op_for_each_node_in_total_area
         }

         void f_set_region_growing(region_idx_t region){
            //TODO: f_do_region_frozen
            //attenzione, ricorsiva causa do_op_for_each_node_in_total_area
         }

         void f_set_region_shrinking(region_idx_t region){
            //TODO: f_do_region_frozen
            //attenzione, ricorsiva causa do_op_for_each_node_in_total_area
         }



template<typename T1, typename T2, typename T3>
void decode(T1& nodes, T2& regions, T3& alt_tree, syndr_t syndrome, corrections_t * corrections){
    
    /*
    #ifndef __SYNTHENSYS__
    for (int i = 1; i <= 5; ++i) {
        node_data_t node = nodes[i];
        std::cout << "Node: " << node.index << std::endl;
    }

    #endif //__SYNTHESYS__
    */
    

    flood_event_t fe;
    fe.node = 2;
    fe.time = 10;
    fe.type = NODE;
    f_dispatcher(fe);

    return;
}

extern "C" void sparse_top(choice_t choice, FpgaGraph* graph, syndr_t syndrome, corrections_t * corrections)
{
/*
#pragma HLS INTERFACE m_axi port = a_arr offset = slave bundle = gmem0 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = b_arr offset = slave bundle = gmem1 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = c_arr offset = slave bundle = gmem2 latency = 0 depth = 1024
#pragma HLS INTERFACE ap_ctrl_hs port = return
*/

    if (choice == LOAD_GRAPH)
    {
        n_nodes = graph->num_nodes;
        n_obs = graph->num_obs;

        for(int i = 0; i < n_nodes; i++){
            node_lut[i] = graph->nodes[i];
        }

        /*
        //DEBUG
        #ifndef __SYNTHESYS__

        std::cout << "num_nodes: " << n_nodes << std::endl;
        std::cout << "n_obs: " << n_obs << std::endl;
        for(int i = 0; i < n_nodes; i++){
            node_data_t node = node_lut[i];
            std::cout << "Node: " << node.index << std::endl;
        }

        #endif //_SYNTHESYS__
        */
    }
    else
    {
        cache_wrapper(decode<node_cache, region_cache, alt_tree_cache>, node_lut, region_lut, alt_tree_lut, syndrome, corrections);
    }
}

#endif //SBA_KERNEL_CPP

/*
chache_if:
#ifndef __SYNTHESIS__
#ifdef PROFILE
    printf("A hit ratio = \n");
    for (auto port = 0; port < RD_PORTS; port++)
    {
        printf("\tP=%d: L1=%d/%d; L2=%d/%d\n", port,
               a_cache.get_n_l1_hits(port), a_cache.get_n_l1_reqs(port),
               a_cache.get_n_hits(port), a_cache.get_n_reqs(port));
    }
    printf("B hit ratio = L1=%d/%d; L2=%d/%d\n",
           b_cache.get_n_l1_hits(0), b_cache.get_n_l1_reqs(0),
    printf("C hit ratio = L1=%d/%d; L2=%d/%d\n",
           c_cache.get_n_l1_hits(0), c_cache.get_n_l1_reqs(0),
           c_cache.get_n_hits(0), c_cache.get_n_reqs(0));
#endif // PROFILE
#endif // __SYNTHESIS__
*/



