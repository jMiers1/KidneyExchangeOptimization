// CycleChainFinder_HPP
#ifndef CycleChainFinder_HPP
#define CycleChainFinder_HPP

#include <ilcplex/ilocplex.h>
#include <vector>
#include <set>
#include <map>

using namespace std;

enum VisitState { UNVISITED, VISITING, VISITED, NO_OUTGOING};


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
    void findCyclesChains();
    void dfs(int focal_node, vector<VisitState> visited, vector<int> parent, int current_depth,  bool chains_allowed, bool cycles_allowed);
    void trackNodeMap(const string& structure_type, const vector<int>& structure);
    void trackWeightMap(const string& structure_type, const vector<int>& structure);
    bool isNew(const std::string& structure_type, const vector<int>& structure);
    void processLeaveNode(const std::string& structure_type, const int& focal_node, const int& child, const vector<int>& parent);

public:

    // input 
    vector<vector<int>> _AdjacencyList; 
    vector<vector<int>> _PredList; 
    map<pair<int,int>,double> _Weights;
    vector<int> _PDPs;
    vector<int> _NDDs;
    int _numChains{-1};
    int _numCycles{-1};
    map<int,pair<vector<int>,vector<int>>> mapNodes; //for each node the indecies of the chains and cycles containing that node
    map<int,double> _cycleWeights; // for each cycle (repesented by its index in cycles) the total value of all contained edges 
    map<int,double> _chainWeights;
    int _maxCycleLength{-1};
    int _maxChainLength{-1};
    bool ndd_cycles;


    // output
    vector<vector<int>> cycles{{}};
    vector<vector<int>> chains{{}};

    set<vector<int>> unique_cycles{{}};
    set<vector<int>> unique_chains{{}};


    //constructor
    CycleChainFinder(const vector<vector<int>>& adjacencyList,  
                    const vector<vector<int>>& predList, 
                    const map<pair<int,int>,double>& _weights, 
                    const int& k,
                    const int& l);

};


vector<int> traceBack(const int& focal_node, const int& go_back_to_node, const vector<int>& parent);

#endif // CycleChainFinder_HPP
