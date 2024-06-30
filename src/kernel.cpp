#include "SBA_kernel.hpp"

region_data_t regions[N_REGIONS];
altTreeNode_data_t alt_tree[ALTTREEEDGE_MAX];

/**
 *
 * no_mwpm_event generator
 *
 * @param mwpm_event is a void event used to signal that to no operations are required from the matcher
 */
void no_mwpm_event(mwpm_event_t *mwpm_event)
{
    mwpm_event->region = 0;
    mwpm_event->region_dst = 0;
    mwpm_event->region_src = 0;
    mwpm_event->blossom_region = 0;
    mwpm_event->in_child_region = 0;
    mwpm_event->in_parent_region = 0;
}

/**
 *
 * finds the next event at particular node distinguishing the case in which it is occupied by a growing top region or not
 *
 * @param detector_node index of the node we want to find the next event
 * @param next_neigh_node node of the next event
 * @param next_best_time time of the next event
 */
void f_find_next_event(node_idx_t detector_node, node_data_t *next_neigh_node, float *next_best_time, node_data_t *nodes, region_data_t *regions)
{
    int best_neighbor_int = 0;

    node_data_t detector_node_data = nodes[detector_node];
    region_data_t region_that_arrived_top_detector_node = regions[detector_node_data.top_region_idx];

    radius_t rad1;

    if (detector_node_data.top_region_idx == 0)
    {
        rad1.value = 0;
    }
    else
    {
        rad1.value = detector_node_data.wrapped_radius_cached + region_that_arrived_top_detector_node.radius.value;
        rad1.status = region_that_arrived_top_detector_node.radius.status;
    }

    if (rad1.status == GROWING)
    {
        *next_best_time = 2147483647;
        best_neighbor_int = N_NEIGH - 1; //(4-1)
        int start = 0;
        int z = 0;
        for (int i = 0; i < N_NEIGH; i++)
        {
            if (detector_node_data.neigh[i] != 0)
            {
                z = 1;
            }
        }
        if (z && detector_node_data.neigh[0] == 0)
        {

            float weight = detector_node_data.neigh_weights[0];
            float collision_time = (float)(weight - rad1.value);
            if (collision_time < *next_best_time)
            {
                *next_best_time = (float)collision_time;
                best_neighbor_int = 0;
            }
            start++;
        }

        int k = 0;
    NEIGH_NEXT_EVENT:
        for (int i = 0; i < N_NEIGH; i++)
        {
#pragma HLS UNROLL factor = 8
            if (detector_node_data.neigh[i] != 0)
            {
                k++;
            }
        }
        for (int i = start; i < k; i++)
        {
            float weight = detector_node_data.neigh_weights[i];

            node_idx_t neighbor = detector_node_data.neigh[i];
            node_data_t neighbor_data = nodes[neighbor];

            if (detector_node_data.top_region_idx == neighbor_data.top_region_idx)
            {
                continue;
            }

            radius_t rad2;

            region_data_t neigh_region_that_arrived_top_detector_node = regions[neighbor_data.top_region_idx];

            if (neighbor_data.top_region_idx == 0)
            {
                rad2.value = 0;
            }
            else
            {
                rad2.value = neighbor_data.wrapped_radius_cached + neigh_region_that_arrived_top_detector_node.radius.value;
                rad2.status = neigh_region_that_arrived_top_detector_node.radius.status;
            }

            if (rad2.status == SHRINKING)
            {
                continue;
            }

            float collision_time = (float)(weight - rad1.value - rad2.value);
            if (rad2.status == GROWING)
            {
                collision_time = collision_time / 2;
            }
            if (collision_time < *next_best_time)
            {
                *next_best_time = (float)collision_time;
                best_neighbor_int = i;
            }
        }
    }
    else
    {
        *next_best_time = 2147483647;
        best_neighbor_int = N_NEIGH - 1; //(4-1)
        int start = 0;
        int z = 0;
        for (int i = 0; i < N_NEIGH; i++)
        {
            if (detector_node_data.neigh[i] != 0)
            {
                z = 1;
            }
        }
        if (z && detector_node_data.neigh[0] == 0)
        {
            start++;
        }
        int k = 0;
        for (int i = 0; i < N_NEIGH; i++)
        {
            if (detector_node_data.neigh[i] != 0)
            {
                k++;
            }
        }
        for (int i = start; i < k; i++)
        {
            float weight = detector_node_data.neigh_weights[i];

            node_idx_t neighbor = detector_node_data.neigh[i];
            node_data_t neighbor_data = nodes[neighbor];
            radius_t rad2;
            region_data_t neigh_region_that_arrived_top_detector_node = regions[neighbor_data.top_region_idx];

            if (neighbor_data.top_region_idx == 0)
            {
                rad2.value = 0;
                rad2.status = neigh_region_that_arrived_top_detector_node.radius.status;
            }
            else
            {
                rad2.value = neighbor_data.wrapped_radius_cached + neigh_region_that_arrived_top_detector_node.radius.value;
                rad2.status = neigh_region_that_arrived_top_detector_node.radius.status;
            }

            if (rad2.status == GROWING)
            {
                float collision_time = (float)(weight - rad1.value - rad2.value);
                if (collision_time < *next_best_time)
                {
                    *next_best_time = collision_time;
                    best_neighbor_int = i;
                }
            }
        }
    }

    *next_neigh_node = nodes[detector_node_data.neigh[best_neighbor_int]];
}

