// DataReader.cpp

#include "_own_DataReader.hpp"
#include "_own_Utility.hpp"
#include "_own_Logger.hpp"


#include <iostream>
#include <fstream>

using namespace std;


// Constructor
DataReader::DataReader(const std::string& path,  IloEnv& env) : _filePath(path), _env(env), _AdjacencyList_ILO(env), _Weights_ILO(env), _PairsType(env) {
        readFile();   // execute reading function at instanciation 
        prevSectionEnd = logging("OWN || Read Data", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);   // logging the time requirement (it is small)
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
    string new_name = _filePath; 
    long pos_str = -1;
    long size_str = _filePath.size();
    while(pos_str <= size_str){
        new_name = new_name.substr(pos_str + 1);
        pos_str = new_name.find("/");
        if (pos_str < 0) break;
    }
    _fileName = new_name;

    _PairsType = IloNumArray(_env);
    inFile >> _Nodes >> _NDDs >> _Pairs >> _NumArcs >> _AdjacencyList_ILO >> _Weights_ILO >> _PairsType;

    /*
    Example: 
        16	0	16	59         // there are 16 nodes, 0 NDD-nodes, 16 Pair-nodes, 59 arcs
        [[5,	6],      
        [12,	11,	1,	3],
        [5,	8],
        [11,	3,	1],
        [],
        [11,	1,	3],
        [1,	3,	12,	11],
        [3,	1,	11],
        [12,	3,	11,	1],
        [11,	2,	8,	1,	9,	15,	14,	12,	3,	5,	16],
        [],
        [1,	3,	11],
        [9,	11,	14,	3,	2,	15,	16,	8,	7,	5],
        [1,	11,	12,	3],
        [11,	3,	1],
        [8,	5,	6]]            // until here is the _AdjacencyList_ILO


        [[1,	1],
        [1,	1,	1,	1],
        [1,	1],
        [1,	1,	1],
        [],
        [1,	1,	1],
        [1,	1,	1,	1],
        [1,	1,	1],
        [1,	1,	1,	1],
        [1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1],
        [],
        [1,	1,	1],
        [1,	1,	1,	1,	1,	1,	1,	1,	1,	1],
        [1,	1,	1,	1],
        [1,	1,	1],
        [1,	1,	1]]            // until here is the _Weights_ILO


        //  _PairsType is not given here 
    */

    // Reduce elements in _AdjacencyList by 1 to account for shift from data source (in Library, first node is assumed to be indexed by 1, we start with 0)
    for (int i = 0; i < _AdjacencyList_ILO.getSize(); ++i) {
        for (int j = 0; j < _AdjacencyList_ILO[i].getSize(); ++j) {
            _AdjacencyList_ILO[i][j] -= 1; 
        }
    }

    _AdjacencyList = convertIloNumArray2To2DArray(_AdjacencyList_ILO);
    _PredList = buildPredecessorList(_AdjacencyList);

    //Build weights matrix: Instead of two separate data structures have a single dictionary mapping an arc to its weight in the optimization for easy look-up 
    for (int i = 0; i < _AdjacencyList.size(); i++){
        for (int j = 0; j < _AdjacencyList[i].size(); j++){
            _Weights[make_pair(i,_AdjacencyList[i][j])] = _Weights_ILO[i][j];
        }
    }
    return 0;
}