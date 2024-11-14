//
//  main.cpp
//
//  Created by Carolina Riascos Alvarez on 2019-11-27.
//  Copyright © 2019 Carolina Riascos Alvarez. All rights reserved.
//


#include <chrono>
#include <mach/mach.h>
#include <unordered_map>
//#include <boost/functional/hash.hpp>
#include <map>
#include "main_VFS.hpp"
#include "BBTree.hpp"
#include <vector>
#include "_own_CycleChainFinder.hpp"
#include "_own_Logger.hpp"
#include "_own_KidneyModel.hpp"
#include "_own_DataReader.hpp"

#include "_own_Utility.hpp"


int main(int argc, const char * argv[]) {

    if (argc < 8) {
        cout << "The program expects 7 additional rguments" << endl;
        cout << "Received: \n";
        for(int i{0}; i < argc; i++) {
            cout << "Argument " << i << ": " << std::string{argv[i]} << "\n";
        }
        
        return -1;
    }
    
    // Start measurements
    startLogging(argv);


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
    prevSectionEnd = logging("Read user input", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);




    // Own     
    cout << "### Start Own Model ### \n \n" <<endl; 

    prevSectionEnd = logging("Own: Start", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);
    IloEnv _env;
    DataReader reader(FilePath, _env);
    prevSectionEnd = logging("Own: Read Data", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);

    reader._AdjacencyList = {{1,2},{2},{3,4,5},{4},{3},{}};
    reader._PredList = {{},{0},{0,1},{2,4},{2,3},{2}};

    print2DArray(reader._AdjacencyList);

    CycleChainFinder finder(reader._AdjacencyList, reader._PredList, reader._Weights, CycleLength, ChainLength);
    return 0;
    prevSectionEnd = logging("Own: Found Cycles and Chains", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);
    KidneyModel model (_env, 
                        finder.cycles, 
                        finder.chains, 
                        reader._Weights, 
                        finder.mapNodes, 
                        finder._chainWeights,
                        finder._cycleWeights,
                        finder._NDDs, 
                        finder._PDPs);
    prevSectionEnd = logging("Own: Solved Model", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);

    cout << "\n \n ### End Own Model ### \n \n" <<endl; 


    // Default
    cout << "### Start Default> Model ### \n \n" <<endl; 
    Problem P(FilePath, OutputPath, DegreeType, CycleLength, ChainLength, TimeLimit, WeightMatrix, AdjacencyList, Pairs, NDDs, Preference);
    cout << "Reading input graph..." << endl;
    if (P.ReadData() == 0) {
        cout << "Failed while reading input..." << endl;
        return -1;
    }

    prevSectionEnd = logging("Read instance file", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);

    
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
    prevSectionEnd = logging("Configure solution mode", "", prevSectionEnd,  __FILE__, __FUNCTION__, __LINE__);
    
    // Apply MDD 
    P.BuildMDDs();
    
    // Apply BBTree
    P.BBTree();
    
    cout << endl << "End \n" <<endl;


    //Logging
    finishLogging();
    return 0;
}
