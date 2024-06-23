import numpy as np
import stim
import pymatching
import json

circuit = stim.Circuit.generated("surface_code:rotated_memory_x", 
                                 distance=5, 
                                 rounds=5, 
                                 after_clifford_depolarization=0.005)

model = circuit.detector_error_model(decompose_errors=True)
matching = pymatching.Matching.from_detector_error_model(model)


edges = matching.edges()
n_dets = matching.num_detectors
n_nodes = matching.num_nodes
n_fault_ids = matching.num_fault_ids
n_edges = matching.num_edges

"""
# Function to exclude specific attributes from the dictionary
def serialize_item(item, exclude_key='fault_ids'):
    if isinstance(item, tuple) and len(item) == 3 and isinstance(item[2], dict):
        item_dict = {k: v for k, v in item[2].items() if k != exclude_key}
        return (item[0], item[1], item_dict)
    return item
"""

edges_fault = [edge in edges if edge[2]["fault_ids"] != set()]
print(edges_fault)

# Initialize an empty dictionary for nodes
nodes = {}
#node_index = 0

# Iterate over each edge to populate the nodes dictionary
for edge in edges:
    serialized_edge = serialize_item(edge)
    node1, node2, attributes = serialized_edge
    for node, neighbor in [(node1, node2), (node2, node1)]:
        if node not in nodes:
            # Assign the current index to the node and increment the index counter
            nodes[node] = {"index": node, "neighbors": [], "neigh_weights": [], "neigh_obs": []}
            #node_index += 1
        #if len(nodes[node]["neighbors"]) < 4:  # Ensure a maximum of 4 neighbors
            nodes[node]["neighbors"].append(neighbor)
            nodes[node]["neigh_weights"].append(attributes.get("weight", 1))  # Default weight to 1 if not present
            nodes[node]["neigh_obs"].append(attributes.get("fault_ids", []))  # Default fault_ids to empty list if not present

# Assuming nodes is your dictionary that you want to serialize
# Convert all sets to lists in the nodes dictionary
for node, data in nodes.items():
    for key in ['neighbors', 'neigh_weights', 'neigh_obs']:
        if isinstance(data[key], set):
            data[key] = list(data[key])

# Now, serialize the nodes dictionary to JSON
with open('nodes.json', 'w') as json_file:
    json.dump({"n_dets": n_dets, "n_nodes": n_nodes, "nodes": list(nodes.values())}, json_file, indent=4)