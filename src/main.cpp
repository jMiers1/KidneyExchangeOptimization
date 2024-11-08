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
#include "Logger.hpp"
#include "KIDNEY_EXCHANGE_MODEL.hpp"





int main(int argc, const char * argv[]) {
    try {
        // Initialize CPLEX environment
        IloEnv env;

        // Define the AdjacencyList, PredList, and Weights
        std::vector<std::vector<int>> AdjacencyList = {
            {1, 2},      // Pair 0 can donate to pairs 1 and 2
            {2},         // Pair 1 can donate to pair 2
            {}           // Pair 2 has no donation possibilities
        };

        std::vector<std::vector<int>> PredList = {
            {},          // Pair 0 has no incoming donors
            {0},         // Pair 1 can receive from pair 0
            {0, 1}       // Pair 2 can receive from pairs 0 and 1
        };

        std::map<myEdge, double> Weights = {
            {{0, 1}, 1.0},    // Weight for donation from 0 to 1
            {{0, 2}, 2.0},    // Weight for donation from 0 to 2
            {{1, 2}, 1.5}     // Weight for donation from 1 to 2
        };

        // Instantiate the KidneyExchangeModel with provided data
        KidneyExchangeModel model(env, AdjacencyList, PredList, Weights);

        // Build and solve the model
        model.buildModel();
        model.solve();

        // End the CPLEX environment
        env.end();
    } catch (const IloException& e) {
        std::cerr << "Error: " << e << std::endl;
    } catch (...) {
        std::cerr << "Unknown error." << std::endl;
    }






    if (argc < 8) {
        cout << "The program expects 7 additional arguments" << endl;
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
