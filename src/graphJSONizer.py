import numpy as np
import stim
import pymatching
import json

circuit = stim.Circuit.generated("surface_code:rotated_memory_x", 
                                 distance=2, 
                                 rounds=1, 
                                 after_clifford_depolarization=0.005)

model = circuit.detector_error_model(decompose_errors=True)
matching = pymatching.Matching.from_detector_error_model(model)


edges = matching.edges()
n_dets = matching.num_detectors
n_nodes = matching.num_nodes
n_fault_ids = matching.num_fault_ids
#print("Number of faults: ", n_fault_ids)
n_edges = matching.num_edges

# Function to exclude specific attributes from the dictionary
def serialize_item(item):
    if isinstance(item, tuple) and len(item) == 3 and isinstance(item[2], dict):
        item_dict = {k: (list(v) if isinstance(v, set) else v) for k, v in item[2].items()}
        return (item[0], item[1], item_dict)
    return item

serialized_edges = [serialize_item(edge) for edge in edges]
with open('edges.json', 'w') as json_file:
    json.dump({"edges": list(serialized_edges)}, json_file, indent=4)


nodes = {}
bound_edges = []
curr_boundary = 0

for edge in serialized_edges:
    node1, node2, attrs = edge
    if node1 == None:
        node1 = n_nodes + curr_boundary
        edge = (node1, node2, attrs)
        curr_boundary += 1
    elif node2 == None:
        node2 = n_nodes + curr_boundary
        edge = (node1, node2, attrs)
        curr_boundary += 1
    bound_edges.append((node1, node2, attrs))
        
    if node1 not in nodes:
        nodes[node1] = {"index": node1, "neighbors": [], "neigh_weights": [], "neigh_obs": []}
    if node2 not in nodes:
        nodes[node2] = {"index": node2, "neighbors": [], "neigh_weights": [], "neigh_obs": []}

for edge in bound_edges:
    node1, node2, attrs = edge
    nodes[node1]["neighbors"].append(node2)
    nodes[node1]["neigh_weights"].append(attrs["weight"])
    nodes[node1]["neigh_obs"].append(attrs["fault_ids"])
    
    nodes[node2]["neighbors"].append(node1)
    nodes[node2]["neigh_weights"].append(attrs["weight"])
    nodes[node2]["neigh_obs"].append(attrs["fault_ids"])

#print(nodes)

# Now, serialize the nodes dictionary to JSON
with open('nodes.json', 'w') as json_file:
    json.dump({"n_dets": n_dets, "n_nodes": n_nodes, "nodes": list(nodes.values())}, json_file, indent=4)    
    
#TODO: fix observables