/**
 *
 * detects COLLIDE event and creates a RegionHitBoundary MWPM event
 *
 * @param node_data node information given by the flood_event
 * @param mwpm_event is the event that the flooder will pass to the matcher
 */
void f_do_RhB_interaction(node_data_t node_data, mwpm_event_t *mwpm_event, node_data_t *nodes)
{
    no_mwpm_event(mwpm_event);

    mwpm_event->ce.dest = 0;

    mwpm_event->ce.src = node_data.reached_from_source;
    mwpm_event->ce.obs_mask = node_data.obs_inter ^ node_data.neigh_obs[0];
    mwpm_event->region = node_data.top_region_idx;
    mwpm_event->type = RegionHitBoundaryEventData;
};

/**
 *
 * manages calling to the tracker, setting a desired or not desired event
 *
 * @param node_data info about the node to do a call to the tracker
 */
void f_reschedule_events_at_detector_node(node_data_t node_data, node_data_t *nodes, region_data_t *regions)
{
    node_data_t next_neigh_node;
    float next_best_time;
    f_find_next_event(node_data.index, &next_neigh_node, &next_best_time, nodes, regions);
    int i = 0;
    while (next_neigh_node.index != node_data.neigh[i])
    {
        i = i + 1;
    }

    if (i == SIZE_MAX)
    {
        // TRACKER call : set_no_deseried_event()
    }
    else
    {
        // TRACKER call : set_desired_event()
    }
}

/**
 *
 * manages an ARRIVE event calling the tracker
 *
 * @param region index of the region of the node given by the flood event
 * @param empty_node neighbour
 * @param from_node node given by the flood event
 * @param from_to_empty_index index of the neighbour
 */
