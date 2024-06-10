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

import json

# Function to exclude specific attributes from the dictionary
def serialize_item(item, exclude_key='fault_ids'):
    if isinstance(item, tuple) and len(item) == 3 and isinstance(item[2], dict):
        item_dict = {k: v for k, v in item[2].items() if k != exclude_key}
        return (item[0], item[1], item_dict)
    return item

# Serialize the list with the exclusion of the specified attribute
serialized_list = [serialize_item(item) for item in edges]

# Write the serialized list to a JSON file
with open('graph.json', 'w') as json_file:
    json.dump({"n_dets": n_dets}, json_file)
    json.dump({"n_nodes": n_nodes}, json_file)
    json.dump(serialized_list, json_file)

# Read the serialized list from the JSON file
with open('graph.json', 'r') as json_file:
    loaded_list = json.load(json_file)
    print(loaded_list)
