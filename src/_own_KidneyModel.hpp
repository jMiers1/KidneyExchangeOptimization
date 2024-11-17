#ifndef KidneyModel_Hpp
#define KidneyModel_Hpp

#include <iostream>
#include <vector>
#include <map>
#include <ilcplex/ilocplex.h>

using namespace std;

class KidneyModel {
public:
    KidneyModel(IloEnv& _env,
                const vector<vector<int>>& _cycles, 
                const vector<vector<int>>& _chains, 
                const map<pair<int,int>,double>& _weights, 
                const map<int,pair<vector<int>,vector<int>>>& _mapNodes,
                const map<int,double>& _chainWeights,
                const map<int,double>& _cycleWeights,
                const vector<int>& _ndds,
                const vector<int>& _pdps,
                const vector<vector<int>>& adjacencyList,  
                const vector<vector<int>>& predList);
    double solvePatternFormulation();
    void printSolutionStructure(const string& structure_type, const int& structure);
    void printSolution(const double& obj, const IloCplex& cplex, const IloBoolVarArray& x_cycle, const IloBoolVarArray& x_chain);
    void printObjectiveFunction();

    double solvePctspFormulation();

private:
    IloEnv env;
    vector<vector<int>> _Cycles;
    vector<vector<int>> _Chains; 
    map<int,double> _chainWeights;
    map<int,double> _cycleWeights;
    map<pair<int,int>,double> _Weights;
    map<int,pair<vector<int>,vector<int>>> _mapNodes;
    vector<int> _NDDs;
    vector<int> _PDPs;
    int _numCycles{-99};
    int _numChains{-99};
    int _numEdges{-99};
    int _numNodes{-99};
    vector<vector<int>> _AdjacencyList;
    vector<vector<int>> _PredList;


    //     // Nested class CutCallback
    // class CutCallback : public IloCplex::LazyConstraintCallbackI {
    // private:
    //     IloEnv env;
    //     IloBoolVarArray& x_cycle;  // Decision variables for cycles
    //     IloBoolVarArray& x_edge;   // Decision variables for edges
    //     IloBoolVarArray& f_in;     // Inflow variables
    //     const map<pair<int, int>, IloBoolVar>& edgeVars;  // Map for edge variables
    //     const vector<vector<int>>& predList;  // Predecessor list
    //     const vector<vector<int>>& adjacencyList;  // Adjacency list

    // public:
    //     // Constructor
    //     CutCallback(const IloEnv& env, 
    //                 IloBoolVarArray& x_cycle, 
    //                 IloBoolVarArray& x_edge, 
    //                 IloBoolVarArray& f_in, 
    //                 const map<pair<int, int>, IloBoolVar>& edgeVars, 
    //                 const vector<vector<int>>& predList, 
    //                 const vector<vector<int>>& adjacencyList)
    //         : env(env), 
    //           x_cycle(x_cycle), 
    //           x_edge(x_edge), 
    //           f_in(f_in), 
    //           edgeVars(edgeVars), 
    //           predList(predList), 
    //           adjacencyList(adjacencyList) {}

    //     // Main callback function to generate cuts
    //     void main() override {
    //         // Only called during the MIP solution phase
    //         IloBoolVarArray x_cycle = this->x_cycle;
    //         IloBoolVarArray x_edge = this->x_edge;
            
    //         // List to collect edges that are in the solution (i.e., y[e] > 0.5)
    //         vector<pair<int, int>> edges_in_solution;

    //         // Check for edges in the solution, i.e., edges that are selected (y[e] > 0.5)
    //         for (const auto& entry : edgeVars) {
    //             if (getValue(entry.second) > 0.5) { // If the edge is selected
    //                 edges_in_solution.push_back(entry.first);
    //             }
    //         }

    //         // Find any violated cycles based on the current solution
    //         vector<vector<int>> violated_cycles = find_violating_cycles(edges_in_solution);

    //         // For each violated cycle, generate a lazy cut
    //         for (const auto& cycle : violated_cycles) {
    //             vector<int> complement = get_complement(cycle);  // Nodes not in the cycle
    //             vector<pair<int, int>> cut_edges = get_cut_edges(complement, cycle);

    //             // Create a lazy constraint (cut) and add it to the model
    //             IloExpr cut_expr(env);
    //             for (const auto& edge : cut_edges) {
    //                 cut_expr += edgeVars.at(edge);  // Add selected edges to the cut expression
    //             }
                
    //             // Add the lazy constraint (cut)
    //             addLazyConstraint(cut_expr >= 1);
    //             cut_expr.end();
    //         }
    //     }

    //     // Helper function to find violated cycles (you need to implement this based on your cycle detection logic)
    //     vector<vector<int>> find_violating_cycles(const vector<pair<int, int>>& edges_in_solution) {
    //         vector<vector<int>> violated_cycles;
    //         // Example (You can replace it with actual cycle detection)
    //         return violated_cycles;
    //     }

    //     // Helper function to get the complement of a cycle
    //     vector<int> get_complement(const vector<int>& cycle) {
    //         vector<int> complement;
    //         return complement;
    //     }

    //     // Helper function to generate the cut edges
    //     vector<pair<int, int>> get_cut_edges(const vector<int>& complement, const vector<int>& cycle) {
    //         vector<pair<int, int>> cut_edges;
    //         for (int node : complement) {
    //             for (int neighbor : cycle) {
    //                 cut_edges.push_back({node, neighbor});
    //             }
    //         }
    //         return cut_edges;
    //     }
    // };



};

#endif // KidneyModel_Hpp