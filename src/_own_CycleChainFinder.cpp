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
                                    // cycles.clear();
                                    // chains.clear();
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
    /*
    Perfroms a depth-first-search on the network's Adjacency Graph to find cycles and chains. 
    params:
    - focal_node: node currently being expanded (i.e. if focal_node has sucessors, they will be expanded and explored into in the upocoming iterations)
    - visited: tracks the exploration status of all nodes, levels are: 
        - UNVISITED: not yet expanded 
        - VISITING: currently under investigation (i.e. not all branches originating from the node in the tree corresponding to this node's discovery have been closed yet)
        - VISITED: has been fully expanded in the past (i.e. all childs, their childs, ... have been expanded)
    - parent: immediate predecessor of each node in the current branch of the tree 
    - current_depth: exploration depth 
    - chains_allowed: bool flag to indeciate if from the current exploration, chains may be derived. (If current_depth > max_chain_length: set to false)
        -> logic: if max_chain_length < max_cycle_length: Not stop exploring yet, as potentially there may be valid cycles coming out of that invalid exploration chain
    - cycles_allowed: -"-
    */


    vector<int> chain, cycle;
    visited[focal_node] = VISITING;

    pair<string, int> smaller_bound = (_maxCycleLength < _maxChainLength) ? pair<string, int>("cycle", _maxCycleLength) : pair<string, int>("chain", _maxChainLength);

    // 1) Check tree depth to infer if max chain length or max cycle length are violated
    if (current_depth < smaller_bound.second){
        // none violated
        ;
    }else{
        if (current_depth > _maxChainLength){ 
            // chains not allowed anymore
            chains_allowed = false; 
        }
        if (current_depth > _maxCycleLength){
            // cycles not allowed anymore
            cycles_allowed = false; 
        }
    }


    // 2) Check if the focal node has childs 
    if (_AdjacencyList[focal_node].size() == 0){
        // no childs -> exploration ends (necessarily with a chain)
        if (current_depth <= _maxChainLength){
            processLeaveNode("chain", focal_node, -1, parent);
        }
    }else{
        // there are childs -> Have they been visited before? 
        for (int child : _AdjacencyList[focal_node]) {
            if (visited[child] == UNVISITED) {
                // UNVISITED child -> continue exploration 
                parent[child] = focal_node;
                dfs(child, visited, parent,current_depth+1, chains_allowed, cycles_allowed);
            }

            else if (visited[child] == VISITING) {
                // Child is currently being explored -> we must have started at this node -> cycle
                if (cycles_allowed){
                    // cycle is allowed -> add it to the set of cycles
                    processLeaveNode("cycle", focal_node, child, parent);
                
                }else{
                    // cycle os not allowed -> test if chains are allowed -> if chains are allowed: cut of child that would have closed to chain to a cycle 
                    if (chains_allowed){
                        processLeaveNode("chain", focal_node, -1, parent);
                    }
                }
            } 
            else if (visited[child] == VISITED) {
                // child has been fully explored
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
    /*
    Extracts a chain in the graph by traversing the parent vector backwards from the last node
    (if the structure is actually a not a chain but a cycle, this is accounted for by appending an arc from the the last to the first node (outside of this function))

    params: 
    - focal_node: the node from the with the backtracking is supposed to start 
    - go_back_to_node backtrack until this node is obtained 
    - parents: sores the index of the predecessor node of each node
    */

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

void CycleChainFinder::processLeaveNode(const std::string& structure_type, const int& focal_node, const int& child, const vector<int>& parent){
    vector<int> chain, cycle;

    /*
    Processing of the leaf nodes of the exploration tree resulting from the depth-first-search on the network

    params: 
    - structure_type: either 'chain' or 'cycle'
    - focal_node: the node tiggering this state to be a leaf
    - child: the child (i.e node currently to be expanded)
    - parent: global mapping of nodes to their immediate predecessor in the traversal of the graph 

    Internal variables: 
    - go_back_to_node: beginning at the current child, the path (cycle or chain) is reconstructed form the parent mapping up until this go_back_to_node. It can have one of 2 values: 
        - go_back_to_node = -1: go back to the start of the chain (-1 is the value, with which the parent mapping is initialized at the beginning of the search)
        - go_back_to_node = child: extract teh cycle starting and ending at the child 
    - isNew(): bool flag, returns true if the focal structure (cycle or chain) has not yet been been found (-> only add if isNew is true)
    - trackNodeMap(): updates the map keeping track of the cycles & chains, a node appears in 

    Global variables: 
    - cycles: the global set of cycles found so far 
    - chains: -"-
    */

    if (structure_type == "cycle"){
        int go_back_to_node = child;
        cycle = traceBack(focal_node, child, parent); 
        cycle.push_back(child); // add child to close up 

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

    /*
    Identification, if the discovered structure is novel or not. 
    The specific ordering of the nodes in the structure does not matter, the following are equal: 
    - cycle: 1-2-3-4-5-1, 2-3-4-5-1-2, 3-4-5-1-2-3, ... , 1-5-4-3-2-1, ... (irrelavent if clockwise or counter-clockwise or which one is seen as "first" node of the cycle) -> can all be reduced to 1-2-3-4-5-1 (this is called the reduced form)
    - chain: 1-2-3-4-5, 5-4-3-2-1 (irrelevant wether form 1 to 5 or from 5 to 1) -> can all be reduced to 1-2-3-4-5

    Thus we must first sort the structure to reduce it to a reduced form representation and only then check if it has been discovered yet.

    params: 
    - structure_type: either 'chain' or 'cycle'
    - structure: list of nodes forming to the chain or cycle

    global variables: 
    - unique_chains: keeps track of all reduced-form chains thet 
    */

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

void CycleChainFinder::trackNodeMap(const string& structure_type, const vector<int>& structure){
    /* 
    Updates a mapping of type {node:([cycle_idx],[chain_idx])} that for each node contains two lists, 
    one with all the cycles the node is part of and one with all the chains the node is part of.

    (cycle_idx and chain_idx refer to the position of the respective structure in the global set of discovered cycles "cycles" and chains "chains")
    */
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


void CycleChainFinder::trackWeightMap(const string& structure_type, const vector<int>& structure){
    /* 
    Updates two mappings of type {structure_idx:cost} (one for all cycles, one for all chains) that associates
    the structure with its total cost 

    (cycle_idx and chain_idx refer to the position of the respective structure in the global set of discovered cycles "cycles" and chains "chains")
    */

    int total = 0;
    for (size_t j = 0; j < structure.size() - 1; ++j) {
        int start = structure[j];
        int end = structure[j + 1];
        total += _Weights.find({start, end})->second; // add cost of arc (start,end) to running total of structure cost
    }


    if (structure_type == "chain"){
        _chainWeights[chains.size()] = total; 
    }
    else if (structure_type == "cycle"){
        auto it = _Weights.find({structure[structure.size()-1], structure[0]});
        total += it->second;      // need to close up cycle, i.e. if 1-2-3-4-5-1, need to add cost of edge 5-1
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