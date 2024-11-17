#include "_own_KidneyModel.hpp"
#include "_own_Logger.hpp"
#include "_own_Utility.hpp"


#include <vector>
#include <map>
#include <utility>
#include <algorithm>


typedef IloArray<IloNumArray>    FloatMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;
//typedef vector<vector<int>>      FloatMatrix;


// Constructor definition
KidneyModel::KidneyModel(IloEnv& _env,
                        const vector<vector<int>>& _cycles, 
                        const vector<vector<int>>& _chains, 
                        const map<pair<int,int>,double>& _weights, 
                        const map<int,pair<vector<int>,vector<int>>>& _mapNodes,
                        const map<int,double>& _chainWeights,
                        const map<int,double>& _cycleWeights,
                        const vector<int>& _ndds,
                        const vector<int>& _pdps,
                        const vector<vector<int>>& adjacencyList,  
                        const vector<vector<int>>& predList)
    : env(_env), _Cycles(_cycles), _Chains(_chains), _cycleWeights(_cycleWeights), _chainWeights(_chainWeights), _Weights(_weights), _mapNodes(_mapNodes), _NDDs(_ndds), _PDPs(_pdps), _AdjacencyList(adjacencyList), _PredList(predList) {
        _numCycles = _Cycles.size();
        _numChains = _Chains.size();
        solvePatternFormulation();

        _numEdges = _Weights.size();
        _numNodes = _NDDs.size() + _PDPs.size();
    }

// Define the callback class using ILOLASYCONSTRAINTCALLBACK2 macro
ILOLAZYCONSTRAINTCALLBACK4(KidneyModelLazyCallback, 
                          IloBoolVarArray, x_edge, 
                          IloBoolVarArray, f_in,
                          FloatMatrix, _adjList,
                          FloatMatrix, _predList) {
    // Callback body
    IloEnv env = getEnv();
    
    IloInt numNodes = _adjList.getSize();

    for (int& p : ) {
        IloNum sum = 0;
        
        // Check the outgoing edges (or incoming based on your logic)
        for (IloInt j = 0; j < _adjList[i].size(); ++j) {
            // Add the corresponding edge variable to the sum (based on selected edges)
            sum += getValue(x_edge[{i, _adjList[i][j]}]);  // Edge between i and adjacent nodes
        }

        // Apply a threshold to check if a constraint is violated (for example)
        if (sum > 1.0) {  // If the condition is violated
            IloNumExpr cut_expr(env);
            for (IloInt j = 0; j < _adjList[i].size(); ++j) {
                // Add edge to the cut expression
                cut_expr += x_edge[{i, _adjList[i][j]}];  // Edge between i and adjacent nodes
            }

            // Generate the lazy constraint: sum <= 1
            add(cut_expr <= 1);
            cut_expr.end();
        }
    }
}



// ILOLAZYCONSTRAINTCALLBACK2(LazyCallback, IloNumVarArray, used, NumVarMatrix, supply) {
//    IloInt const nbLocations = used.getSize();
//    IloInt const nbClients = supply.getSize();
//    for (IloInt j = 0; j < nbLocations; ++j) {
//       IloNum isUsed = getValue(used[j]);
//       IloNum served = 0.0; // Number of clients currently served from j
//       for (IloInt c = 0; c < nbClients; ++c)
//          served += getValue(supply[c][j]);
//       if ( served > (nbClients - 1.0) * isUsed + 0.01 ) {
//          IloNumExpr sum = IloExpr(getEnv());
//          for (IloInt c = 0; c < nbClients; ++c)
//             sum += supply[c][j];
//          sum -= (nbClients - 1) * used[j];
//          cout << "Adding lazy capacity constraint " << sum << " <= 0" << endl;
//          add(sum <= 0.0).end();
//          sum.end();
//       }
//    }
// }





