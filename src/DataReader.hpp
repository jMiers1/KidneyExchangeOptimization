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
   
    // read this from the file
    int _Nodes{0}; 
    int _NDDs{0};
    int _Pairs{0};
    int _NumArcs{0};
    IloNumArray2 _AdjacencyList; // identical
    IloNumArray2 _WeightMatrix;  
    IloNumArray _PairsType;


    // create this in call
    string _fileName; 
    vector<vector<int>> _PredList;
    map<pair<int,int>,double> _Weights;

public:
    // Constructor
    IloEnv _env;
    DataReader(const string& path,  IloEnv& env);

    // Member function declarations
    int readFile();

    void print(const string& content);

    // Getter for filePath
    string getFilePath() const;
};