void f_do_region_arriving_at_empty_detector_node(region_idx_t region, node_data_t *empty_node, const node_data_t from_node, int from_to_empty_index, node_data_t *nodes, region_data_t *regions)
{
    // printf("entrato in f_do_region_arriving_at_empty_detector_node\n");
    empty_node->obs_inter =
        (from_node.obs_inter ^ from_node.neigh_obs[from_to_empty_index]);
    empty_node->reached_from_source = from_node.reached_from_source;
    region_data_t region_data = regions[region];
    int k = 0;
    empty_node->radius_of_arrival = k;
    empty_node->region_idx = region;
    empty_node->region_idx = region_data.blossom_parent_top_region_idx;

    int t = 0;
    if (empty_node->reached_from_source == 0)
    { // punta a NULL
        t = 0;
    }
    int total = 0;
    region_idx_t r = empty_node->region_idx;
    region_data_t r_t = regions[r];
REGION_ARRIVING:
    while (r != empty_node->top_region_idx)
    {
#pragma HLS UNROLL factor = 8
        total = (int)(total + (r_t.radius.value));
        r_t = regions[r_t.blossom_parent_region_idx];
    }

    empty_node->wrapped_radius_cached = (total - empty_node->radius_of_arrival);

    f_reschedule_events_at_detector_node(*empty_node, nodes, regions);
}

/**
 *
 * checks the intereaction among neighbors and if it detects ARRIVE event it calls f_do_region_arriving_at_empty_detector_node, if it detects COLLIDE event it creates a RegionHitRegion MWPM event
 *
 * @param src node information given by the flood_event
 * @param src_to_dst_idx index of the neighbor with whom it interacts
 * @param dst neighbour node
 * @param mwpm_event is the event that the flooder will pass to the matcher
 */
void f_do_N_interaction(node_data_t src, int src_to_dst_idx, node_data_t dst, mwpm_event_t *mwpm_event, node_data_t *nodes, region_data_t *regions)
{
    if (src.region_idx && !dst.region_idx)
    {
        f_do_region_arriving_at_empty_detector_node(src.region_idx, &dst, src, src_to_dst_idx, nodes, regions);
        no_mwpm_event(mwpm_event);
    }
    else if (dst.region_idx && !src.region_idx)
    {
        int i = 0;
        while (dst.neigh[i] == src.index)
        {
            i = i + 1;
        }
        f_do_region_arriving_at_empty_detector_node(dst.region_idx, &src, dst, i, nodes, regions);
        no_mwpm_event(mwpm_event);
    }
    else
    {
        no_mwpm_event(mwpm_event);
        mwpm_event->region_src = src.top_region_idx;
        mwpm_event->region_dst = dst.top_region_idx;
        mwpm_event->ce.src = src.reached_from_source;
        mwpm_event->ce.dest = dst.reached_from_source;
        mwpm_event->ce.obs_mask = src.obs_inter ^ dst.obs_inter ^ src.neigh_obs[src_to_dst_idx];
        mwpm_event->type = RegionHitRegionEventData;
    }
};

/**
 *
 * handles a node-flood_event
 *
 * @param tentative_event is a flood event given by the tracker
 * @param mwpm_event is the event that the flooder will pass to the matcher
 */
void f_do_look_at_node(flood_event_t event, mwpm_event_t *mwpm_event, node_data_t *nodes, region_data_t *regions)
{
    ;
    node_data_t next_neigh_node;
    float next_best_time = 0;
    f_find_next_event(event.node, &next_neigh_node, &next_best_time, nodes, regions);
    float cur_time = 5.007308;
    float epsilon = 0.0001f;
    float second_cur_time = 0;
    if (fabsf(next_best_time - cur_time) < epsilon || next_best_time == 0)
    {
        node_data_t node_data = nodes[event.node];
        // TRACKER call : set_desired_event()

        if (next_neigh_node.index == 0)
        {
            f_do_RhB_interaction(node_data, mwpm_event, nodes);
        }
        else
        {
            node_data_t neighbor;
            neighbor = nodes[0];
            f_do_N_interaction(node_data, next_neigh_node.index, neighbor, mwpm_event, nodes, regions);
        }
    }
    else
    {
        // MATCHER call : NO_EVENT
    }
}

/**
 *
 * manages the assignment of the shattered region to a specific node
 *
 * @param node_data
 * @param region_data is the shattered region
 */
