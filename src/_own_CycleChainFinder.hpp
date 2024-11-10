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
    void chain_dfs(int currentNode, vector<int>& stack, set<int>& visited, int depth);

public:

    // input 
    vector<vector<int>> _AdjacencyList; 
    vector<vector<int>> _PredList; 
    vector<int> _PDPs;
    vector<int> _NDDs;
    int _K{99};
    int _L{99};

    // output
    vector<vector<int>> cycles{{}};
    vector<vector<int>> chains{{}};

    //constructor
    CycleChainFinder(const vector<vector<int>>& adjacencyList,  
                    const vector<vector<int>>& predList,  
                    const int& k,
                    const int& l);

    void findCycles();
    void findChains();
    void separateNodeSet();
    void extractUniques(const string& type);
};

#endif // CycleChainFinder_HPP
