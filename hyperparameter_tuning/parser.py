import json
import os

# Define function to parse file content
def parse_file(file_path):
    data = {}
    with open(file_path, "r") as file:
        lines = file.readlines()
    
    # Parse header information
    data["FilePath"] = file_path
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

if __name__ == '__main__':



    '''# Path to the folder containing the files
    input_file = "/Users/juliusmiers/KidneyExchangeOptimization/out/PrefLib/3_cycle_length/3_chain_length/Kidney_Matching_17_1/KP_Num11_N17_A1.txt_cycle3_chain3.txt"
    output_file = "/Users/juliusmiers/KidneyExchangeOptimization/logs/PrefLib/3_cycle_length/3_chain_length/Kidney_Matching_17_1/KP_Num11_N17_A1.txt_cycle3_chain3.txt"

    # Parse all files in the folder
    results = []
    results.append(parse_file(input_file))
    # for filename in os.listdir(input_folder):
    #     if filename.endswith(".txt"):  # Assuming files have .txt extension
    #         file_path = os.path.join(input_folder, filename)
    #         results.append(parse_file(file_path))

    # Save the results as a JSON file
    with open(output_file, "w") as json_file:
        json.dump(results, json_file, indent=4)

    print(f"Data has been written to {output_file}")
    '''
    # Paths
    base_input_folder = "/Users/juliusmiers/KidneyExchangeOptimization/out"
    results = []
    for root, _, files in os.walk(base_input_folder):
        for file in files:
            if file.endswith(".txt"):
                # Input and output file paths
                input_file = os.path.join(root, file)
                '''relative_path = os.path.relpath(input_file, base_input_folder)
                output_file = os.path.join(base_output_folder, relative_path).replace(".txt", ".json")
                
                # Ensure the output folder structure exists
                os.makedirs(os.path.dirname(output_file), exist_ok=True)
                
                # Parse the file and save as JSON
                parsed_data = parse_file(input_file)
                with open(output_file, "w") as json_file:
                    json.dump(parsed_data, json_file, indent=4)

                print(f"Processed {input_file} -> {output_file}")'''
                try: 
                    results.append(parse_file(input_file))
                except: 
                    results.append(f'Skipped {input_file}')
                    print(f'Skipped {input_file}')

    output_json_file = "/Users/juliusmiers/KidneyExchangeOptimization/hyperparameter_tuning/results.json"  # Change this to the desired output file path

    # Write the results to the JSON file
    with open(output_json_file, "w") as json_file:
        json.dump(results, json_file, indent=4)

    print("Written output to json")