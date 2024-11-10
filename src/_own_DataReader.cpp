// DataReader.cpp

#include "_own_DataReader.hpp"
#include "_own_Utility.hpp"
#include <iostream>
#include <fstream>

using namespace std;


// Constructor
DataReader::DataReader(const std::string& path,  IloEnv& env) : _filePath(path), _env(env), _AdjacencyList_ILO(env), _Weights_ILO(env), _PairsType(env) {
        readFile();
}


// readFile 
int DataReader::readFile() {
    
    // check if valid file
    ifstream inFile(_filePath, ifstream::in);
    if (!inFile) {
        return 0;
    }else{
    }

    // extract file name 
    string new_name = _filePath; long pos_str = -1; long size_str = _filePath.size();
    while(pos_str <= size_str){
        new_name = new_name.substr(pos_str + 1);
        pos_str = new_name.find("/");
        if (pos_str < 0) break;
    }
    _fileName = new_name;

    cout << "got name"<<endl;

    _PairsType = IloNumArray(_env);
    inFile >> _Nodes >> _NDDs >> _Pairs >> _NumArcs >> _AdjacencyList_ILO >> _Weights_ILO >> _PairsType;

    // Reduce elemnts in _AdjacencyList by 1 to account for shift from data source
    for (int i = 0; i < _AdjacencyList_ILO.getSize(); ++i) {
        for (int j = 0; j < _AdjacencyList_ILO[i].getSize(); ++j) {
            _AdjacencyList_ILO[i][j] -= 1; 
        }
    }

    _AdjacencyList = convertIloNumArray2To2DArray(_AdjacencyList_ILO);
    _PredList = buildPredecessorList(_AdjacencyList);
    print2DArray(_AdjacencyList);

    //Build weights matrix
    for (int i = 0; i < _AdjacencyList.size(); i++){
        for (int j = 0; j < _AdjacencyList[i].size(); j++){
            _Weights[make_pair(i,_AdjacencyList[i][j] - 1)] = _Weights_ILO[i][j];
        }
    }
    return 0;
}