double KidneyModel::solvePctspFormulation(){
    double result;

    // Decision variables
    IloBoolVarArray x_cycle(env, _numCycles);  
    map<pair<int, int>, IloBoolVar> x_edge;
    IloBoolVarArray f_out(env, _numNodes);  
    IloBoolVarArray f_in(env, _numNodes);  

    // Objective
    IloExpr obj(env);
    for (int i = 0; i < _numCycles; ++i) {
        double cycle_weight = _cycleWeights[i]; 
        obj += cycle_weight * x_cycle[i];  
    }

    for (const auto& entry : _Weights) {
        const pair<int, int>& edge = entry.first;  // Edge (i, j)
        double edge_weight = entry.second;         // Weight of the edge
        x_edge[edge] = IloBoolVar(env);        // Create IloBoolVar for the edge
        obj += edge_weight * x_edge[edge];
    }
    IloObjective objective = IloMaximize(env, obj);

    this -> printObjectiveFunction();


    // Constraints
    IloConstraintArray constraints(env);

    //incoming edges 
    for (int i = 0; i < _numNodes; i++) {
        IloExpr in_Constraint(env); 
        for (int predecessor : _PredList[i]) { 
            pair<int, int> edge = {predecessor, i};  
            in_Constraint += x_edge[edge]; 
        }
        constraints.add(f_out[i] == in_Constraint);  
        in_Constraint.end(); 
    }

    //outgoing edges 
    for (int i = 0; i < _numNodes; i++) {
        IloExpr out_Constraint(env);  // Initialize the expression
        for (int successor : _AdjacencyList[i]) { 
            pair<int, int> edge = {i, successor};  
            out_Constraint += x_edge[edge]; 
        }
        constraints.add(f_out[i] == out_Constraint);  // Add the constraint directly
        out_Constraint.end();  // Free memory allocated to the expression
    }

    //PDP balance
    for (int& p : _PDPs) {
        IloExpr pdp_cyles(env); 
        for (int& c : _mapNodes[p].second) { 
            // ONLY cycles that contain the node
            pdp_cyles += x_cycle[c]; 
        }
        IloExpr pdp_out(env); 
        pdp_out =  pdp_cyles +f_out[p];

        IloExpr pdp_in(env); 
        pdp_in =  pdp_cyles +f_in[p];

        //first part: out <= in 
        constraints.add(pdp_out <= pdp_in);  

        // second part: in <= 1
        constraints.add(pdp_in <= 1);  

        pdp_out.end();  
        pdp_in.end();
    }

    //NDD balance 
    for (int& n : _NDDs){
        IloExpr ndd_in;
        ndd_in = f_out[n];
        constraints.add(ndd_in <= 1);  
        ndd_in.end();
    }



    IloModel model(env);
    model.add(objective);   
    model.add(constraints); 
    IloCplex cplex(model);

    // Use the callback
    cplex.use(KidneyModelLazyCallback(env, x_edge, f_in, _PredList, _AdjacencyList));
    //cplex.use(new (env) KidneyModelLazyCallback(env, x_cycle, x_edge, f_in, f_out, _PredList, _AdjacencyList));
    
    // Solve the model
    cplex.solve();
    result = cplex.getObjValue();


    return result;
}



// // Implementation of the CutCallback constructor
// KidneyModel::CutCallback::CutCallback(const IloEnv& env, const IloBoolVarArray& x_cycle, 
//                                       const IloBoolVarArray& x_edge, const IloBoolVarArray& f_in, 
//                                       const map<pair<int, int>, IloBoolVar>& edgeVars, 
//                                       const vector<vector<int>>& predList, const vector<vector<int>>& adjacencyList)
//     : env(env), x_cycle(x_cycle), x_edge(x_edge), f_in(f_in), edgeVars(edgeVars), 
//       predList(predList), adjacencyList(adjacencyList) {}

// // Main callback function to generate cuts
// void KidneyModel::CutCallback::main() {
//     // Only called during the MIP solution phase
//     // Get the incumbent solution (current best solution)
//     IloBoolVarArray x_cycle = this->x_cycle;
//     IloBoolVarArray x_edge = this->x_edge;
    
//     // List to collect edges that are in the solution (i.e., y[e] > 0.5)
//     vector<pair<int, int>> edges_in_solution;

//     // Check for edges in the solution, i.e., edges that are selected (y[e] > 0.5)
//     for (const auto& entry : edgeVars) {
//         if (getValue(entry.second) > 0.5) { // If the edge is selected
//             edges_in_solution.push_back(entry.first);
//         }
//     }

//     // Find any violated cycles based on the current solution
//     vector<vector<int>> violated_cycles = find_violating_cycles(edges_in_solution);

