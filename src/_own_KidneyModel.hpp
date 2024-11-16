#ifndef KIDNEYEXCHANGE_HPP
#define KIDNEYEXCHANGE_HPP

#include <iostream>
#include <vector>
#include <map>
#include <ilcplex/ilocplex.h>

using namespace std;

class KidneyModel {
public:
    KidneyModel(IloEnv& _env,
                const vector<vector<int>>& _cycles, 
                const vector<vector<int>>& _chains, 
                const map<pair<int,int>,double>& _weights, 
                const map<int,pair<vector<int>,vector<int>>>& _mapNodes,
                const map<int,double>& _chainWeights,
                const map<int,double>& _cycleWeights,
                const vector<int>& _ndds,
                const vector<int>& _pdps,
                const vector<vector<int>>& adjacencyList,  
                const vector<vector<int>>& predList);
    double solvePatternFormulation();
    void printSolutionStructure(const string& structure_type, const int& structure);
    void printSolution(const double& obj, const IloCplex& cplex, const IloBoolVarArray& x_cycle, const IloBoolVarArray& x_chain);
    void printObjectiveFunction();

    void solvePctspFormulation();

private:
    IloEnv env;
    vector<vector<int>> _Cycles;
    vector<vector<int>> _Chains; 
    map<int,double> _chainWeights;
    map<int,double> _cycleWeights;
    map<pair<int,int>,double> _Weights;
    map<int,pair<vector<int>,vector<int>>> _mapNodes;
    vector<int> _NDDs;
    vector<int> _PDPs;
    int _numCycles{-99};
    int _numChains{-99};
    int _numEdges{-99};
    int _numNodes{-99};
    vector<vector<int>> _AdjacencyList;
    vector<vector<int>> _PredList;
};

#endif // KIDNEYEXCHANGE_HPP