void f_heir_region_on_shatter(node_data_t node_data, region_data_t r, node_data_t *nodes, region_data_t *regions)
{
    region_data_t rr = r;
    while (1)
    {
        region_data_t p = regions[rr.blossom_parent_region_idx];
        if (p.index == node_data.top_region_idx)
        {
            break;
        }
        rr = p;
    }
}

/**
 *
 * detects a IMPLODE event and create a BlossomShatterEvent MWPM event
 *
 * @param region_data is the region information given by the flood event
 * @param mwpm_event is the event that the flooder will pass to the matcher
 */
void f_do_blossom_shattering(region_data_t region_data, mwpm_event_t *mwpm_event, node_data_t *nodes, region_data_t *regions, altTreeNode_data_t *alt_tree)
{
    no_mwpm_event(mwpm_event);
    mwpm_event->type = BlossomShatterEventData;
    mwpm_event->blossom_region = region_data.index;
    altTreeNode_data_t altTreeNode_data = alt_tree[region_data.alt_tree_node];
    node_data_t node_data_1 = nodes[altTreeNode_data.parent.edge.src];
    node_data_t node_data_2 = nodes[altTreeNode_data.inner_to_outer_edge.src];
    region_data_t in_parent_region_data = regions[mwpm_event->in_parent_region];
    region_data_t in_child_region_data = regions[mwpm_event->in_child_region];
    f_heir_region_on_shatter(node_data_1, in_parent_region_data, nodes, regions);
    f_heir_region_on_shatter(node_data_2, in_child_region_data, nodes, regions);
    mwpm_event->in_parent_region = in_parent_region_data.index;
    mwpm_event->in_child_region = in_child_region_data.index;
}

/**
 *
 * detects a IMPLODE event and create a RegionHitRegion MWPM event
 *
 * @param region_data is the region information given by the flood event
 * @param mwpm_event is the event that the flooder will pass to the matcher
 */
void f_do_degenerate_implosion(region_data_t region_data, mwpm_event_t *mwpm_event, node_data_t *nodes, altTreeNode_data_t *alt_tree)
{
    no_mwpm_event(mwpm_event);
    altTreeNode_data_t altTreeNode_data = alt_tree[region_data.alt_tree_node];
    altTreeNode_data_t altTreeNode_data_1 = alt_tree[altTreeNode_data.parent.alt_tree_node];
    mwpm_event->region_src = altTreeNode_data_1.outer_region_idx;
    mwpm_event->region_dst = altTreeNode_data.outer_region_idx;
    mwpm_event->ce.src = altTreeNode_data.parent.edge.dest;
    mwpm_event->ce.dest = altTreeNode_data.inner_to_outer_edge.dest;
    mwpm_event->ce.obs_mask = altTreeNode_data.inner_to_outer_edge.obs_mask ^ altTreeNode_data.parent.edge.obs_mask;
    mwpm_event->type = RegionHitRegionEventData;
}

/**
 *
 * handles the reschedule of the LEAVE event
 *
 * @param region_data is the region information given by the flood event
 */
void f_schedule_tentative_shrink_event(region_data_t region_data, node_data_t *nodes, region_data_t *regions)
{
    int t;
    int k = 0;
SCHEDULE_EVENT:
    for (int i = 0; i < SHELL_AREA_MAX; i++)
    {
#pragma HLS PIPELINE II = 1
        if (region_data.shell_area[i] != 0)
        {
            k = 1;
        }
    }
    if (k == 0)
    {
        t = (int)(region_data.radius.value);
    }
    else
    {
        node_data_t node_data = nodes[region_data.shell_area[SHELL_AREA_MAX - 1]];
        region_data_t top_region_data = regions[node_data.top_region_idx];
        t = (int)(top_region_data.radius.value + node_data.wrapped_radius_cached);
    }

    // TRACKER call : set_desred_event()
}

/**
 *
 * detects a LEAVE event, call the tracker to reschedule the event and create a void MWPM No_event
 *
 * @param region_data is the region information given by the flood event
 * @param mwpm_event is the event that the flooder will pass to the matcher
 */
