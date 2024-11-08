//
//  ReadData.cpp
//
//  Created by Carolina Riascos Alvarez on 2019-11-27.
//  Copyright © 2019 Carolina Riascos Alvarez. All rights reserved.
//


#include "ReadData.hpp"
//Implementations

int Problem::ReadData() {
    
    // Check that instance file is valid 
    ifstream inFile(FilePath, ifstream::in);
    if (!inFile) {
        cout << endl << "Instance's files not found. " << FilePath << endl;
        return 0;
    }

    //Extrcat the file name (i.e. remove the path)
    string new_name = FilePath; long pos_str = -1; long size_str = FilePath.size();
    while(pos_str <= size_str){
        new_name = new_name.substr(pos_str + 1);
        pos_str = new_name.find("/");
        if (pos_str < 0) break;
    }
    FileName = new_name;


    //------ Preparations 
    
    //Read Data
    PairsType = IloNumArray(env);
    inFile >> Nodes >> NDDs >> Pairs >> NumArcs >> AdjacencyList >> WeightMatrix >> PairsType;
    
    
    //Build Predeccessors List
    PredList = vector<vector<int>>(AdjacencyList.getSize());
    for (int i = 0; i < AdjacencyList.getSize(); i++){
        for (int j = 0; j < AdjacencyList.getSize(); j++){
            if (i != j){
                for (int h = 0; h < AdjacencyList[j].getSize(); h++){
                    if (AdjacencyList[j][h] == i + 1){
                        PredList[i].push_back(j);
                    }
                }
            }
        }
    }

    vector<vector<int>> NewPredList;
    NewPredList = vector<vector<int>>(AdjacencyList.getSize());
    for (int i = 0; i < AdjacencyList.getSize(); i++) {
        for (int j = 0; j < AdjacencyList.getSize(); j++) {
            if (i != j) {  // Skip self-loops
                for (int h = 0; h < AdjacencyList[j].getSize(); h++) {
                    // Compare the destination of node j's edges to node i
                    if (AdjacencyList[j][h] == i) {
                        NewPredList[i].push_back(j);  // Add node j as a predecessor of node i
                    }
                }
            }
        }
    }


    // Printing AdjacencyList
    for (size_t i = 0; i < AdjacencyList.getSize(); ++i) {
        std::cout << "AdjacencyList[" << i << "]: ";
        for (size_t j = 0; j < AdjacencyList[i].getSize(); ++j) {
            std::cout << AdjacencyList[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n"<< std::endl;
    

    // Print PredList
    for (size_t i = 0; i < PredList.size(); ++i) {
        std::cout << "PredList[" << i << "]: ";
        for (size_t j = 0; j < PredList[i].size(); ++j) {
            std::cout << PredList[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n"<< std::endl;

    // Printing NewPredList
    for (size_t i = 0; i < NewPredList.size(); ++i) {
        std::cout << "NewPredList[" << i << "]: ";
        for (size_t j = 0; j < NewPredList[i].size(); ++j) {
            std::cout << NewPredList[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n"<< std::endl;



    
    //Build weights matrix
    for (int i = 0; i < AdjacencyList.getSize(); i++){
        for (int j = 0; j < AdjacencyList[i].getSize(); j++){
            Weights[make_pair(i,AdjacencyList[i][j] - 1)] = WeightMatrix[i][j];
        }
    }
    
    int pimany = 0;
    if (ChainLength == 0){
        pimany = int(Pairs);
    }
    else{
        pimany = int(Nodes);
    }
    solpi = IloNumArray(env, pimany);
    
    //To find positive-priced cycles on the go
    for (int i = 0; i < Pairs; i++){
        for (int j = 0; j < AdjacencyList[i].getSize(); j++){
            ArcsinSol.push_back(CheckedArc(i,j,0));
        }
    }

    //To find positive-priced chains on the go
    for (int i = AdjacencyList.getSize() - 1; i >= 0 ; i--){
        VertexinSolChain.push_back(vChain(i, vector<int>()));
    }
    //Check whether arc weights are integers
    AllArcWeightsInt = true;
    int number = 0;
    
    map<pair<int,int>,double>::iterator it = Weights.begin();
    for (it; it != Weights.end(); it++){
        number = floor(it->second);
        if (number != it->second){
            AllArcWeightsInt = false;
            break;
        }
    }
    
    z_sol = IloNumArray(this->env);
    
    return 1;
}
