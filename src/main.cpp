//
//  main.cpp
//
//  Created by Carolina Riascos Alvarez on 2019-11-27.
//  Copyright © 2019 Carolina Riascos Alvarez. All rights reserved.
//


#include "main_VFS.hpp"
#include <chrono>
#include <map>

int main(int argc, const char * argv[]) {

    map<string, double> time_measurements;

    if (argc < 8) {
        cout << "The program expects 7 additional arguments" << endl;
        
        cout << "Received: \n";

        for(int i{0}; i < argc; i++) {
            cout << "Argument " << i << ": " << std::string{argv[i]} << "\n";
        }
        
        return -1;
    }

    cout << "1: Input Path - got: \"" << string(argv[1]) << "\"\n" 
        << "2: Output Path - got: \"" << string(argv[2]) << "\"\n"
        << "3: Max cycle length - got: \"" << string(argv[3]) << "\"\n"
        << "4: Max chain length - got: \"" << string(argv[4]) << "\"\n"
        << "5: Degree type - got: \"" << string(argv[5]) << "\"\n"
        << "6: Time limit - got: \"" << string(argv[6]) << "\"\n"
        << "7: Cycle / Chain mode - got: \"" << string(argv[7]) << "\"" << endl;

    

    // Read user input
    auto time_start = chrono::high_resolution_clock::now();
    auto time_start_global = chrono::high_resolution_clock::now();
    string FilePath = argv[1];
    string OutputPath = argv[2];
    stringstream str; str << argv[3];
    IloInt CycleLength; str >> CycleLength;
    str.clear(); str << argv[4];
    IloInt ChainLength; str >> ChainLength;
    string DegreeType = argv[5];
    str.clear(); str << argv[6];
    IloNum TimeLimit; str >> TimeLimit;
    string Preference = argv[7];
    auto time_end = std::chrono::high_resolution_clock::now();
    time_measurements["Read user input"] = std::chrono::duration<double>(time_end - time_start).count();

    
    // Read intance File
    time_start = std::chrono::high_resolution_clock::now();
    Problem P(FilePath, OutputPath, DegreeType, CycleLength, ChainLength, TimeLimit, WeightMatrix, AdjacencyList, Pairs, NDDs, Preference);
    cout << "Reading input graph..." << endl;
    if (P.ReadData() == 0) {
        cout << "Failed while reading input..." << endl;
        return -1;
    }
    time_end = std::chrono::high_resolution_clock::now();
    time_measurements["Read instance file"] = std::chrono::duration<double>(time_end - time_start).count();

    
    // Confoigure solution mode
    time_start = std::chrono::high_resolution_clock::now();
    if (P.Nodes > 2000){
        if (ChainLength == 0){
            P.CadaCuantoMerge = 600;
        }
        else{
            P.CadaCuantoMerge = 20;
        }
    }else{
        if (ChainLength == 0){
            if (CycleLength >= 4){
                P.CadaCuantoMerge = 600;
            }
            else{
                P.CadaCuantoMerge = ceil(P.Nodes/2);
            }
        }
        else{
            P.CadaCuantoMerge = 20;
        }
    }
    time_end = std::chrono::high_resolution_clock::now();
    time_measurements["Configure solution mode"] = std::chrono::duration<double>(time_end - time_start).count();

    
    // Apply MDD 
    time_start = std::chrono::high_resolution_clock::now();
    P.BuildMDDs();
    time_end = std::chrono::high_resolution_clock::now();
    time_measurements["Apply MDD"] = std::chrono::duration<double>(time_end - time_start).count();
    
    // Apply BBTree
    time_start = chrono::high_resolution_clock::now();
    P.BBTree();
    time_end = chrono::high_resolution_clock::now();
    time_measurements["Apply BBTree"] = std::chrono::duration<double>(time_end - time_start).count();
    
    auto time_end_global = chrono::high_resolution_clock::now();
    double time_global{std::chrono::duration<double>(time_end_global - time_start_global).count()};
    cout << endl << "End \n" <<endl;

    // Output results in a table format
    int step = 0;
    cout << setw(10) << "Step" << setw(25) << "Task" << setw(25) << "Time (seconds)" << setw(25) << "Time (%)" << '\n';
    for (auto it = time_measurements.begin(); it != time_measurements.end(); ++it) {
        cout << setw(10) << step << setw(25) << it->first << setw(25) << it->second << setw(25) << (it->second)/time_global <<'\n';
        ++step;
    }

    return 0;
}