//     // For each violated cycle, generate a lazy cut
//     for (const auto& cycle : violated_cycles) {
//         vector<int> complement = get_complement(cycle);  // Nodes not in the cycle
//         vector<pair<int, int>> cut_edges = get_cut_edges(complement, cycle);

//         // Create a lazy constraint (cut) and add it to the model
//         IloExpr cut_expr(env);
//         for (const auto& edge : cut_edges) {
//             cut_expr += edgeVars[edge];  // Add selected edges to the cut expression
//         }
        
//         // Add the lazy constraint (cut)
//         addLazyConstraint(cut_expr >= 1);
//         cut_expr.end();
//     }
// }

// // Helper function to find violated cycles (you need to implement this based on your cycle detection logic)
// vector<vector<int>> KidneyModel::CutCallback::find_violating_cycles(const vector<pair<int, int>>& edges_in_solution) {
//     // Your cycle detection logic goes here
//     vector<vector<int>> violated_cycles;
//     // Example (You can replace it with actual cycle detection)
//     // Find cycles that are violated by the current solution
//     return violated_cycles;
// }

// // Helper function to get the complement of a cycle
// vector<int> KidneyModel::CutCallback::get_complement(const vector<int>& cycle) {
//     vector<int> complement;
//     // Get the complement of nodes in the cycle
//     return complement;
// }

// // Helper function to generate the cut edges
// vector<pair<int, int>> KidneyModel::CutCallback::get_cut_edges(const vector<int>& complement, const vector<int>& cycle) {
//     vector<pair<int, int>> cut_edges;
//     // Create the edges that go from the complement to the cycle
//     for (int node : complement) {
//         for (int neighbor : cycle) {
//             cut_edges.push_back({node, neighbor});
//         }
//     }
//     return cut_edges;
// }




// class CutCallback : public IloCplex::LazyConstraintCallbackI {
// private:
//     IloEnv env;
//     IloBoolVarArray& x_cycle;
//     std::map<std::pair<int, int>, IloBoolVar>& x_edge;
//     IloBoolVarArray& f_in;
//     IloBoolVarArray& f_out;
//     std::vector<std::vector<int>>& _PredList;
//     std::vector<std::vector<int>>& _AdjacencyList;

// public:
//     CutCallback(IloEnv env,
//                 IloBoolVarArray& x_cycle,
//                 std::map<std::pair<int, int>, IloBoolVar>& x_edge,
//                 IloBoolVarArray& f_in,
//                 IloBoolVarArray& f_out,
//                 std::vector<std::vector<int>>& PredList,
//                 std::vector<std::vector<int>>& AdjacencyList)
//         : IloCplex::LazyConstraintCallbackI(env),
//           env(env),
//           x_cycle(x_cycle),
//           x_edge(x_edge),
//           f_in(f_in),
//           f_out(f_out),
//           _PredList(PredList),
//           _AdjacencyList(AdjacencyList) {}

//     void main() override {
//         // Example: Add your lazy constraints or cuts here.
//         IloInt numNodes = f_in.getSize();
//         for (IloInt i = 0; i < numNodes; ++i) {
//             IloNum inFlow = 0.0;
//             IloNum outFlow = 0.0;

//             // Calculate inFlow
//             for (int pred : _PredList[i]) {
//                 std::pair<int, int> edge = {pred, i};
//                 inFlow += getValue(x_edge[edge]);
//             }

//             // Calculate outFlow
//             for (int succ : _AdjacencyList[i]) {
//                 std::pair<int, int> edge = {i, succ};
//                 outFlow += getValue(x_edge[edge]);
//             }

//             // Add lazy constraint: outFlow <= inFlow
//             if (outFlow > inFlow + 1e-6) {
//                 IloExpr constraint(env);
//                 for (int succ : _AdjacencyList[i]) {
//                     std::pair<int, int> edge = {i, succ};
//                     constraint += x_edge[edge];
//                 }
//                 for (int pred : _PredList[i]) {
//                     std::pair<int, int> edge = {pred, i};
//                     constraint -= x_edge[edge];
//                 }
//                 add(constraint <= 0).end();
//                 constraint.end();
//             }
//         }
//     }

//     IloCplex::CallbackI* duplicateCallback() const override {
//         return new (env) CutCallback(env, x_cycle, x_edge, f_in, f_out, _PredList, _AdjacencyList);
//     }
// };





