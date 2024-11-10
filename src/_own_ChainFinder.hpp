// ChainFinder.hpp
#ifndef CHAINFINDER_HPP
#define CHAINFINDER_HPP

#include <ilcplex/ilocplex.h>
#include <vector>
#include <set>

using namespace std;

ILOSTLBEGIN

class ChainFinder {

private:
    void dfs(int currentNode, vector<int>& stack, set<int>& visited);

public:

    // input 
    IloEnv _env;
    IloNumArray2 _AdjacencyList; 
    int _L{99};

    // output
    vector<vector<int>> chains{{}};

    //constructor
    ChainFinder(IloEnv& env, 
                const IloNumArray2& adjacencyList, 
                const int& l);

    void findChains();
    void printAdjacencyList();
    //void printAllCycles();
    vector<int> normalizeChains(const vector<int>& cycle);
    string printVector(const std::vector<int>& stack);
};

#endif // CHAINFINDER_HPP
