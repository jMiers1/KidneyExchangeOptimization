#!/bin/sh

base_path="Users/juliusmiers/CodingWS24/ORSeminar"
instance_type="PrefLib"
instance_folder="Kidney_Matching_16_0"
instance="KP_Num2_N16_A0.txt"

input_path="/$base_path/instances/$instance_type/$instance_folder/$instance"
output_path="/$base_path/out/$instance_type/$instance_folder/$instance"

cycle_length=6
chain_length=6
degree_type="Indegree"
time_limit=15
mode="CY"

cd build
cmake ../src
make
./bin/KidneyProblem "$input_path" "$output_path" "$cycle_length" $chain_length $degree_type $time_limit $mode


#execution: ./command.sh