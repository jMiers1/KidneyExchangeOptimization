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
    void mapCycleAndChainWeights();
    void printResults();



public:

    // input 
    vector<vector<int>> _AdjacencyList; 
    vector<vector<int>> _PredList; 
    map<pair<int,int>,double> _Weights;
    vector<int> _PDPs;
    vector<int> _NDDs;
    int _numChains{-1};
    int _numCycles{-1};
    map<int,pair<vector<int>,vector<int>>> mapNodes; //for ech nodes the indecies of the chains and cycles containing that node
    map<int,double> _cycleWeights; // for each cycle (repesented by its index in cycles) the total value of all contained edges 
    map<int,double> _chainWeights;
    int _K{-1};
    int _L{-1};

    // output
    vector<vector<int>> cycles{{}};
    vector<vector<int>> chains{{}};

    //constructor
    CycleChainFinder(const vector<vector<int>>& adjacencyList,  
                    const vector<vector<int>>& predList, 
                    const map<pair<int,int>,double>& _weights, 
                    const int& k,
                    const int& l);

};

#endif // CycleChainFinder_HPP
