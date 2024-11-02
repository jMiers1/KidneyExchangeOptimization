#!/bin/sh

instance= "KP_Num1_N16_A0.txt"
instance_folder= "Kidney_Matching_16_0"
input_path= "/Users/juliusmiers/CodingWS24/ORSeminar/instances/PrefLib/$instance_folder/$instance"
output_path= "/Users/juliusmiers/CodingWS24/ORSeminar/out/$instance"

cycle_length= 3
chain_length= 4
degree_type= "Indegree"
time_limit= 15
mode= "CY"

cd build
cmake ../src
make

# Program takes 8 arguments
#
#   1: Path to instance
#   2: Output path
#   3: Cycle Length
#   4: Chain length 
#   5: Degree type
#   6: Time limit
#   7: Mode
#
#



./bin/KidneyProblem $input_path $output_path $cycle_length $chain_length $degree_type $time_limit $mode