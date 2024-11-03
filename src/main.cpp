//
//  main.cpp
//
//  Created by Carolina Riascos Alvarez on 2019-11-27.
//  Copyright © 2019 Carolina Riascos Alvarez. All rights reserved.
//

#include <mach/mach.h>
#include "main_VFS.hpp"
#include <chrono>
#include "timer.hpp"
#include "BBTree.hpp"
#include <thread> 


double measureMemoryUsage() {
    task_basic_info info;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT; 
    kern_return_t result = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &count);
    if (result != KERN_SUCCESS) {
        std::cerr << "Error retrieving memory info: " << result << std::endl;
        return -1;
    }
    double memory_bytes = info.resident_size; 
    double memory_gb = static_cast<double>(memory_bytes) / (1024 * 1024 * 1024);
    return memory_gb;
}


int main(int argc, const char * argv[]) {

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
    auto time_end = chrono::high_resolution_clock::now();
    operationTimes.emplace_back("Read user input", chrono::duration<double>(time_end - time_start).count());

    
    // Read intance File
    time_start = chrono::high_resolution_clock::now();
    Problem P(FilePath, OutputPath, DegreeType, CycleLength, ChainLength, TimeLimit, WeightMatrix, AdjacencyList, Pairs, NDDs, Preference);
    cout << "Reading input graph..." << endl;
    if (P.ReadData() == 0) {
        cout << "Failed while reading input..." << endl;
        return -1;
    }
    time_end = chrono::high_resolution_clock::now();
    operationTimes.emplace_back("Read instance file", chrono::duration<double>(time_end - time_start).count());

    
    // Confoigure solution mode
    time_start = chrono::high_resolution_clock::now();
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
    time_end = chrono::high_resolution_clock::now();
    operationTimes.emplace_back("Configure solution mode", chrono::duration<double>(time_end - time_start).count());

    
    // Apply MDD 
    time_start = std::chrono::high_resolution_clock::now();
    P.BuildMDDs();
    time_end = chrono::high_resolution_clock::now();
    operationTimes.emplace_back("Build MDDs", chrono::duration<double>(time_end - time_start).count());
    
    // Apply BBTree
    time_start = chrono::high_resolution_clock::now();
    P.BBTree();
    time_end = chrono::high_resolution_clock::now();
    operationTimes.emplace_back("Apply BBTree", chrono::duration<double>(time_end - time_start).count());
    
    auto time_end_global = chrono::high_resolution_clock::now();
    double time_global{std::chrono::duration<double>(time_end_global - time_start_global).count()};
    cout << endl << "End \n" <<endl;

    cout << endl << " ##### Analysis ##### \n" <<endl;

    // Timings 
    std::cout << "Timings" << std::endl;
    cout << "Total time: " << time_global << "\n" <<endl;
    int step = 0;
    cout << setw(10) << "Step" << setw(25) << "Task" << setw(25) << "Time (seconds)" << setw(25) << "Time (%)" << '\n';
    for (const auto& entry : operationTimes) {
        cout << setw(10) << step << setw(25) << get<0>(entry) << setw(25) << get<1>(entry) << setw(25) << get<1>(entry)/time_global <<'\n';
        ++step;
    }
    std::cout << "\n" << std::endl;

    // Locations
    std::cout << "Location: " << __FILE__  << std::endl;
    std::cout << "Line: " << __LINE__ << std::endl;
    std::cout << "Function: " << __FUNCTION__  << std::endl; 
    std::cout << "\n" << std::endl;

    // Memory usage
    double memoryUsage = measureMemoryUsage();
    std::cout << memoryUsage << " GB" << std::endl;


    return 0;
}
