#ifndef FLOODER_H
#define FLOODER_H

#include "/home/users/davide.salonico/sparse_blossom_prj/include/SBA_kernel.hpp"

void no_mwpm_event(mwpm_event_t *mwpm_event);

void f_find_next_event(node_idx_t detector_node, node_data_t *next_neigh_node, int *next_best_time);

void f_do_RhB_interaction(node_data_t node_data, mwpm_event_t *mwpm_event);

void f_reschedule_events_at_detector_node(node_data_t node_data);

void f_do_region_arriving_at_empty_detector_node(region_idx_t region, node_data_t *empty_node, const node_data_t from_node, int from_to_empty_index);

void f_do_N_interaction(node_data_t src,int src_to_dst_idx, node_data_t dst, mwpm_event_t *mwpm_event);

void f_do_look_at_node(flood_event_t event, mwpm_event_t *mwpm_event);

void f_heir_region_on_shatter(node_data_t node_data, region_data_t r);

void f_do_blossom_shattering(region_data_t region_data, mwpm_event_t *mwpm_event);

void f_do_degenerate_implosion(region_data_t region_data, mwpm_event_t *mwpm_event);

void f_schedule_tentative_shrink_event(region_data_t region_data);

void f_do_leave_node(region_data_t region_data, /*region_data_shrink_t region_shrink_data,*/ mwpm_event_t *mwpm_event);

void f_do_region_shrinking(flood_event_t event, mwpm_event_t *mwpm_event);

void f_dispatcher(flood_event_t tentative_event);

void f_set_region_frozen(region_idx_t region);

void f_set_region_growing(region_idx_t region);

void f_set_region_shrinking(region_idx_t region);

#endif //FLOODER_H