// Pattern model 
double KidneyModel::solvePatternFormulation() {
    double result;

    // Decision variables
    IloBoolVarArray x_cycle(env, _numCycles);  // 1 if cycle i is chosen
    IloBoolVarArray x_chain(env, _numChains);  // 1 if chain i is chosen

    // Objective
    IloExpr obj(env);
    for (int i = 0; i < _numCycles; ++i) {
        double cycle_weight = _cycleWeights[i]; 
        obj += cycle_weight * x_cycle[i];  
    }
    for (int i = 0; i < _numChains; ++i) {
        double chain_weight = _chainWeights[i];  
        obj += chain_weight * x_chain[i];  
    }
    IloObjective objective = IloMaximize(env, obj);

    this -> printObjectiveFunction();

    


    // Constraints: Each node can appear at most once
    IloRangeArray constraints(env);
    for (auto& node_pair : _mapNodes) {
        int node = node_pair.first;
        vector<int> node_chains = node_pair.second.first; // a list of the chains containing the node
        vector<int> node_cycles = node_pair.second.second; // a list of the cycles containing the node   

        // total sum of chaining cycles and chains <= 1
        IloExpr nodeConstraint(env);
        for (int cycle_index : node_cycles) {
            nodeConstraint += x_cycle[cycle_index];
        }
        for (int chain_index : node_chains) {
            nodeConstraint += x_chain[chain_index];
        }
        constraints.add(IloRange(env, 0, nodeConstraint, 1)); 
        nodeConstraint.end(); 

        // cout << "node " << node << " chains " << node_chains.size()<< " cycles " << node_cycles.size() <<endl;
    }

    IloModel model(env);
    model.add(objective);   
    model.add(constraints); 
    IloCplex cplex(model);

    prevSectionEnd = logging("Own || Built Model", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);

    cplex.setOut(env.getNullStream());
    cplex.solve();

    prevSectionEnd = logging("Own || Solved Model", "", prevSectionEnd, __FILE__, __FUNCTION__, __LINE__);

    if (cplex.getStatus() == IloAlgorithm::Optimal) {
        result = cplex.getObjValue();
        printSolution(result, cplex, x_cycle,x_chain);
        return result;

    } else {
        cout << "ERROR" << endl;
        return 0;
    }
    env.end();
}

void KidneyModel::printObjectiveFunction(){
  
    cout << "Objective function: " << endl;
    for (int i = 0; i < _numCycles; ++i) {
        cout << " + " << _cycleWeights[i] << " * x_cycle[" << i << "]";
    }
    for (int i = 0; i < _numChains; ++i) {
        cout << " + " << _chainWeights[i] << " * x_path[" << i << "]";
    }
    cout << endl;
}

void KidneyModel::printSolution(const double& obj, const IloCplex& cplex, const IloBoolVarArray& x_cycle, const IloBoolVarArray& x_chain){

    cout << "Pattern Formulation | Optimal solution:" << obj << endl;

    // cout << "Cycles:" << endl;
    for (int i = 0; i < _numCycles; ++i) {
        if (cplex.getValue(x_cycle[i]) > 0.5) { 
            printSolutionStructure("cycle", i);
        }
    }

    // cout << "Chains:" << endl;
    for (int i = 0; i < _numChains; ++i) {
        if (cplex.getValue(x_chain[i]) > 0.5) { 
            printSolutionStructure("chain", i);
        }
    }

}

void KidneyModel::printSolutionStructure(const string& structure_type, const int& index){
     stringstream ss;
    if (structure_type== "cycle"){
        vector<int> cycle = _Cycles[index];
        ss << "Cycle "<< index<<": weight " << _cycleWeights[index] << "[";
        for (size_t i = 0; i < cycle.size(); ++i) {
            ss << cycle[i];
            if (i != cycle.size() - 1) {
                ss << ", "; 
            }
        }
        ss << "]";

    }else if (structure_type== "chain"){
        vector<int> cycle = _Chains[index];
        ss << "Chain "<< index<<": weight " << _chainWeights[index] << "[";
        for (size_t i = 0; i < cycle.size(); ++i) {
            ss << cycle[i];
            if (i != cycle.size() - 1) {
                ss << ", "; 
            }
        }
        ss << "]";
    }
    string out_str = ss.str();  
    cout << out_str <<endl;
}