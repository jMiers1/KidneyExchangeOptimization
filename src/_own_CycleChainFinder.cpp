// CycleFinder.cpp
#include "_own_CycleChainFinder.hpp"
#include "_own_Utility.hpp"
#include "_own_Logger.hpp"

#include <algorithm>
#include <sstream>
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


    // stringstream ss;
    // ss << "NDD-cycles " << ndd_cycles << ".txt";
    // string filename = ss.str();
    // logChainsCycles(chains = chains, cycles = cycles, filename = filename);
    
    // cout << "Cycles "<<cycles.size()<<endl;
    // printCycles(cycles);
    // cout <<" Chains "<<chains.size()<<endl;
    // printChains(chains);

                                    




// Constructor
CycleChainFinder::CycleChainFinder(const vector<vector<int>>& adjacencyList,  
                                    const vector<vector<int>>& predList,  
                                    const map<pair<int,int>,double>& _weights,
                                    const int& k,
                                    const int& l) : _AdjacencyList(adjacencyList), _PredList(predList), _Weights(_weights), _maxCycleLength(k), _maxChainLength(l) {
                                    cycles.clear();
                                    chains.clear();
                                    separateNodeSet(); // differentiates node set into NDDs and PDPs
                                    findCyclesChains(); //finds all cycles and chains up to the max length 
                                    prevSectionEnd = logging("OWN || Found cycles & chains", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);
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

   for (int ndd : _NDDs){
        // no cycles startign from ndds are allowed
        // BUT: quicker if allowed
        dfs(ndd, visited, parent, 1, true, true);
   }

   for (int pdp : _PDPs){
        dfs(pdp, visited, parent, 1, true, true);
   }
}

void CycleChainFinder::dfs(int focal_node, vector<VisitState> visited, vector<int> parent, int current_depth, bool chains_allowed, bool cycles_allowed){
    vector<int> chain, cycle;
    // bool new_cycle, new_chain;
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
            processLeaveNode("chain", focal_node, -1, parent);
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
                    processLeaveNode("cycle", focal_node, child, parent);
                
                }else{
                    if (chains_allowed){
                        processLeaveNode("chain", focal_node, -1, parent);
                    }
                }
            } 

            // chain 
            else if (visited[child] == VISITED) {
                if (chains_allowed){
                    processLeaveNode("chain", focal_node, -1, parent);
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

void CycleChainFinder::trackNodeMap(const string& structure_type, const vector<int>& structure){
    if (structure_type == "chain"){
        int struture_index = chains.size();
        for (int node : structure) {
            mapNodes[node].first.push_back(struture_index);
        }
    }else if(structure_type == "cycle"){
        int struture_index = cycles.size();
        for (int node : structure) {
            mapNodes[node].second.push_back(struture_index);
        }
    } 
}

void CycleChainFinder::processLeaveNode(const std::string& structure_type, const int& focal_node, const int& child, const vector<int>& parent){
    vector<int> chain, cycle;

    /*
    Go_back_to_node: In the parent list, the predecsessor of each node is stored. 
        Going back to -1 corresponds to going back to the start of the chain, 
        Going back to the child correspoonds to extracting the cycle

    isNew() returns true if the focal structure (cycle or chain) has not yet been discovered 
    trackNodeMap(): adds the focal structure to the mapping indicating for each node the structures, that contain that node

    Eventually, the structure is added to the set of chains or cycles respectively
    
    */
    if (structure_type == "cycle"){
        int go_back_to_node = child;
        cycle = traceBack(focal_node, child, parent); 
        cycle.push_back(child); // add current child for completeness

        if (cycle.size() > 1 && isNew(structure_type, cycle)){  
            trackNodeMap(structure_type, cycle);  
            trackWeightMap(structure_type, cycle);
            cycles.push_back(cycle); 
        }
    }
    else if (structure_type == "chain"){
        int go_back_to_node = -1;
        chain = traceBack(focal_node, go_back_to_node, parent); 
        // NOT add current child

        if (chain.size() > 1 && isNew(structure_type, chain)){  
            trackNodeMap(structure_type, chain);  
            trackWeightMap(structure_type, chain);
            chains.push_back(chain);  
        }
    }
}

bool CycleChainFinder::isNew(const std::string& structure_type, const vector<int>& structure){
    vector<int> structure_sorted = structure;
    sort(structure_sorted.begin(), structure_sorted.end()); 

    if (structure_type == "chain" && unique_chains.find(structure_sorted) == unique_chains.end()){
        // chain not yet found
        unique_chains.insert(structure_sorted);
        return true;
    }
    else if (structure_type == "cycle" && unique_cycles.find(structure_sorted) == unique_cycles.end()){
        // cycle not yet found
        unique_cycles.insert(structure_sorted);
        return true;
    }
    return false;
}

void CycleChainFinder::trackWeightMap(const string& structure_type, const vector<int>& structure){
    int total = 0;
    for (size_t j = 0; j < structure.size() - 1; ++j) {
        int start = structure[j];
        int end = structure[j + 1];
        total += _Weights.find({start, end})->second;
    }


    if (structure_type == "chain"){
        _chainWeights[chains.size()] = total; 
    }
    else if (structure_type == "cycle"){
        // Close up the cycle: check for the last arc
        auto it = _Weights.find({structure[structure.size()-1], structure[0]});
        total += it->second;
        _cycleWeights[cycles.size()] = total; 
    }
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