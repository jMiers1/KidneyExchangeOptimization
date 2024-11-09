// DataReader.hpp

#ifndef DATAREADER_HPP
#define DATAREADER_HPP

#include <ilcplex/ilocplex.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>

class DataReader {
public:
    DataReader(const std::string& filePath, IloEnv& environment, int chainLength)
        : FilePath(filePath), env(environment), ChainLength(chainLength) {}

    virtual ~DataReader() {}  // Virtual destructor for polymorphic use

    int ReadData();  // Shared ReadData function

protected:
    // Common data members used by ReadData
    std::string FilePath;
    IloEnv& env;
    int ChainLength;
    int Nodes, NDDs, Pairs, NumArcs;

    IloNumArray2 AdjacencyList, WeightMatrix;
    IloNumArray PairsType, solpi, z_sol;
    std::vector<std::vector<int>> PredList;
    std::map<std::pair<int, int>, double> Weights;
    std::vector<CheckedArc> ArcsinSol;
    std::vector<vChain> VertexinSolChain;
    bool AllArcWeightsInt;
};

#endif // DATAREADER_HPP
