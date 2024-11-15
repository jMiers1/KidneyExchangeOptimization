// CycleFinder.cpp
#include "_own_CycleChainFinder.hpp"
#include "_own_Utility.hpp"
#include "_own_Logger.hpp"

#include <algorithm>
#include <stack>
#include <tuple>
#include <map>
#include <queue>
#include <utility>
#include <iostream>
#include <numeric> 



    // reader._AdjacencyList = {{1,2},{2},{3,4,5},{1},{},{6},{}};
    // reader._PredList = {{},{0,3},{0,1},{2},{2},{2},{5}};
    // CycleLength = 2;
    // ChainLength = 4;


// Constructor
CycleChainFinder::CycleChainFinder(const vector<vector<int>>& adjacencyList,  
                                    const vector<vector<int>>& predList,  
                                    const map<pair<int,int>,double>& _weights,
                                    const int& k,
                                    const int& l) : _AdjacencyList(adjacencyList), _PredList(predList), _Weights(_weights), _maxCycleLength(k), _maxChainLength(l) {
                                    separateNodeSet(); // differentiates node set into NDDs and PDPs
                                    prevSectionEnd = logging("Separate Node Set", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);


                                    findCyclesChains();
                                    
                                    cout << "Cycles "<<cycles.size()<<" chains "<<chains.size()<<endl;
                                    write2DArrayToFile(chains, "chains_NDD_cycle_false.txt");
                                    // cout << "chains"<<endl;
                                    // printCycles(chains);

                                    prevSectionEnd = logging("own -- Found cycles & chains", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);
                                  

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
   string mode = (_maxChainLength >= _maxCycleLength) ? "chain_bound" : "cycle_bound";

   for (int u : _NDDs){
        dfs(u, visited, parent, 1, true, true);
   }

   for (int u : _PDPs){
        dfs(u, visited, parent, 1, true, true);
   }
   extractUniques("chains"); // from the set of chains, extracts only the unique ones
   extractUniques("cycles"); // find unique cycles
}

void CycleChainFinder::dfs(int focal_node, vector<VisitState> visited, vector<int> parent, int current_depth, bool chains_allowed, bool cycles_allowed){
    vector<int> chain, cycle;
    visited[focal_node] = VISITING;

    pair<string, int> smaller_bound = (_maxCycleLength < _maxChainLength) ? pair<string, int>("cycle", _maxCycleLength) : pair<string, int>("chain", _maxChainLength);


    if (current_depth < smaller_bound.second){
        ;
    }else{
        if (current_depth > _maxChainLength){
            /* 
            chains can as such not be lengethened further,
            but for the meantime we do, hoping for a cycle. 
            if eventually a cycle is found, then ok. else throw away, but not add the chain  
            */ 
            chains_allowed = false;
        }
        if (current_depth > _maxCycleLength){
            // cycles not allowed anymore
            cycles_allowed = false;
        }
        
    }

    if (_AdjacencyList[focal_node].size() == 0){
        // no childs -> need to stop here 
        if (current_depth <= _maxChainLength){
            chain = traceBack(focal_node, -1, parent);
            chains.push_back(chain);
        }
    }else{
        for (int child : _AdjacencyList[focal_node]) {

            // continue dfs
            if (visited[child] == UNVISITED) {
                parent[child] = focal_node;
                dfs(child, visited, parent,current_depth+1, chains_allowed, cycles_allowed);
            }

            // cycle
            else if (visited[child] == VISITING) {
                if (cycles_allowed){
                    cycle = traceBack(focal_node, child, parent);
                    cycle.push_back(child);
                    cycles.push_back(cycle);
                }else{
                    if (chains_allowed){
                        chain = traceBack(focal_node, child, parent);
                        //not add the child -> this actually is a chain (missing the last linkage)
                        chains.push_back(chain);
                    }
                }
            } 

            // chain 
            else if (visited[child] == VISITED) {
                if (chains_allowed){
                    chain = traceBack(focal_node, -1, parent);
                    chains.push_back(chain);
                }else{
                    cout << "WEIRD STATE"<<endl;
                }
            }
        }
    }

    visited[focal_node] = VISITED;
}

vector<int> traceBack(const int& focal_node, const int& go_back_to_node, const vector<int>& parent){
    vector<int> chain;
    int current = focal_node;
    while (current != go_back_to_node) {
        chain.push_back(current);
        current = parent[current];
    }
    if (chain.size() == 1){
        return {};
    }
    reverse(chain.begin(), chain.end());
    return chain;
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
                if (cycle.size() >= 2){
                    uniqueCycles.insert(cycle); 
                    uniqueCycles_sorted.insert(cycle_sorted);
                }
            }
        }
        cycles.assign(uniqueCycles.begin(), uniqueCycles.end());

    } else if (type == "chains") {
        std::set<std::vector<int>> uniqueChains;
        for (auto& chain : chains) {
            if (chain.size() >= 2){
                uniqueChains.insert(chain);  
            }
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


/// NOT USED



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
        if (depth == _maxChainLength) {
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
        if (cycle.size() <= _maxCycleLength) {
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
                if (cycle.size() <= _maxCycleLength) {
                    cycles.push_back(cycle);
                }
            }
        }
    }

    // Backtrack: remove currentNode from stack and visited set
    stack.pop_back();
    visited.erase(currentNode);
}
