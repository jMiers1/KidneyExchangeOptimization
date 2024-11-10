// CycleFinder.cpp
#include "_own_CycleFinder.hpp"
#include <algorithm>
#include <iostream>


// Constructor
CycleFinder::CycleFinder(IloEnv& env,
                        const IloNumArray2& adjacencyList,  
                        const int& k) : _env(env), _AdjacencyList(adjacencyList), _K(k) {}


//TODO: make more efficient
void CycleFinder::findCycles() {
    cycles.clear();
    int numNodes = _AdjacencyList.getSize();

    for (int node = 0; node < numNodes; ++node) {
        set<int> visited;
        vector<int> stack;
        dfs(node, stack, visited);
    }

    // Remove duplicates
    set<vector<int>> unique_cycles;
    for (const auto& cycle : cycles) {
        vector<int> normalized_cycle = normalizeCycle(cycle); 
        unique_cycles.insert(normalized_cycle);  
    }

    cycles.assign(unique_cycles.begin(), unique_cycles.end());
    this -> printAdjacencyList(); 
    cout << "\n"<<endl;
    this -> printAllCycles();


}

void CycleFinder::dfs(int currentNode, vector<int>& stack, set<int>& visited) {

    // neigbors of current node 
    vector<int> neighbors;
    for (int j = 0; j < _AdjacencyList[currentNode].getSize(); ++j) {
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
    for (int j = 0; j < _AdjacencyList[currentNode].getSize(); ++j) {
        int neighbor = static_cast<int>(_AdjacencyList[currentNode][j]);
        if (visited.find(neighbor) == visited.end()) {
            dfs(neighbor, stack, visited);
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

void CycleFinder::printAdjacencyList(){
    for (int i = 0; i < _AdjacencyList.getSize(); ++i) { 
        IloNumArray& row = _AdjacencyList[i];  
        
        cout << "Row " << i << ": ";
        for (int j = 0; j < row.getSize(); ++j) {  
            cout << row[j] << " ";  
        }
        cout << endl;  
    }
}

string CycleFinder::printVector(const std::vector<int>& neighbors) {
    std::stringstream ss;
    for (size_t i = 0; i < neighbors.size(); ++i) {
        ss << neighbors[i];
        if (i != neighbors.size() - 1) {
            ss << ", ";  // Add a separator after each element except the last
        }
    }
    std::string neighbors_str = ss.str();  // Convert the stringstream to string
    return neighbors_str;
}

void CycleFinder::printAllCycles() {
    cout << "Unique cycles: " << to_string(cycles.size()) << endl;
    
    // Iterate over each cycle in the 'cycles' vector
    for (size_t i = 0; i < cycles.size(); ++i) {
        cout << "Cycle " << i + 1 << ": ";
        
        // Print each element in the current cycle
        for (size_t j = 0; j < cycles[i].size(); ++j) {
            cout << cycles[i][j] << " ";
        }
        cout << endl; // Move to the next line after each cycle
    }
}

vector<int> CycleFinder::normalizeCycle(const vector<int>& cycle) {
    auto min_it = min_element(cycle.begin(), cycle.end());
    int min_index = distance(cycle.begin(), min_it);
    vector<int> normalized(cycle.begin() + min_index, cycle.end());
    normalized.insert(normalized.end(), cycle.begin(), cycle.begin() + min_index);
    return normalized;
}






// cout << "Current node: " <<to_string(currentNode) <<"\n"
//      << "Number of neighbors: " << _AdjacencyList[currentNode].getSize()<<"\n"
//      << "Neighbors: " << printVector(neighbors) <<"\n"
//      << "Stack: " << printVector(stack) <<"\n"
//      << "Visited: " << printVector(visited_vec) << endl; 
