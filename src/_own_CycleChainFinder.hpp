// CycleChainFinder_HPP
#ifndef CycleChainFinder_HPP
#define CycleChainFinder_HPP

#include <ilcplex/ilocplex.h>
#include <vector>
#include <set>
#include <map>

using namespace std;

ILOSTLBEGIN

class CycleChainFinder {

private:
    void cycle_depth_first_search(int currentNode, vector<int>& stack, set<int>& visited);
    void chain_breadth_first_serach(int currentNode);
    void findCycles();
    void findChains();
    void separateNodeSet();
    void extractUniques(const string& type);
    void mapNodestoCyclesAndChains();
    void printResults();


public:

    // input 
    vector<vector<int>> _AdjacencyList; 
    vector<vector<int>> _PredList; 
    vector<int> _PDPs;
    vector<int> _NDDs;
    map<int,pair<vector<int>,vector<int>>> mapNodes; //for ech nodes the chains and cycles containing that node
    int _K{-1};
    int _L{-1};

    // output
    vector<vector<int>> cycles{{}};
    vector<vector<int>> chains{{}};

    //constructor
    CycleChainFinder(const vector<vector<int>>& adjacencyList,  
                    const vector<vector<int>>& predList,  
                    const int& k,
                    const int& l);

};

#endif // CycleChainFinder_HPP
