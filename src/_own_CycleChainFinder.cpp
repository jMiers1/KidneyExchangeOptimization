// CycleFinder.cpp
#include "_own_CycleChainFinder.hpp"
#include "_own_Utility.hpp"
#include "_own_Logger.hpp"

#include <algorithm>
#include <stack>
#include <tuple>
#include <map>
#include <queue>
#include <iostream>







// Constructor
CycleChainFinder::CycleChainFinder(const vector<vector<int>>& adjacencyList,  
                                    const vector<vector<int>>& predList,  
                                    const map<pair<int,int>,double>& _weights,
                                    const int& k,
                                    const int& l) : _AdjacencyList(adjacencyList), _PredList(predList), _Weights(_weights), _K(k), _L(l) {
                                    separateNodeSet(); // differentiates node set into NDDs and PDPs
                                    prevSectionEnd = logging("Separate Node Set", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);

                                    findCyclesChains();
                                    print2DArray(chains);

                                    // findCycles();  // finds cycles
                                    // prevSectionEnd = logging("Find cycles", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);

                                    // findChains();  // finds chains
                                    //  prevSectionEnd = logging("Find chains", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);

                                    _numChains = chains.size();
                                    _numCycles = cycles.size();
                                    mapNodestoCyclesAndChains();
                                    mapCycleAndChainWeights();
                                     prevSectionEnd = logging("Other mappings", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);

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

void CycleChainFinder::findCyclesChains(){
   chains.clear();
   cycles.clear();
   int N_nodes = _NDDs.size() + _PDPs.size();
   vector<VisitState> visited(N_nodes, UNVISITED);
   vector<int> parent(N_nodes, -1);
   vector<vector<int>> cycles, chains;

   for (int i = 0; i<N_nodes; i++){
        if (_AdjacencyList[i].size() == 0){
            visited[i] = NO_OUTGOING;
        }
   }

   for (int u : _NDDs){
        cout << "DFS for node " << u <<endl;
        dfs(u = u, visited = visited, parent = parent);
   }
   for (int u : _PDPs){
        cout << "DFS for node " << u <<endl;
        dfs(u = u, visited = visited, parent = parent);
   }
   extractUniques("chains");
}

void CycleChainFinder::dfs(int u, vector<VisitState> visited, vector<int> parent){
    visited[u] = VISITING;

    for (int v : _AdjacencyList[u]) {
        if (visited[v] == UNVISITED) {
            cout << "New parent: "<<v<<endl;
            parent[v] = u;
            dfs(v, visited, parent);
        }
        else if (visited[v] == VISITING) {
            cout << "Found cycle" <<endl;
            vector<int> cycle;
            int current = u;
            while (current != v) {
                cout<< current <<endl;
                cycle.push_back(current);
                current = parent[current];
            }
            cycle.push_back(v);
            reverse(cycle.begin(), cycle.end());
            cycles.push_back(cycle);
        }
        else if (visited[v] == VISITED || visited[v] == NO_OUTGOING) {
            cout <<"Found chain"<<endl;
            vector<int> chain;
            int current = u;
            while (current != -1) {
                chain.push_back(current);
                current = parent[current];
            }
            
            reverse(chain.begin(), chain.end());
            chain.push_back(v);
            chains.push_back(chain);
        }
    }



    visited[u] = VISITED;
}


void CycleChainFinder::findChains(){
    // Chains have to start at an NDD 
    chains.clear();
    for (int node : _NDDs) {
        set<int> visited;
        vector<int> path;
        chain_breadth_first_serach(node);
    }
    extractUniques("chains");
}

void CycleChainFinder::chain_breadth_first_serach(int startNode) {
    // queue: (current node, path, visited set, depth)
    std::queue<std::tuple<int, std::vector<int>, std::set<int>, int>> queue;
    queue.push({startNode, {startNode}, {startNode}, 0}); 

    while (!queue.empty()) {
        auto [currentNode, path, visited, depth] = queue.front();
        queue.pop();
        if (depth == _L) {
            // max depth reached
            chains.push_back(path);  
            continue;                // Move to the next item in the queue
        }
        // Traverse all neighbors of the current node
        for (int successor : _AdjacencyList[currentNode]) {
            if (visited.find(successor) == visited.end()) {  
                // Only process unvisited nodes to avoid cycles
                // Prepare new path and visited set for the successor
                std::vector<int> newPath = path;
                std::set<int> newVisited = visited;
                newPath.push_back(successor);
                newVisited.insert(successor);

                // Enqueue the successor with the new path, visited set, and incremented depth
                queue.push({successor, newPath, newVisited, depth + 1});
            }
        }

        // If no outgoing edges (leaf node or dead-end), add the current path to chains as a terminal path
        if (_AdjacencyList[currentNode].empty()) {
            chains.push_back(path);
        }
    }
}

void CycleChainFinder::findCycles() {

    // Cycles must start at a PDP -> not include NDDs
    cycles.clear();
    for (int node : _PDPs) {
        set<int> visited;
        vector<int> stack;
        cycle_depth_first_search(node, stack, visited);
    }
    extractUniques("cycles");
}

void CycleChainFinder::cycle_depth_first_search(int currentNode, vector<int>& stack, set<int>& visited) {

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
            cycle_depth_first_search(neighbor, stack, visited);
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

void CycleChainFinder::extractUniques(const std::string& type) {
    if (type == "cycles") {
        std::set<std::vector<int>> uniqueCycles_sorted;
        std::set<std::vector<int>> uniqueCycles;
        for (auto& cycle : cycles) {
            vector<int> cycle_sorted = cycle;
            std::sort(cycle_sorted.begin(), cycle_sorted.end()); 
            if (uniqueCycles_sorted.find(cycle_sorted) == uniqueCycles_sorted.end()){
                // cycle not yet in the unique set 
                uniqueCycles.insert(cycle); 
                uniqueCycles_sorted.insert(cycle_sorted);
            }
        }
        cycles.assign(uniqueCycles.begin(), uniqueCycles.end());

    } else if (type == "chains") {
        std::set<std::vector<int>> uniqueChains;
        for (auto& chain : chains) {
            uniqueChains.insert(chain);  
        }
        chains.assign(uniqueChains.begin(), uniqueChains.end());
    }
}

void CycleChainFinder::mapNodestoCyclesAndChains(){
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

void CycleChainFinder::mapCycleAndChainWeights(){
    // Cycles  
    for (size_t i = 0; i < _numCycles; i++){
        vector<int> x = cycles[i];
        int total = 0;
        for (size_t j = 0; j < x.size() - 1; ++j) {
            int start = x[j];
            int end = x[j + 1];
            total +=  _Weights.find({start, end})->second;
        }
        // Close up the cycle: check for the last arc
        auto it = _Weights.find({x[x.size()-1], x[0]});
        total += it->second;
        _cycleWeights[i] = total;
    }
    // Chains
    for (size_t i = 0; i < _numChains; i++){
        vector<int> x = chains[i];
        int total = 0;
        for (size_t j = 0; j < x.size() - 1; ++j) {
            int start = x[j];
            int end = x[j + 1];
            total += _Weights.find({start, end})->second;
        }
        _chainWeights[i] = total; 
    }
}

void CycleChainFinder::printResults(){
    cout <<"\n "<<"Sucessors"<<endl;
    print2DArray(_AdjacencyList);
    cout <<"\n"<<" Predecessors"<<endl;
    print2DArray(_PredList);
    cout <<"\n "<<"NDDs"<<endl;
    printVector(_NDDs);
    cout <<"\n"<<" PDPs"<<endl;
    printVector(_PDPs);
    cout <<"\n "<<"Cycles"<<endl;
    print2DArray(cycles);
    cout <<"\n"<<" Chains"<<endl;
    print2DArray(chains);
    cout <<"\n "<<"Mapped"<<endl;
    print2DMap(mapNodes);
}