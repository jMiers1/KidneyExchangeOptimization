// CycleFinder.hpp
#ifndef CYCLEFINDER_HPP
#define CYCLEFINDER_HPP

#include <ilcplex/ilocplex.h>
#include <vector>
#include <set>

using namespace std;

ILOSTLBEGIN

class CycleFinder {

private:
    void dfs(int currentNode, vector<int>& stack, set<int>& visited);

public:

    // input 
    IloEnv _env;
    IloNumArray2 _AdjacencyList; 
    int _K{99};

    // output
    vector<vector<int>> cycles{{}};

    //constructor
    CycleFinder(IloEnv& env, 
                const IloNumArray2& adjacencyList, 
                const int& k);

    void findCycles();
};

#endif // CYCLEFINDER_HPP
