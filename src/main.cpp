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
#include <vector>
#include "Logger.hpp"
#include "KidneyModel.hpp"


int main(int argc, const char * argv[]) {

    // std::vector<std::vector<int>> adjacencyListData = {
    //         {2, 4},
    //         {0, 2},
    //         {0, 1, 4},
    //         {0, 1},
    //         {}
    //     };

       
    // IloNumArray2 AdjacencyList_Own(env, adjacencyListData.size());
    // for (size_t i = 0; i < adjacencyListData.size(); ++i) {
    //     IloNumArray row(env, adjacencyListData[i].size());
    //     for (size_t j = 0; j < adjacencyListData[i].size(); ++j) {
    //         row[j] = adjacencyListData[i][j];
    //     }
    //     AdjacencyList_Own[i] = row;
    // }

    // vector<vector<int>> PredList_Own= {
    //     {1,2,3},
    //     {2,3},
    //     {1},
    //     {},
    //     {0,2}
    // };

    // std::map<std::pair<int, int>, double> Weights_Own;
    // for (int i = 0; i < AdjacencyList_Own.getSize(); ++i) {
    //     for (int j = 0; j < AdjacencyList_Own[i].getSize(); j++) {
    //         Weights_Own[{i, j}] = 1;
    //     }
    // };

    // int K = 2;
    // int L = 1; 

    cout << "Created dummy params" <<endl; 

    //KidneyExchangeModel PC_TSP (AdjacencyList_Own, PredList_Own, Weights_Own, K, L);

    cout << "Created Kidney Model" <<endl; 

    return 0;


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


    //Traces
    //finishLogging();
    return 0;
}
