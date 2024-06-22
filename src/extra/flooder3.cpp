#ifndef _KERNEL_HPP
#define _KERNEL_HPP

#include "DaCH/src/cache.h" // or include DaCH src path in the cflags
#include <math.h>
#include "ap_int.h"

#define MAX_N_NODES 1000    //??? Comprehend also boundary nodes
#define MAX_N_OBS 1400      //???
#define N_NEIGH 4

#define ALTTREEEDGE_MAX 10
#define SHELL_AREA_MAX 50
#define BLOSSOM_CHILDREN_MAX 10

#define N_REGIONS MAX_N_NODES // Exagerated assumption

#define NODE_BIT ceil(log2(MAX_N_NODES+1))
#define REGION_BIT ceil(log2(N_REGIONS+1))
#define OBS_BIT ceil(log2(MAX_N_OBS))

#define LLONG_MAX 2147483647

typedef ap_uint<NODE_BIT> node_idx_t;
typedef ap_uint<REGION_BIT> region_idx_t;
typedef ap_uint<OBS_BIT> obs_mask_t;

typedef uint32_t weight_t;           //TODO: define more efficient

#define NULL_NODE 0
#define NULL_REGION 0

enum fe_type{
    NODE = 0,
    REGION_SHR = 1
};

typedef struct {
    fe_type type;
    time_t time;
    node_idx_t node; //node index
} flood_event_t;

typedef struct{
    node_idx_t src;
    node_idx_t dest;
    obs_mask_t obs_mask;
} compressed_edge_t;

typedef struct{
    region_idx_t region;
    compressed_edge_t ce;
} region_edge_t;

enum mwpm_type{
    RegionHitRegionEventData,
    RegionHitBoundaryEventData,
    BlossomShatterEventData
};

typedef struct{ //Optimizable
    region_idx_t region_src;
    region_idx_t region_dst;
    region_idx_t region;
    compressed_edge_t ce;
    region_idx_t blossom_region;
    region_idx_t in_parent_region;
    region_idx_t in_child_region;
    enum mwpm_type type;
    /*
     RegionHitRegionEventData
     
         GraphFillRegion *region1; //region_src
         GraphFillRegion *region2; //region_dst
         CompressedEdge edge;
     */
    
    /*
     RegionHitBoundaryEventData
     
         GraphFillRegion *region; //region
         CompressedEdge edge;

     */
    
    /*
     BlossomShatterEventData
     
         GraphFillRegion *blossom_region;
         GraphFillRegion *in_parent_region;
         GraphFillRegion *in_child_region;
     */
} mwpm_event_t;

typedef struct{
    node_idx_t index;
    region_idx_t region_idx;
    region_idx_t top_region_idx;
    radius_t wrapped_radius_cached;
    node_idx_t reached_from_source;
    obs_mask_t obs_inter;
    radius_t radius_of_arrival;
    node_idx_t neigh[N_NEIGH];             //if node.neigh[2] == 0 -> node hasn't the neigh[2]
    weight_t neigh_weights[N_NEIGH];
    obs_mask_t neigh_obs[N_NEIGH];
} node_data_t;

typedef struct{
    region_idx_t index;
    region_idx_t blossom_parent_region_idx;
    region_idx_t blossom_parent_top_region_idx;
    altTreeNode_t *alt_tree_node;
    radius_t radius;
    //QueuedEventTracker shrink_event_traker
    //Match match
    node_data_t shell_area[SHELL_AREA_MAX];
    region_edge_t blossom_children[BLOSSOM_CHILDREN_MAX];
} region_data_t;

typedef int altTreeNode_t;

typedef struct{
    compressed_edge_t edge;
    altTreeNode_t alt_tree_node;
} altTreeEdge_t;

typedef struct{
    altTreeNode_t index;
    region_idx_t inner_region_idx;
    region_idx_t outer_region_idx;
    compressed_edge_t inner_to_outer_edge;
    altTreeEdge_t parent;
    altTreeEdge_t children[ALTTREEEDGE_MAX];
    bool visited;
} altTreeNode_data_t;

typedef struct{
    time_t value;
    radius_status_t status;
} radius_t;

enum radius_status_t{
    GROWING = 1,
    FROZEN = 0,
    SHRINKING = -1
}


#define RD_ENABLED true
#define WR_ENABLED true
#define PORTS 1                     // number of ports (1 if WR_ENABLED is true).
#define MAIN_SIZE_NODE MAX_N_NODES  // size of the original array.
#define MAIN_SIZE_REGION N_REGIONS  // size of the original array.
#define MAIN_SIZE_ALT_TREE 10000    // size of the original array.
#define N_SETS 1                    // the number of L2 sets (1 for fully-associative cache).
#define N_WAYS 1                    // the number of L2 ways (1 for direct-mapped cache).
#define N_WORDS_PER_LINE 32          // the size of the cache line, in words.
#define LRU true                    // the replacement policy least-recently used if true, last-in first-out otherwise.
#define N_L1_SETS 0                 // the number of L1 sets.
#define N_L1_WAYS 0                 // the number of L1 ways.
#define SWAP_TAG_SET false           // the address bits mapping
#define LATENCY 2                   // the request-response distance of the L2 cache

