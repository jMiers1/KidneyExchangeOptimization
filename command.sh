#!/bin/sh

instance="KP_Num1_N16_A0.txt"
instance_folder="Kidney_Matching_16_0"
input_path="/Users/juliusmiers/CodingWS24/ORSeminar/instances/PrefLib/$instance_folder/$instance"
output_path="/Users/juliusmiers/CodingWS24/ORSeminar/out/$instance"

cycle_length=3
chain_length=4
degree_type="Indegree"
time_limit=15
mode="CY"

cd build
cmake ../src
make

# echo "Input path:$input_path"
# echo "Output path: $output_path"
# echo "Cycle length: $cycle_length"
# echo "Chain length: $chain_length"
# echo "Degree type: $degree_type"
# echo "Time limit: $time_limit"
# echo "Mode: $mode" 


./bin/KidneyProblem "$input_path" "$output_path" "$cycle_length" $chain_length $degree_type $time_limit $mode