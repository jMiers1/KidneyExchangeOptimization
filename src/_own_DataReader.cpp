// DataReader.cpp

#include "_own_DataReader.hpp"
#include <iostream>
#include <fstream>

using namespace std;


// Constructor
DataReader::DataReader(const std::string& path,  IloEnv& env) : _filePath(path), _env(env), _AdjacencyList(env), _WeightMatrix(env), _PairsType(env) {}


// readFile 
int DataReader::readFile() {
    cout << "Reading file: " << _filePath << endl;

    // check if valid file
    ifstream inFile(_filePath, ifstream::in);
    if (!inFile) {
        this -> print("File does not exist");
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

    _PairsType = IloNumArray(_env);
    inFile >> _Nodes >> _NDDs >> _Pairs >> _NumArcs >> _AdjacencyList >> _WeightMatrix >> _PairsType;



    //Build Predeccessors List
    _PredList = vector<vector<int>>(_AdjacencyList.getSize());
    for (int i = 0; i < _AdjacencyList.getSize(); i++){
        for (int j = 0; j < _AdjacencyList.getSize(); j++){
            if (i != j){
                for (int h = 0; h < _AdjacencyList[j].getSize(); h++){
                    if (_AdjacencyList[j][h] == i + 1){
                        _PredList[i].push_back(j);
                    }
                }
            }
        }
    }

    //Build weights matrix
    for (int i = 0; i < _AdjacencyList.getSize(); i++){
        for (int j = 0; j < _AdjacencyList[i].getSize(); j++){
            _Weights[make_pair(i,_AdjacencyList[i][j] - 1)] = _WeightMatrix[i][j];
        }
    }

    return 0;
}

void DataReader::print(const string& content){
    cout << "--- Data Reader: " << content <<endl;
}
