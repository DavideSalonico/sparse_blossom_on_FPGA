#include <cstdlib>
#include "/home/dado/sparse_blossom/sparse_blossom_on_FPGA/src/sparse_fpga.cpp"

#define N_DEC_NODES 1000
#define NODE_IDX_BIT log(2, N_DEC_NODES)
#define N_NEIGH 4
#define N_REGIONS N_DEC_NODES
#define REGION_IDX_BIT log(2, N_REGIONS)
#define N_OBS 1400
#define LLONG_MAX 2147483647
#define SHELL_AREA_MAX 4
#define BLOSSOM_CHILDREN_MAX 4
#define ALTTREEEDGE_MAX 2

//typedefs
typedef int flood_type_t;
typedef int node_t;         //index of detector node
typedef int time_t;
typedef int region_t;
typedef int obs_mask_t[N_OBS];
typedef int obs_int_t;
typedef int altTreeNode_t;


//TODO: change when merging cache implementation
node_data_t node_lut[1000000];
region_data_t region_lut[100000];
altTreeNode_data_t altTreeNode_lut[100000];

typedef struct {
    flood_type_t type;
    time_t time;
    node_t node; //node index
} flood_event_t;

typedef struct{
    node_t src;
    node_t dest;
    //obs_mask_t obs_mask;
    obs_int_t obs_mask;
} compressed_edge_t;

enum mwpm_type{
    RegionHitRegionEventData,
    RegionHitBoundaryEventData,
    BlossomShatterEventData
};