typedef cache<node_data_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE_NODE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, SWAP_TAG_SET, LATENCY> node_cache;
typedef cache<region_data_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE_REGION, N_SETS, N_WAYS, N_WORDS_PER_LINE. LRU, SWAP_TAG_SET, LATENCY> region_cache;
typedef cache<altTreeNode_data_t, RD_ENABLED, WR_ENABLED, MAIN_SIZE_ALT_TREE, N_SETS, N_WAYS, N_WORDS_PER_LINE, LRU, SWAP_TAG_SET, LATENCY> alt_tree_cache;

typedef ap_uint<MAX_N_NODES> syndr_t;
typedef ap_uint<MAX_N_OBS> err_t;

extern "C" void top(int *, int *, int *, syndr_t *, err_t *);

#endif

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>

#include "kernel.hpp"

void no_mwpm_event(mwpm_event_t *mwpm_event){
    mwpm_event->region = 0;
    mwpm_event->region_dst = 0;
    mwpm_event->region_src = 0;
    mwpm_event->blossom_region = 0;
    mwpm_event->in_child_region = 0;
    mwpm_event->in_parent_region = 0;
    //oppure magari mettere semplicemente nella enum un tipo in più: noEvent
}


void f_find_next_event(node_idx_t node, node_data_t next_neigh_node, int next_best_time){
    
    node_data_t node_data = node_lut[node];
    region_data_t region_arrived_top = region_lut[node_data.top_region_idx];
    
    radius_t rad1 = (node_data.wrapped_radius_cached + region_arrived_top.radius);
    
    if (/*rad1 & (1 << (sizeof(int) * 8 - 1))*/ rad1 & 1) {
        //return find_next_event_at_node_occupied_by_growing_top_region(detector_node, rad1);
        
        time_t best_time = LLONG_MAX;
        int best_neighbor = N_NEIGH;
        int start = 0;
        weight_t weight;
        //node_data_t neighbor;
        radius_t rad2;
        time_t collision_time;
        
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
        time_t best_time = LLONG_MAX;
        int best_neighbor = N_NEIGH;
        weight_t weight;
        radius_t rad2;
        time_t collision_time;

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

void f_do_RhB_interaction(node_data_t node_data, mwpm_event_t mwpm_event){
    no_mwpm_event(mwpm_event);
    //nel caso in cui manteniamo la convenzione di dire che se idx = 0 punta a null
    //mwpm_event.ce.dest = 0;
    mwpm_event.ce.src = node_data.reached_from_source;
    mwpm_event.ce.obs_mask = node_data.obs_inter ^ node_data.neigh_obs[0];
    mwpm_event.region = node_data.top_region_idx;
    //considerando convenzione =0 => punta a null
    
    mwpm_event.type = RegionHitBoundaryEventData;
};

void f_reschedule_events_at_detector_node(node_data_t node_data){
    node_data_t next_neigh_node;
    int next_best_time;
    f_find_next_event(node_data.index, next_neigh_node, next_best_time);
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

void f_do_region_arriving_at_empty_detector_node(region_idx_t region, node_data_t empty_node, const node_data_t from_node, int from_to_empty_index){
    empty_node.obs_inter =
            (from_node.obs_inter ^ from_node.neigh_obs[from_to_empty_index]);
    empty_node.reached_from_source = from_node.reached_from_source;
    region_data_t region_data = region_lut[region];
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
    empty_node.radius_of_arrival = k;
    empty_node.region_idx = region;
    empty_node.region_idx = region_data.blossom_parent_top_region_idx;
    
    int t = 0;
    if (empty_node.reached_from_source == 0) { // punta a NULL
        t = 0;
    }
    int total = 0;
    region_idx_t r = empty_node.region_idx;
    region_data_t r_t = region_lut[r];
    while (r != empty_node.top_region_idx) {
        //r_t = region_lut[r];
        total = total + (r_t.radius/4);
        //total += r->radius.y_intercept();
        r_t = region_lut[r_t.blossom_parent_region_idx];
    }
    
    empty_node.wrapped_radius_cached = (total - empty_node.radius_of_arrival);
    
    //TODO: push_back
    //region_data.shell_area.push_back(empty_node.index);
    
    f_reschedule_events_at_detector_node(empty_node);
    
}

void f_do_N_interaction(node_data_t src,int src_to_dst_idx, node_data_t dst, mwpm_event_t mwpm_event){
    if (src.region_idx && !dst.region_idx) {
        f_do_region_arriving_at_empty_detector_node(src.region_idx, dst, src, src_to_dst_idx);
        //return MwpmEvent::no_event();
        no_mwpm_event(mwpm_event);
    } else if (dst.region_idx && !src.region_idx) {
        int i = 0;
        while(dst.neigh[i] == src.index){
            i = i + 1;
        }
        f_do_region_arriving_at_empty_detector_node(dst.region_idx, src, dst, i);
        //return MwpmEvent::no_event();
        no_mwpm_event(mwpm_event);
    } else {
        no_mwpm_event(mwpm_event);
        mwpm_event.region_src = src.top_region_idx;
        mwpm_event.region_dst = dst.top_region_idx;
        mwpm_event.ce.src = src.reached_from_source;
        mwpm_event.ce.dest = dst.reached_from_source;
        mwpm_event.ce.obs_mask = src.obs_inter ^ dst.obs_inter ^ src.neigh_obs[src_to_dst_idx];
        mwpm_event.type = RegionHitRegionEventData;
    }
};


void f_do_look_at_node(flood_event_t event, mwpm_event_t mwpm_event) {
    // auto next = find_next_event_at_node_returning_neighbor_index_and_time(event.node);
    node_data_t next_neigh_node;
    int next_best_time = 0;
    f_find_next_event(event.node, next_neigh_node, next_best_time);
    
    if (next_best_time == queue.cur_time) {
        node_data_t node_data = node_lut[event.node];
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
    }

}

void f_heir_region_on_shatter(node_data_t node_data, region_data_t r){
    //ciclo da riscrivere
    //TODO: heir_region_on_shatter
    
    /*
    r = region_lut[node_data.region_idx];
        while (true) {
            region_node_t p = r.blossom_parent_region_idx;
            if (p == node_data.top_region_idx) {
                r
                return r;
            }
            r = p;
        }*/
}

void f_do_blossom_shattering(region_data_t region_data, mwpm_event_t mwpm_event){
    no_mwpm_event(mwpm_event);
    mwpm_event.type = BlossomShatterEventData;
    mwpm_event.blossom_region = region_data.index;
    altTreeNode_data_t altTreeNode_data = altTreeNode_lut[region_data.alt_tree_node];
    node_data_t node_data_1 = node_lut[altTreeNode_data.parent.edge.src];
    node_data_t node_data_2 = node_lut[altTreeNode_data.inner_to_outer_edge.src];
    region_data_t in_parent_region_data = region_lut[mwpm_event.in_parent_region];
    region_data_t in_child_region_data = region_lut[mwpm_event.in_child_region];
    f_heir_region_on_shatter(node_data_1, in_parent_region_data);
    f_heir_region_on_shatter(node_data_2, in_child_region_data);
}

void f_do_degenerate_implosion(region_data_t region_data, mwpm_event_t mwpm_event){
    no_mwpm_event(mwpm_event);
    altTreeNode_data_t altTreeNode_data = altTreeNode_lut[region_data.alt_tree_node];
    altTreeNode_data_t altTreeNode_data_1 = altTreeNode_lut[altTreeNode_data.parent.alt_tree_node];
    mwpm_event.region_src= altTreeNode_data_1.outer_region_idx;
    mwpm_event.region_dst = altTreeNode_data.outer_region_idx;
    mwpm_event.ce.src = altTreeNode_data.parent.edge.dest;
    mwpm_event.ce.dest = altTreeNode_data.inner_to_outer_edge.dest;
    mwpm_event.ce.obs_mask = altTreeNode_data.inner_to_outer_edge.obs_mask ^ altTreeNode_data.parent.edge.obs_mask;
    mwpm_event.type = RegionHitRegionEventData;
}

void f_schedule_tentative_shrink_event(region_data_t region_data){
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
        if(region_data.shell_area[i].index != 0){
            k = 1;
        }
    }
    if (k == 0) { //k = 0 -> region empty
        t = region_data.radius/4;
    } else {
        node_data_t node_data = node_lut[region_data.shell_area[SHELL_AREA_MAX-1]];
        region_data_t top_region_data = region_lut[node_data.top_region_idx];
        t = (top_region_data.radius + node_data.wrapped_radius_cached)/4;
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

void f_do_leave_node(region_data_t region_data, /*region_data_shrink_t region_shrink_data,*/ mwpm_event_t mwpm_event){
    node_data_t leaving_node_data = node_lut[region_data.shell_area[SHELL_AREA_MAX - 1]]; //leaving_node = region.shell_area.back();
    region_data.shell_area[SHELL_AREA_MAX - 1].index = 0; //pop_back()
    leaving_node_data.region_idx = 0;
    leaving_node_data.top_region_idx = 0;
    leaving_node_data.wrapped_radius_cached = 0;
    leaving_node_data.reached_from_source = 0;
    leaving_node_data.radius_of_arrival = 0;
    leaving_node_data.obs_inter = 0;
    f_reschedule_events_at_detector_node(leaving_node_data);
    f_schedule_tentative_shrink_event(region_data);
    no_mwpm_event(mwpm_event);
}

void f_do_region_shrinking(flood_event_t event, mwpm_event_t mwpm_event){
    region_idx_t region = event.node;
    region_data_t region_data = region_lut[region];
    
    //TODO: empty()
    //TODO: size()
    int s = 0; //s = 0 if region_data.shell_area.empty() and s = 1 if region.shell_area.size() == 1
    for(int i; i < SHELL_AREA_MAX; i++){
        if(region_data.shell_area[i].index != 0){
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

void f_dispatcher(flood_event_t *tentative_event, bool *done) {
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
        case NO_EVENT:{
            *(done) = true;
            break;
        }
    }
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
