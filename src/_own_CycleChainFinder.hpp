// CycleChainFinder_HPP
#ifndef CycleChainFinder_HPP
#define CycleChainFinder_HPP

#include <ilcplex/ilocplex.h>
#include <vector>
#include <set>

using namespace std;

ILOSTLBEGIN

class CycleChainFinder {

private:
    void cycle_dfs(int currentNode, vector<int>& stack, set<int>& visited);
    void chain_dfs(int currentNode, vector<int>& stack, set<int>& visited);

public:

    // input 
    IloEnv _env;
    IloNumArray2 _AdjacencyList; 
    int _K{99};
    int _L{99};

    // output
    vector<vector<int>> cycles{{}};
    vector<vector<int>> chains{{}};

    //constructor
    CycleChainFinder(IloEnv& env, 
                    const IloNumArray2& adjacencyList, 
                    const int& k,
                    const int& l);

    void findCycles();
    void findChains();
    void extractUnique(const string& type);
    void printAll();

    vector<int> normalizeCycle(const vector<int>& cycle);
    vector<int> sortVector(const vector<int>& cycle);
    string printVector(const std::vector<int>& stack);
    void printAdjacencyList();

};

#endif // CycleChainFinder_HPP
