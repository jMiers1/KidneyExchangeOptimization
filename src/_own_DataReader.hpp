// DataReader.hpp

#pragma once 

#include <map>
#include <vector>
#include <stdio.h>
#include <string>
#include <ilcplex/ilocplex.h>

using namespace std;

ILOSTLBEGIN

class DataReader {

private:
    // input path 
    string _filePath; 

public:
    IloEnv _env;

    // read this from the file
    IloInt _Nodes{99}; 
    IloInt _NDDs{99};
    IloInt _Pairs{99};
    IloInt _NumArcs{99};
    IloNumArray2 _AdjacencyList; // identical
    IloNumArray2 _WeightMatrix;  
    IloNumArray _PairsType;


    // create this in call
    string _fileName; 
    vector<vector<int>> _PredList;
    map<pair<int,int>,double> _Weights;


    DataReader(const string& path,  IloEnv& env);
    int readFile();
    void print(const string& content);
};