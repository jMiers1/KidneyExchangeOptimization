// CycleFinder.cpp
#include "_own_CycleChainFinder.hpp"
#include "_own_Utility.hpp"
#include <algorithm>
#include <iostream>


// Constructor
CycleChainFinder::CycleChainFinder(const vector<vector<int>>& adjacencyList,  
                                    const vector<vector<int>>& predList,  
                                    const int& k,
                                    const int& l) : _AdjacencyList(adjacencyList), _PredList(predList), _K(k), _L(l) {
                                        separateNodeSet();
                                        findCycles();
                                        findChains();
                                        mapNodestoCyclesAndChains();
                                    }


void CycleChainFinder::separateNodeSet(){
    // Separates the node set intop PDPs(have incoming arcs) and NDDs(no incoming arcs)
    vector<int> pdps;
    vector<int> ndds;
    for (int i = 0; i < _PredList.size(); ++i) {
        if (_PredList[i].size() > 0){
            pdps.push_back(i);
            }
        else{
            ndds.push_back(i);
        }
    }
    _PDPs =  pdps;
    _NDDs = ndds;
}

void CycleChainFinder::findChains(){

    // Assumption: Chains have to start at an NDD 

    chains.clear();
    _NDDs = {3};
    for (int node : _NDDs) {
        set<int> visited;
        vector<int> path;
        chain_dfs(node, path, visited, 0);
    }
    extractUniques("chains");
}

void CycleChainFinder::chain_dfs(int currentNode, vector<int>& path, set<int>& visited, int depth){
    path.push_back(currentNode);
    if (depth == _L) {
        chains.push_back(path);  // Add the current path (chain) to the list of chains
        path.pop_back();  // Remove the current node as we're not continuing the path
        return;
    }

    // Traverse all neighbors of currentNode
    cout <<"Current node "<<currentNode<< endl;
    printVector(_AdjacencyList[currentNode]);

    vector<int> successors = _AdjacencyList[currentNode];
    int outDegree = successors.size();
    if (outDegree > 0){
        for (int sucessor : successors){
            if (containedInSet(sucessor, visited)){
                // this is a cycle
                break;
            }else{
                visited.insert(sucessor);
                path.push_back(sucessor);
                chain_dfs(sucessor, path, visited, depth+1);}
        }
    }else{
        
    }

    for (int neighbor : _AdjacencyList[currentNode]) {
        if (!containedInSet(neighbor, visited)) {
            hasOutgoingEdge = true;
            visited.insert(neighbor);
            cout << "New neighbor " <<neighbor<<endl;
            chain_dfs(neighbor, path, visited, depth+1);  // Recursively search for a path
        }else{
            cout <<"cycle"<<endl;
        }
    }

    // If there are no more neighbors (leaf node or dead-end), we store the chain
    if (!hasOutgoingEdge) {
        chains.push_back(path);  // Add the current path (chain) to the list of chains
    }

    // Backtrack: remove currentNode from path and visited set
    path.pop_back();
    visited.erase(currentNode);
}

void CycleChainFinder::findCycles() {

    // Cycles must start at a PDP -> not include NDDs
    cycles.clear();
    for (int node : _PDPs) {
        set<int> visited;
        vector<int> stack;
        cycle_dfs(node, stack, visited);
    }
    extractUniques("cycles");
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

void CycleChainFinder::extractUniques(const string& type) {
    // Helper lambda function to handle the unique extraction process
    auto extractUnique = [this](vector<vector<int>>& container) {
        set<vector<int>> unique_set;
        for (auto& x : container) {
            unique_set.insert(sortVector(x));  // Sort and insert into the set
        }
        container.assign(unique_set.begin(), unique_set.end());  // Assign unique elements back to the container
    };

    if (type == "cycles") {
        extractUnique(cycles);  // Extract unique cycles
    } else if (type == "chains") {
        extractUnique(chains);  // Extract unique chains
    }
}

void CycleChainFinder::mapNodestoCyclesAndChains(){

    int _numChains = chains.size();
    int _numCycles = cycles.size();
    for (int node: _PDPs){
         // can be in cycles and in chains 
        for (int chain_index = 0; chain_index< _numChains; chain_index++){
            const vector<int>& chain = chains[chain_index];
            if (containedInVector(node,chain)){
                mapNodes[node].first.push_back(chain_index);
            }
        }
        for (int cycle_index = 0; cycle_index< _numCycles; cycle_index++){
            const vector<int>& cycle = cycles[cycle_index];
            if (containedInVector(node,cycle)){
                mapNodes[node].second.push_back(cycle_index);
            }
        }
    }
    for (int node: _NDDs){
    // has to be the start of a chain 
        for (int chain_index = 0; chain_index< _numChains; chain_index++){
            const vector<int>& chain = chains[chain_index];
            if (chain.front() == node){
                mapNodes[node].first.push_back(chain_index);
            }
        }
    }
}