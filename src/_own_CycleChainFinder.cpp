// CycleFinder.cpp
#include "_own_CycleChainFinder.hpp"
#include "_own_Utility.hpp"
#include <algorithm>
#include <iostream>


// Constructor
CycleChainFinder::CycleChainFinder(const vector<vector<int>>& adjacencyList,  
                                    const vector<vector<int>>& predList,  
                                    const int& k,
                                    const int& l) : _AdjacencyList(adjacencyList), _PredList(predList), _K(k), _L(l) {}


void CycleChainFinder::findPDPs(){
    cout << ""<<endl;
}

void CycleChainFinder::findNDDs(){
    cout << ""<<endl;
}


void CycleChainFinder::findChains(){
    cout << ""<<endl;
}

void CycleChainFinder::findCycles() {
    cycles.clear();
    int numNodes = _AdjacencyList.size();

    //TODO: make more efficient
    for (int node = 0; node < numNodes; ++node) {
        set<int> visited;
        vector<int> stack;
        cycle_dfs(node, stack, visited);
    }

    cycles = extractUniques("cycle");
}

void CycleChainFinder::cycle_dfs(int currentNode, vector<int>& stack, set<int>& visited) {

    // neigbors of current node 
    vector<int> neighbors;
    for (int j = 0; j < _AdjacencyList[currentNode].size(); ++j) {
        neighbors.push_back(static_cast<int>(_AdjacencyList[currentNode][j]));
    }
    
    std::vector<int> visited_vec(visited.begin(), visited.end());

    // Check if currentNode is already in the stack to detect a cycle
    auto it = find(stack.begin(), stack.end(), currentNode);

    if (it != stack.end()) {
        // Cycle detected; add it to cycles
        vector<int> cycle(it, stack.end());
        if (cycle.size() <= _K) {
            cycles.push_back(cycle);
        }
        return;
    }

    // Mark current node as visited and add it to the stack
    visited.insert(currentNode);
    stack.push_back(currentNode);

    // Traverse all neighbors of currentNode
    for (int j = 0; j < _AdjacencyList[currentNode].size(); ++j) {
        int neighbor = static_cast<int>(_AdjacencyList[currentNode][j]);
        if (visited.find(neighbor) == visited.end()) {
            cycle_dfs(neighbor, stack, visited);
        } else {
            // Check if the neighbor is part of the current stack (cycle)
            auto it = find(stack.begin(), stack.end(), neighbor);
            if (it != stack.end()) {
                vector<int> cycle(it, stack.end());
                if (cycle.size() <= _K) {
                    cycles.push_back(cycle);
                }
            }
        }
    }

    // Backtrack: remove currentNode from stack and visited set
    stack.pop_back();
    visited.erase(currentNode);
}





vector<vector<int>> CycleChainFinder::extractUniques(const string& type){
    set<vector<int>> uniques;
    if (type == "cycle"){
        for (const auto& cycle : cycles) {
            vector<int> sorted_cycle = sortVector(cycle); 
            uniques.insert(sorted_cycle);  
        }
    }else{
        for (const auto& chain : chains) {
            vector<int> sorted_chain = sortVector(chain); 
            uniques.insert(sorted_chain);  
        }
    }
    vector<vector<int>> unqiues_vec (uniques.begin(), uniques.end());
    return unqiues_vec;
}

