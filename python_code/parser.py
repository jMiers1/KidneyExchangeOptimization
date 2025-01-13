import json
import os

# Define function to parse file content
def parse_file(file_path):
    data = {}
    with open(file_path, "r") as file:
        lines = file.readlines()
    
    # Parse header information
    data["Instance"] = lines[0].split(":")[1].strip()
    data["MaxCycleLength"] = int(lines[1].split(":")[1].strip())
    data["MaxChainLength"] = int(lines[2].split(":")[1].strip())
    data["DegreeType"] = lines[3].split(":")[1].strip()
    data["Preference"] = lines[4].split(":")[1].strip()
    data["TimeTaken"] = float(lines[5].split(":")[1].strip())
    data["TimeLimit"] = int(lines[6].split(":")[1].strip())
    data["Status"] = lines[7].split(":")[1].strip()
    data["ObjectiveValue"] = int(lines[8].split(":")[1].strip())
    
    # Parse cycles
    data["Cycles"] = []
    for line in lines[9:]:
        if line.startswith("Cycle"):
            cycle_values = list(map(int, line.split(":")[1].strip().split()))
            data["Cycles"].append(cycle_values)
        elif line.startswith("Chain"):
            break
    
    # Parse chains
    data["Chains"] = []
    for line in lines[9:]:
        if line.startswith("Chain"):
            chain_values = list(map(int, line.split(":")[1].strip().split()))
            data["Chains"].append(chain_values)
    
    # Parse footer information
    footer_index = len(lines) - 3
    data["AverageChainLength"] = float(lines[footer_index].split(":")[1].strip())
    data["AverageCycleLength"] = float(lines[footer_index + 1].split(":")[1].strip())
    data["HighlySensitizedPercentage"] = float(lines[footer_index + 2].split(":")[1].split()[0].strip())

    return data

# Path to the folder containing the files
input_folder = "/path/to/files"
output_file = "output.json"

# Parse all files in the folder
results = []
for filename in os.listdir(input_folder):
    if filename.endswith(".txt"):  # Assuming files have .txt extension
        file_path = os.path.join(input_folder, filename)
        results.append(parse_file(file_path))

# Save the results as a JSON file
with open(output_file, "w") as json_file:
    json.dump(results, json_file, indent=4)

print(f"Data has been written to {output_file}")
