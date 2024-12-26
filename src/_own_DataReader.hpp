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
    IloNumArray2 _AdjacencyList_ILO; // _ILO attributes for CPLEX model, without _ILO for own procressing (e.g. _AdjacencyList for cycle and chain finding)
    IloNumArray2 _Weights_ILO;  
    IloNumArray _PairsType;

    vector<vector<int>> _AdjacencyList;
    vector<vector<int>> _PredList;
    map<pair<int,int>,double> _Weights;
    string _fileName; 

    DataReader(const string& path,  IloEnv& env);
    int readFile();
};