typedef struct{
    region_t region_src;
    region_t region_dst;
    region_t region;
    compressed_edge_t ce;
    region_t blossom_region;
    region_t in_parent_region;
    region_t in_child_region;
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

enum radius_status_t{
    GROWING = 1,
    FROZEN = 0,
    SHRINKING = -1
};

typedef struct{
    time_t value;
    enum radius_status_t status;
} radius_t;

typedef struct{ 
    node_t index;
    region_t region_idx;
    region_t top_region_idx;
    int wrapped_radius_cached;
    node_t reached_from_source;
    //obs_mask_t obs_inter;
    obs_int_t obs_inter;
    int radius_of_arrival;
    node_t neigh[4]; //if node.neigh[2] == 0 -> node hasn't the neigh[2]
    int neigh_weights[4];
    //obs_mask_t neigh_obs[4];
    obs_int_t neigh_obs[4];
} node_data_t;


//NEW
typedef struct{
    region_t region;
    compressed_edge_t edge;
}
match_t;

typedef struct{
    region_t index;
    region_t blossom_parent_region_idx;
    region_t blossom_parent_top_region_idx;
    altTreeNode_t alt_tree_node;
    radius_t radius;
    //QueuedEventTracker shrink_event_traker
    match_t match;
    node_data_t shell_area[4]; //4 random
    region_edge_t blossom_children[4]; //4 random
} region_data_t;

typedef struct{
    compressed_edge_t edge;
    altTreeNode_t alt_tree_node;
} altTreeEdge_t;

typedef struct{ //ho aggiunto int state
    altTreeNode_t index;
    region_t inner_region_idx;
    region_t outer_region_idx;
    compressed_edge_t inner_to_outer_edge;
    altTreeEdge_t parent;
    altTreeEdge_t children[ALTTREEEDGE_MAX];
    bool visited;
    int state;
} altTreeNode_data_t;

enum f_event_type{
    NODE = 0,
    REGION_SHR = 1
};

void m_handle_blossom_shattering(mwpm_event_t *event) {
    /*
    for (auto &child : event.blossom_region->blossom_children) {
        child.region->clear_blossom_parent();
    }*/
    
    for (int i = 0; i < BLOSSOM_CHILDREN_MAX; ++i) {
        region_data_t r = region_lut[event->blossom_region];
        if (r.blossom_children[i].region != 0) {
            //clear_blossom_parent(event.blossom_region->blossom_children[i].region);
            //clear_blossom utilizza la funzione do_op_for_each_descendant_and_self che è ricorsiva
        }
    }

    // First find indices of in_parent_region and in_child_region
    // in_parent_region is the blossom cycle region connected to the parent of the blossom inner node.
    // in_child_region is the blossom cycle region connected to the child of the inner node
    //auto blossom_cycle = std::move(event.blossom_region->blossom_children);
    region_edge_t blossom_cycle[4];
    region_data_t br;
    br = region_lut[event->blossom_region];
    for(int i = 0; i < 4; i++){
        blossom_cycle[i] = br.blossom_children[i];
        br.blossom_children[i].region = 0;
        br.blossom_children[i].ce.dest = 0;
        br.blossom_children[i].ce.src = 0;
        br.blossom_children[i].ce.obs_mask = 0;
    }
    
    altTreeNode_data_t blossom_alt_node = altTreeNode_lut[event->blossom_region.alt_tree_node];
    size_t bsize = blossom_cycle.size(); //max 4
    size_t parent_idx = 0;
    size_t child_idx = 0;
    for (size_t i = 0; i < bsize; i++) {
        if (blossom_cycle[i].region == event->in_parent_region) {
            parent_idx = i;
        }
        if (blossom_cycle[i].region == event->in_child_region) {
            child_idx = i;
        }
    }

    // Length of path starting on in_parent and stopping before in_child
    size_t gap = ((child_idx + bsize - parent_idx) % bsize);
    altTreeNode_data_t *current_alt_node;
    size_t evens_start, evens_end;

    current_alt_node = event->blossom_region.alt_tree_node.parent.alt_tree_node;
    
    /*
    ARRIVATO QUI
    unstable_erase(current_alt_node->children, [blossom_alt_node](AltTreeEdge x) {
        return x.alt_tree_node == blossom_alt_node;
    });*/
    
    auto child_edge = blossom_alt_node->parent.edge.reversed();

    if (gap % 2 == 0) {
        // The path starting after in_child and stopping before in_parent is even length. Regions will
        // be matched along this path
        evens_start = child_idx + 1;
        evens_end = child_idx + bsize - gap;

        // Now insert odd-length path starting on in_parent and ending on in_child into alternating tree
        for (size_t i = parent_idx; i < parent_idx + gap; i += 2) {
            current_alt_node = make_child(
                *current_alt_node,
                blossom_cycle[i % bsize].region,
                blossom_cycle[(i + 1) % bsize].region,
                blossom_cycle[i % bsize].edge,
                child_edge);
            child_edge = blossom_cycle[(i + 1) % bsize].edge;
            flooder.set_region_shrinking(*current_alt_node->inner_region);
            flooder.set_region_growing(*current_alt_node->outer_region);
        }
    } else {
        // The path starting after in_parent and stopping before in_child is even length. Regions will
        // be matched along this path.
        evens_start = parent_idx + 1;
        evens_end = parent_idx + gap;

        // Now insert odd-length path into alternating tree
        for (size_t i = 0; i < bsize - gap; i += 2) {
            size_t k1 = (parent_idx + bsize - i) % bsize;
            size_t k2 = (parent_idx + bsize - i - 1) % bsize;
            size_t k3 = (parent_idx + bsize - i - 2) % bsize;
            current_alt_node = make_child(
                *current_alt_node,
                blossom_cycle[k1].region,
                blossom_cycle[k2].region,
                blossom_cycle[k2].edge.reversed(),
                child_edge);
            child_edge = blossom_cycle[k3].edge.reversed();
            flooder.set_region_shrinking(*current_alt_node->inner_region);
            flooder.set_region_growing(*current_alt_node->outer_region);
        }
    }

    for (size_t j = evens_start; j < evens_end; j += 2) {
        size_t k1 = j % bsize;
        size_t k2 = (j + 1) % bsize;
        blossom_cycle[k1].region->add_match(blossom_cycle[k2].region, blossom_cycle[k1].edge);

        // The blossom regions were previously shrinking. Now they are stopped. This can create new
        // events on the nodes, and so the nodes must be reprocessed.
        blossom_cycle[k1].region->do_op_for_each_node_in_total_area([this](DetectorNode *n) {
            flooder.reschedule_events_at_detector_node(*n);
        });
        blossom_cycle[k2].region->do_op_for_each_node_in_total_area([this](DetectorNode *n) {
            flooder.reschedule_events_at_detector_node(*n);
        });
    }

    blossom_alt_node->inner_region = blossom_cycle[child_idx].region;
    flooder.set_region_shrinking(*blossom_alt_node->inner_region);
    blossom_cycle[child_idx].region->alt_tree_node = blossom_alt_node;
    current_alt_node->add_child(AltTreeEdge(blossom_alt_node, child_edge));

    flooder.region_arena.del(event.blossom_region);
}

void children_visited (altTreeNode_data_t *alt_node, int *i){
    i = ALTTREEEDGE_MAX + 1;
    for (int k = 0; k < ALTTREEEDGE_MAX; k++) {
        altTreeNode_data_t atn;
        atn = altTreeNode_lut[alt_node->children[k].alt_tree_node];
        if(atn.visited)
            i = k;
    }
}


void become_root(altTreeNode_data_t *alt_node){
    altTreeNode_data_t* current_node = alt_node;

    while (current_node->parent.alt_tree_node != NULL) {
        // Memorizza il vecchio genitore
        altTreeNode_data_t* old_parent = current_node->parent.alt_tree_node;

        // Aggiorna i puntatori interni del vecchio genitore
        old_parent->inner_region = current_node->inner_region;
        old_parent->inner_to_outer_edge = old_parent->parent.edge;

        // Rimuovi il nodo corrente dai figli del vecchio genitore
        for (int i = 0; i < ALTTREEEDGE_MAX; ++i) {
            if (old_parent->children[i].alt_tree_node == current_node) {
                old_parent->children[i].alt_tree_node = 0;
                old_parent->children[i].inner_to_outer_edge = (CompressedEdge){0, 0, 0};
                break;
            }
        }

        // Aggiorna il genitore del nodo corrente
        current_node->parent.alt_tree_node = NULL;
        current_node->parent.edge.src = 0;
        current_node->parent.edge.dest = 0;
        current_node->parent.edge.obs_mask = 0;
    
        //add_child(&(AltTreeEdge_t){ old_parent, reverse_compressed_edge(old_parent->parent.edge) });
        AltTreeEdge_t new_child;
        new_child.edge.src = old_parent->parent.edge.dest;
        new_child.edge.dest = old_parent->parent.edge.src;
        new_child.edge.obs_mask = old_parent->parent.edge.obs_mask;
        new_child.alt_tree_node = old_parent->index;
        altTreeNode_data_t new_child_atn;
        alt_node->children[ALTTREEEDGE_MAX-1] = new_child;
        new_child_atn = altTreeNode_lut[new_child.alt_tree_node];
        new_child_atn.parent.alt_tree_node = alt_node->index;
        new_child_atn.parent.src = old_parent->parent.edge.src;
        new_child_atn.parent.dest = old_parent->parent.edge.dest;
        new_child_ant.parent.obs_mask = old_parent->parent.edge.obs_mask;

        // Resetta i puntatori interni del nodo corrente
        current_node->inner_to_outer_edge.src = 0;
        current_node->inner_to_outer_edge.dest = 0;
        current_node->inner_to_outer_edge.obs_mask = 0;

        // Passa al vecchio genitore per il prossimo ciclo
        current_node = old_parent;
    }
    
    
    /*
     RICORSIVA
     if (!parent.alt_tree_node)
             return;
         auto old_parent = parent.alt_tree_node;
         old_parent->become_root();
         parent.alt_tree_node->inner_region = inner_region;
         parent.alt_tree_node->inner_to_outer_edge = parent.edge;
         inner_region = nullptr;
         unstable_erase(parent.alt_tree_node->children, [&](const AltTreeEdge &x) {
             return x.alt_tree_node == this;
         });
         parent = AltTreeEdge();
         add_child(AltTreeEdge(old_parent, inner_to_outer_edge.reversed()));
         inner_to_outer_edge = CompressedEdge{nullptr, nullptr, 0};
     */
};

void shatter_descendants_into_matches_and_freeze(altTreeNode_data_t *alt_node){
    altTreeNode_data_t* current_node = alt_tree_node;
    current_node->state = 0;  // Inizializza lo stato del nodo radice

    while (current_node->index != alt_tree_node->parent.alt_tree_node) {
        if (current_node->state == 0) {
            // Visita iniziale
            current_node->state = 1;
        }
        if (current_node->state == 1) {
            // Processa i figli
            bool has_unvisited_children = false;
            for (int i = 0; i < ALTTREEEDGE_MAX; i++) {
                altTreeNode_data_t newn;
                altTreeNode_lut[current_node->children[i].alt_tree_node]
                if (current_node->children[i].alt_tree_edge != 0 && newn.state == 0) {
                    current_node = newn;
                    current_node->state = 0;  // Inizializza lo stato del figlio
                    has_unvisited_children = true;
                    break;
                }
            }
            if (!has_unvisited_children) {
                current_node->state = 2; // Tutti i figli sono stati processati
            }
        }
        if (current_node->state == 2) {
            // Processa il nodo corrente
            if (current_node->inner_region_idx != 0) {
                //alt_tree_node.parent = AltTreeEdge();
                current_node->parent.alt_tree_node=0;
                current_node->parent.edge.src = 0;
                current_node->parent.edge.dest = 0;
                current_node->parent.edge.obs_mask = 0;
                
                region_data_t r1;
                region_data_t r2;
                r1 = region_lut[current_node->inner_region_idx];
                r2 = region_lut[current_node->outer_region_idx];
                
                //alt_tree_node.inner_region->add_match(alt_tree_node.outer_region, alt_tree_node.inner_to_outer_edge);
                r1.match.region = r2.index;
                r1.match.edge = current_node->inner_to_outer_edge;
                r2.match.region = r1.index;
                r2.match.edge.dest = r1.match.edge.src;
                r2.match.edge.src = r1.match.edge.dest;
                r2.match.edge.obs_mask = r1.match.edge.obs_mask;
                
                //flooder.set_region_frozen(r1);
                //flooder.set_region_frozen(r2);
                
                
                r1.alt_tree_node = 0;
                r2.alt_tree_node = 0;
            
            }
            if (current_node->outer_region_idx != 0) {
                region_data_t r2;
                r2 = region_lut[current_node->outer_region_idx];
                r2.alt_tree_node = 0;
            }
            
            //non credo dobbiamo effettivamente eliminarlo, al massimo mettere un altro flag che dice se un nodo è disponibile/esiste o meno (e in questo caso dovremmo settarlo su 0 in modo che si veda che non esiste più)
            //node_arena.del(current_node);
            
            
            // Reset dello stato e backtracking
            current_node->state = 0;
            if (current_node->parent.alt_tree_node != 0) {
            current_node = current_node->parent.alt_tree_node;
            } else {
                current_node = 0;
            }
        }
    }
};


void m_handle_tree_hitting_boundary(mwpm_event_t *event) {
    region_data_t region = region_lut[event->region];
    altTreeNode_data_t node = altTreeNode_lut[region.alt_tree_node];
    become_root(&node);
    shatter_descendants_into_matches_and_freeze(&node);

    region.match.region = 0;
    region.match.edge = event->ce;
    //flooder.set_region_frozen(*event.region);
}

/*
 
 
 QUA SOTTO TUTTO m_handle_region_hit_region tranne become_root e shatter_descendants_into_matches_and_freeze
 
 
 */

void most_recent_common_ancestor(altTreeNode_t alt_node_2, altTreeNode_t alt_node_1, altTreeNode_t *common_ancestor){
    
    altTreeNode_data_t *this_current = altTreeNode_lut[alt_node_1];
    this_current->visited = true;
    altTreeNode_data_t *other_current = altTreeNode_lut[alt_node_2];
    other_current->visited = true;
    altTreeNode_data_t *this_parent;
    altTreeNode_data_t *other_parent;
    
    while (true) {
    
        this_parent = this_current -> altTreeNode_lut[this_current->parent.alt_tree_node];
        other_parent = other_current -> altTreeNode_lut[other_current->parent.alt_tree_node];
        if (this_parent || other_parent) {
            if (this_parent) {
                this_current = this_parent;
                if (this_current->visited) {
                    common_ancestor = this_current;
                    break;
                }
                this_current->visited = true;
            }
            if (other_parent) {
                other_current = other_parent;
                if (other_current->visited) {
                    common_ancestor = other_current;
                    break;
                }
                other_current->visited = true;
            }
        } else {
            return nullptr;
        }
    }
    
    common_ancestor->visited = false;
    this_parent = common_ancestor->parent.alt_tree_node;
    while (this_parent && this_parent->visited) {
        this_parent->visited = false;
        this_parent = this_parent->parent.alt_tree_node;
    }
}

void handle_tree_hitting_other_tree(mwpm_event_t *event){
    altTreeNode_data_t alt_node_1 = altTreeNode_lut[region_lut[event->region_src].altTreeNode];
    altTreeNode_data_t alt_node_2 = altTreeNode_lut[region_lut[event->region_dst].altTreeNode];
    // Tree rotation
    become_root(&alt_node_1);
    become_root(&alt_node_2);
    // Match and freeze descendants
    shatter_descendants_into_matches_and_freeze(&alt_node_1);
    shatter_descendants_into_matches_and_freeze(&alt_node_2);
    // Match colliding nodes
    region_data_t region1 = region_lut[event->ce.src];
    match_t new_match;
    region_data_t region2 = region_lut[event->ce.dest];
    new_match.region = region2.index;
    new_match.edge = event->ce;
    region1.match = new_match;
    region2.match.region = region1.index;
    compressed_edge_t new_edge;
    new_edge.dest = event->ce.src;
    new_edge.src = event->ce.dest;
    new_edge.obs_mask = event->ce.obs_mask;
    region2.match.edge = new_edge;
    
    // Freeze colliding regions
    //TODO: set_region_frozen
    //flooder.set_region_frozen(*event.region1);
    //flooder.set_region_frozen(*event.region2);
}

void prune_upward_path_stopping_before(/*Arena node_arena,*/altTreeNode_t *common_ancestor, bool back, altTreeEdge_t *orphan_edges, region_edge_t *pruned_path_region_edges){
        /*
        std::vector<AltTreeEdge> orphan_edges;
        std::vector<RegionEdge> pruned_path_region_edges;
        auto current_node = this;
        if (current_node != prune_parent)
            pruned_path_region_edges.reserve(3);
        // Assumes prune_parent is an ancestor
        while (current_node != prune_parent) {
            move_append(current_node->children, orphan_edges);
            if (back) {
                pruned_path_region_edges.push_back({current_node->inner_region, current_node->inner_to_outer_edge});
                pruned_path_region_edges.push_back(
                    {current_node->parent.alt_tree_node->outer_region, current_node->parent.edge.reversed()});
            } else {
                pruned_path_region_edges.push_back(
                    {current_node->outer_region, current_node->inner_to_outer_edge.reversed()});
                pruned_path_region_edges.push_back({current_node->inner_region, current_node->parent.edge});
            }
            unstable_erase(current_node->parent.alt_tree_node->children, [&current_node](const AltTreeEdge &child_edge) {
                return child_edge.alt_tree_node == current_node;
            });
            current_node->outer_region->alt_tree_node = nullptr;
            current_node->inner_region->alt_tree_node = nullptr;
            auto to_remove = current_node;
            current_node = current_node->parent.alt_tree_node;
            arena.del(to_remove);
        }
        return {orphan_edges, pruned_path_region_edges};
         */
}

void handle_tree_hitting_self(mwpm_event_t *event, altTreeNode_t *common_ancestor){
    altTreeNode_t alt_node_1 = region_lut[event->region_src].alt_tree_node;
    altTreeNode_t alt_node_2 = region_lut[event->region_dst].alt_tree_node;
    
    altTreeNode_t orphan_edges[];
    region_edge_t pruned_path_region_edges[];
    
    /*
     il grande problema qui è che la funzione prune_upward_path_stopping_before restituisce due array di
     lunghezza indefinita, uno di altTreeEdge_t 'orphan_edges' e uno di region_edge_t 'pruned_path_region_edges'
     */
    
    
    //auto prune_result_1 = alt_node_1->prune_upward_path_stopping_before(node_arena, common_ancestor, true);
    //auto prune_result_2 = alt_node_2->prune_upward_path_stopping_before(node_arena, common_ancestor, false);

    // Construct blossom region cycle
    /*
    auto blossom_cycle = std::move(prune_result_2.pruned_path_region_edges);
    //move copia i valori di prune_result_2.pruned_path_region_edges in blossom_cycle e 'svota'/'azzera' o valori di prune_result_2.pruned_path_region_edges
    auto p1s = prune_result_1.pruned_path_region_edges.size();
    blossom_cycle.reserve(blossom_cycle.size() + p1s + 1);
    for (size_t i = 0; i < p1s; i++)
        blossom_cycle.push_back(prune_result_1.pruned_path_region_edges[p1s - i - 1]);
    blossom_cycle.push_back(RegionEdge{event.region1, event.edge});
    common_ancestor->outer_region->alt_tree_node = nullptr;
    auto blossom_region = flooder.create_blossom(blossom_cycle);

    common_ancestor->outer_region = blossom_region;
    blossom_region->alt_tree_node = common_ancestor;
    common_ancestor->children.reserve(
        common_ancestor->children.size() + prune_result_1.orphan_edges.size() + prune_result_2.orphan_edges.size());
    for (auto &c : prune_result_1.orphan_edges) {
        common_ancestor->add_child(c);
    }
    for (auto &c : prune_result_2.orphan_edges) {
        common_ancestor->add_child(c);
    }*/
}

void make_child(altTreeNode_t *parent, region_t *child_inner_region, region_t *child_outer_region, const compressed_edge_t child_inner_to_outer_edge, const compressed_edge_t child_compressed_edge, altTreeNode_t *child){
    //auto child = node_arena.alloc_unconstructed();
    altTreeNode_data_t child_node = altTreeNode_lut[child];
    child_node.inner_region_idx = child_inner_region;
    child_node.outer_region_idx = child_outer_region;
    child_node.inner_to_outer_edge = child_inner_to_outer_edge;
    
    altTreeEdge_t child_alt_tree_edge;
    child_alt_tree_edge.edge = child_compressed_edge;
    child_alt_tree_edge.alt_tree_node = child_node;
    parent.add_child(child_alt_tree_edge);
    altTreeNode_data_t parent_node = altTreeNode_lut[parent];
    int i = 0;
    while(parent_node->children[i] == 0){
        i++;
    }
    parent_node->children[i] = child_alt_tree_edge;
    altTreeNode_data_t c = altTreeNode_data_t[child_alt_tree_edge.alt_tree_node];
    
    compressed_edge_t child_edge_reversed;
    child_edge_reversed.src = child_alt_tree_edge.edge.dest;
    child_edge_reversed.dest = child_alt_tree_edge.edge.src;
    child_edge_reversed.obs_mask = child_alt_tree_edge.edge.obs_mask;
    
    c.parent.edge = child_edge_reversed;
    c.parent.alt_tree_node = parent;
    
    //non sono sicuro che gli edge creati qui rimangano, molto probabilmente bisogna fare una lut degli edge
    
}

void find_free_node(altTreeNode_t *child){
    //TODO: trova nella lut un nodo non ancora utilizzato, gli mette index oppure un flag che dice che non è più inutilizzato
    for(int i = 1; i < ALTTREEEDGE_MAX; i++){
        if(altTreeNode_lut[i].region_t inner_region_idx != 0 &&
           altTreeNode_lut[i].outer_region_idx != 0 &&
           altTreeNode_lut[i].inner_to_outer_edge != 0 &&
           altTreeNode_lut[i].parent != 0){
            child->index = i;
            break;
        }
    }
}

void handle_tree_hitting_match(region_data_t *unmatched_region, region_data_t *matched_region, compressed_edge_t *unmatched_to_matched_edge){
    altTreeNode_t alt_tree_node = unmatched_region->alt_tree_node;
    altTreeNode_t child;
    find_free_node(&child);
    make_child(*alt_tree_node, matched_region, matched_region->match.region, matched_region->match.edge, unmatched_to_matched_edge, *child);
    region_data_t other_match = region_lut[matched_region->match.region];
    
    other_match.match.region = 0;
    other_match.match.edge.src= 0;
    other_match.match.edge.dest = 0;
    other_match.match.edge.obs_mask = 0;
    
    matched_region->match.region = 0;
    matched_region->match.edge.src = 0;
    matched_region->match.edge.dest = 0;
    matched_region->match.edge.obs_mask = 0;
    
    //flooder.set_region_shrinking(*matched_region);
    //flooder.set_region_growing(*other_match);
}

void handle_tree_hitting_boundary_match(region_data_t *region1,region_data_t *region2,compressed_edge_t ce){
    altTreeNode_t alt_tree_node = region1->alt_tree_node;
    
    region2->match.region = region1.index;
    region2->match.edge.src = ce.dest;
    region2->match.edge.dest = ce.src;
    region2->match.edge.obs_mask = ce.obs_mask;
    
    //flooder.set_region_frozen(*region1);
    alt_tree_node->become_root();
    altTreeNode_data_t alt_tree_node_d = altTreeNode_lut[alt_tree_node];
    if(alt_tree_node_d.parent != 0){
        
    }
    
    shatter_descendants_into_matches_and_freeze(*alt_tree_node);
}

void m_handle_region_hit_region(mwpm_event_t *event){
    //const auto &d = event.region_hit_region_event_data;
    region_data_t region1 = region_lut[event->region_src];
    region_data_t region2 = region_lut[event->region_dst];
    altTreeNode_t alt_node_1 = region1.alt_tree_node;
    altTreeNode_t alt_node_2 = region2.alt_tree_node;
    if (alt_node_1 != 0 && alt_node_2 != 0) {
        altTreeNode_t common_ancestor;
        most_recent_common_ancestor(alt_node_2, alt_node_1, &common_ancestor);
        if (common_ancestor == 0) {
            handle_tree_hitting_other_tree(event);
        } else {
            handle_tree_hitting_self(event, &common_ancestor);
        }
    } else if (alt_node_1 != 0) {
        // Region 2 is not in the tree, so must be matched to the boundary or another region
        if (region2.match.region != 0) {
            handle_tree_hitting_match(region1, region2, event->ce);
        } else {
            handle_tree_hitting_boundary_match(region1, region2, event->ce);
        }
    } else {
        // Region 1 is not in the tree, so must be matched to the boundary or another region
        if (region1.match.region != 0) {
            handle_tree_hitting_match(region2, region1, d.edge.reversed());
        } else {
            handle_tree_hitting_boundary_match(region2, region1, d.edge.reversed());
        }
    }
}

void m_process_event(mwpm_event_t *event) {
    switch (event->type) {
        case REGION_HIT_REGION:
            m_handle_region_hit_region(event);
            break;
        case REGION_HIT_BOUNDARY:
            m_handle_tree_hitting_boundary(event);
            break;
        case BLOSSOM_SHATTER:
            m_handle_blossom_shattering(event);
            break;
        case NO_EVENT:
            // Do nothing.
            break;
    }
}