void f_do_leave_node(region_data_t region_data, mwpm_event_t *mwpm_event, node_data_t *nodes, region_data_t *regions)
{
    node_data_t leaving_node_data = nodes[region_data.shell_area[SHELL_AREA_MAX - 1]];

    leaving_node_data.index = 0;
    region_data.shell_area[SHELL_AREA_MAX - 1] = 0;
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

/**
 *
 * handles a region-flood_event
 *
 * @param tentative_event is a flood event given by the tracker
 * @param mwpm_event is the event that the flooder will pass to the matcher
 */
void f_do_region_shrinking(flood_event_t event, mwpm_event_t *mwpm_event, node_data_t *nodes, region_data_t *regions, altTreeNode_data_t *alt_tree)
{
    region_idx_t region = event.node;
    region_data_t region_data = regions[region];

    int s = 0;
REGION_SHRINK_SHELL:
    for (int i = 0; i < SHELL_AREA_MAX; i++)
    {
#pragma HLS PIPELINE II = 1
        if (region_data.shell_area[i] != 0)
        {
            s = s + 1;
        }
    }
    int b = 0;
REGION_SHRINK_CHILDREN:
    for (int i = 0; i < BLOSSOM_CHILDREN_MAX; i++)
    {
#pragma HLS PIPELINE II = 1
        if (region_data.blossom_children[i].region != 0)
        {
            b = 1;
        }
    }

    if (s == 0)
    {
        f_do_blossom_shattering(region_data, mwpm_event, nodes, regions, alt_tree);
    }
    else if (s == 1 && b == 0)
    {
        f_do_degenerate_implosion(region_data, mwpm_event, nodes, alt_tree);
    }
    else
    {
        f_do_leave_node(region_data, mwpm_event, nodes, regions);
    }
}

/**
 *
 * handles a flood_event based on its type
 *
 * @param tentative_event is a flood event given by the tracker
 */
void f_dispatcher(flood_event_t tentative_event, flood_event_t out_event, node_data_t *nodes, region_data_t *regions, altTreeNode_data_t *alt_tree)
{
    mwpm_event_t mwpm_event;
    switch (tentative_event.type)
    {
    case NODE:
    {
        f_do_look_at_node(tentative_event, &mwpm_event, nodes, regions);
        break;
    }
    case REGION_SHR:
    {
        f_do_region_shrinking(tentative_event, &mwpm_event, nodes, regions, alt_tree);
        break;
    }
    default:
        break;
    }

    out_event = tentative_event;
}

void f_set_region_frozen(region_idx_t region)
{
    // MATCHER call
}

void f_set_region_growing(region_idx_t region)
{
    // MATCHER call
}

void f_set_region_shrinking(region_idx_t region)
{
    // MATCHER call
}

void decode(node_data_t *nodes, syndr_t syndrome, corrections_t *corrections)
{
    // Hardcoded event to start the computation
    // In the report is clearly explained why
    flood_event_t fe;
    fe.node = 2;
    fe.time = 10;
    fe.type = NODE;
    f_dispatcher(fe, fe, nodes, regions, alt_tree);

    *(corrections) = syndrome % 2; // just to simulate interaction with corrections
}

extern "C" void sparse_top(node_data_t *nodes, syndr_t syndrome, corrections_t *corrections)
{
#pragma HLS INTERFACE m_axi port = nodes offset = slave bundle = gmem0 latency = 0 depth = 1024
#pragma HLS INTERFACE m_axi port = corrections offset = slave bundle = gmem1 latency = 0 depth = 1024

#pragma HLS INTERFACE s_axilite port = nodes bundle = control
#pragma HLS INTERFACE s_axilite port = syndrome bundle = control
#pragma HLS INTERFACE s_axilite port = corrections bundle = control

#pragma HLS INTERFACE s_axilite port = return bundle = control

    decode(nodes, syndrome, corrections);
}
