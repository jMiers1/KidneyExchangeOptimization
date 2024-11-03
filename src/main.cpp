//
//  main.cpp
//
//  Created by Carolina Riascos Alvarez on 2019-11-27.
//  Copyright © 2019 Carolina Riascos Alvarez. All rights reserved.
//


#include <chrono>
#include <mach/mach.h>
#include "main_VFS.hpp"
#include "BBTree.hpp"
#include "OperationTracer.hpp"



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

    
    // Start measurements
    initializeTraces();
    auto prevSectionEnd{std::chrono::high_resolution_clock::now()};


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
    prevSectionEnd = logTraces("Read user input", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);

    
    Problem P(FilePath, OutputPath, DegreeType, CycleLength, ChainLength, TimeLimit, WeightMatrix, AdjacencyList, Pairs, NDDs, Preference);
    cout << "Reading input graph..." << endl;
    if (P.ReadData() == 0) {
        cout << "Failed while reading input..." << endl;
        return -1;
    }
    prevSectionEnd = logTraces("Read instance file", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);
   

    
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
    prevSectionEnd = logTraces("Configure solution mode", "", prevSectionEnd,  __FILE__, __FUNCTION__, __LINE__);
    
    // Apply MDD 
    P.BuildMDDs();
    prevSectionEnd = logTraces("Apply MDD", "", prevSectionEnd,  __FILE__, __FUNCTION__, __LINE__);
    
    // Apply BBTree
    P.BBTree();
    prevSectionEnd = logTraces("Apply BBTree", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);
    
    cout << endl << "End \n" <<endl;


    //Traces
    printTraces();

    return 0;
}
