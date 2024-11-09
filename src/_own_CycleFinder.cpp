// CycleFinder.cpp
#include "_own_CycleFinder.hpp"
#include <algorithm>
#include <iostream>


// Constructor
CycleFinder::CycleFinder(IloEnv& env,
                        const IloNumArray2& adjacencyList,  
                        const int& k) : _env(env), _AdjacencyList(_AdjacencyList), _K(k) {}


void CycleFinder::findCycles() {
    // Iterates over all nodes, calls dfs on each of them
    cycles.clear();
    int numNodes = _AdjacencyList.getSize();
    for (int node = 0; node < numNodes; ++node) {
        set<int> visited;
        vector<int> stack;
        dfs(node, stack, visited);
    }

    // Remove duplicate cycles
    set<vector<int>> unique_cycles(cycles.begin(), cycles.end());
    cycles.assign(unique_cycles.begin(), unique_cycles.end());
}

void CycleFinder::dfs(int currentNode, vector<int>& stack, set<int>& visited) {
    // Check if currentNode is already in the stack to detect a cycle
    auto it = find(stack.begin(), stack.end(), currentNode);
    if (it != stack.end()) {
        // Cycle detected; add it to cycles
        vector<int> cycle(it, stack.end());
        cycles.push_back(cycle);
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
                cycles.push_back(cycle);
            }
        }
    }

    // Backtrack: remove currentNode from stack and visited set
    stack.pop_back();
    visited.erase(currentNode);
}
