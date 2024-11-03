//
//  main.cpp
//
//  Created by Carolina Riascos Alvarez on 2019-11-27.
//  Copyright © 2019 Carolina Riascos Alvarez. All rights reserved.
//

#include <mach/mach.h>
#include "main_VFS.hpp"
#include <chrono>
#include "OperationTracer.hpp"
#include "BBTree.hpp"
#include <thread> 


std::chrono::time_point<std::chrono::high_resolution_clock> logFunction(const std::string& description, 
                                                                        const std::string& extraInfo,
                                                                        std::chrono::time_point<std::chrono::high_resolution_clock> start, 
                                                                        std::chrono::time_point<std::chrono::high_resolution_clock> end,
                                                                        const std::string& file, 
                                                                        const std::string& function,
                                                                        int line) {
    // File Name
    std::string filename = std::filesystem::path(file).filename().string();
    
    // Time Usage
    double timeUsage = std::chrono::duration<double>(end - start).count();

    // Memory Usage
    task_basic_info info;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT; 
    kern_return_t result = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &count);
    double memoryUsage = 0.0;
    if (result != KERN_SUCCESS) {
        std::cerr << "Error retrieving memory info: " << result << std::endl;
    } else {
        memoryUsage = info.resident_size / (1024.0 * 1024.0);  // Convert to MB
    }

    // CPU Usage
    double CPUUsage = 1.0; // todo

    // Log entry
    operationTrace.emplace_back(description, filename, line, function, timeUsage, memoryUsage, CPUUsage, extraInfo);

    return chrono::high_resolution_clock::now();
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

    
    auto start = chrono::high_resolution_clock::now();
    auto start_global = chrono::high_resolution_clock::now();
    auto prevSectionEnd = chrono::high_resolution_clock::now();

    // Read user input
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
    prevSectionEnd = logFunction("Read user input", "", start, chrono::high_resolution_clock::now(), __FILE__, __FUNCTION__, __LINE__);

    
    Problem P(FilePath, OutputPath, DegreeType, CycleLength, ChainLength, TimeLimit, WeightMatrix, AdjacencyList, Pairs, NDDs, Preference);
    cout << "Reading input graph..." << endl;
    if (P.ReadData() == 0) {
        cout << "Failed while reading input..." << endl;
        return -1;
    }
    prevSectionEnd = logFunction("Read instance file", "", prevSectionEnd, chrono::high_resolution_clock::now(), __FILE__, __FUNCTION__, __LINE__);
   

    
    // Configure solution mode
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
    prevSectionEnd = logFunction("Configure solution mode", "", prevSectionEnd, chrono::high_resolution_clock::now(), __FILE__, __FUNCTION__, __LINE__);
    
    // Apply MDD 
    P.BuildMDDs();
    prevSectionEnd = logFunction("Apply MDD", "", prevSectionEnd, chrono::high_resolution_clock::now(), __FILE__, __FUNCTION__, __LINE__);
    
    // Apply BBTree
    P.BBTree();
    prevSectionEnd = logFunction("Apply BBTree", "", prevSectionEnd, chrono::high_resolution_clock::now(), __FILE__, __FUNCTION__, __LINE__);
    
    
    cout << endl << "End \n" <<endl;

    cout << endl << " ##### Analysis ##### \n" <<endl;
    auto end_global = chrono::high_resolution_clock::now();
    double time_global{std::chrono::duration<double>(end_global - start_global).count()};

    //Traces
    std::cout << "Traces" << std::endl;
    int step = 0;
    int colWidth{20};
    cout << setw(colWidth-15) << "Step" << setw(colWidth+5) << "Task" << setw(colWidth) << "File" << setw(colWidth) << "Line" << setw(colWidth) << "Function" << setw(colWidth) << "Time(s)" << setw(colWidth) << "Memory(b)" << setw(colWidth) << "CPU"<< setw(colWidth) << "Comment" << '\n';
    for (const auto& entry : operationTrace) {
        cout << setw(colWidth-15) << step << setw(colWidth+5) << get<0>(entry) << setw(colWidth) << get<1>(entry) << setw(colWidth) << get<2>(entry) << setw(colWidth) << get<3>(entry) << setw(colWidth) << get<4>(entry) << setw(colWidth) << get<5>(entry) << setw(colWidth) << get<6>(entry) << setw(colWidth) << get<7>(entry) << '\n';
        ++step;
    }
    std::cout << "\n" << std::endl;

    return 0;
}
