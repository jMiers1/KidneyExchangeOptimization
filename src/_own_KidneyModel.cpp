#include "_own_KidneyModel.hpp"
#include "_own_Logger.hpp"
#include "_own_Utility.hpp"


#include <vector>
#include <map>
#include <set>
#include <utility>
#include <algorithm>
#include <iostream>
#include <stack>
#include <unordered_set>


typedef IloArray<IloNumArray>    FloatMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef map<pair<int, int>, IloBoolVar>  EdgeMap;


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
        // solvePatternFormulation();

        _numEdges = _Weights.size();
        _numNodes = _NDDs.size() + _PDPs.size();

        test();
    }

// Define the callback class using ILOLASYCONSTRAINTCALLBACK2 macro
ILOLAZYCONSTRAINTCALLBACK4(KidneyModelLazyCallback, 
                            EdgeMap, x_edge, 
                            IloBoolVarArray, f_in,
                            vector<vector<int>> , _adjList,
                            vector<vector<int>> , _predList) {

    /*
    1 find edges active in the current solutuion 
    -> getActiveEdges() returns set of <int,int> pairs

    2 find cycles that are violated by the current solution 
    -> find_violated_cycles() returns set of vector<int> cycles

    3 for each violated cycle, do:
        3.1 find the complement of the cycle 
            -> get_complement() returns vector<int> as the set of nodes not included in the cycle
        3.2 find the edges that go from the complement to the cycle
            -> get_cut_edges() returns set of <int,int> pairs
        3.3 create a lazy constraint and add it to the model
            -> sum(cut_edges) >= f_in[first node in cycle]
    

    PARAM: purgebale: do i need the constraint later on?
    */ 

    // Callback body
    IloEnv env = getEnv();

    // get active edges 
    int _numEdges = x_edge.size();
    vector<pair<int, int>> activeEdges; // the indecies of the active edges, i.e (i,j) 
    for(const auto& element: x_edge){
        const pair<int, int>& key = element.first;
        if (getValue(x_edge[key]) > 0.5){
            activeEdges.push_back(key);
        }
    }


    // find violated cycles
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
        IloExpr out_Constraint(env);  
        for (int successor : _AdjacencyList[i]) { 
            pair<int, int> edge = {i, successor};  
            out_Constraint += x_edge[edge]; 
        }
        constraints.add(f_out[i] == out_Constraint);  
        out_Constraint.end(); 
    }

    //PDP balance
    for (int& p : _PDPs) {

        //out <= in 
        constraints.add(f_out[p] <= f_in[p]);  

        //in <= 1
        IloExpr pdp_cyles(env); 
        for (int& c : _mapNodes[p].second) { 
            // ONLY cycles that contain the node
            pdp_cyles += x_cycle[c]; 
        }
        constraints.add(pdp_cyles +f_in[p] <= 1);  
        pdp_cyles.end();  
    }

    //NDD out <= 1
    for (int& n : _NDDs){
        constraints.add(f_out[n] <= 1);  
    }

// JUST VERIFY THIS PART
    for (int& n : _NDDs){
        constraints.add(f_in[n] == 0);  
    }
// JUST VERIFY THIS PART




    IloModel model(env);
    model.add(objective);   
    model.add(constraints); 
    IloCplex cplex(model);

    // Use the callback
    //cplex.use(KidneyModelLazyCallback(env, x_edge, f_in, _PredList, _AdjacencyList));
    //cplex.use(new (env) KidneyModelLazyCallback(env, x_cycle, x_edge, f_in, f_out, _PredList, _AdjacencyList));
    
    // Solve the model
    cplex.solve();
    result = cplex.getObjValue();


    return result;
}




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









vector<vector<int>> findViolatingEdgeCycles(const vector<pair<int, int>>& activeEdges) {
    vector<vector<int>> edgeCycles; // To store all cycles found
    set<int> exploredNodes;         // To store fully explored nodes
    set<pair<int, int>> unexploredEdges(activeEdges.begin(), activeEdges.end());
    
    while (!unexploredEdges.empty()) {
        // Start with an unexplored edge
        pair<int, int> edge = *unexploredEdges.begin();
        int start = edge.first;
        int end = edge.second;

        // Track the current exploration path
        vector<int> explorationPath;
        set<int> visitedInPath;  // Track nodes in the current path

        // Add the first edge to the exploration path
        explorationPath.push_back(start);
        explorationPath.push_back(end);
        visitedInPath.insert(start);
        visitedInPath.insert(end);

        // Explore the path starting from 'start'
        while (true) {
            // Search for the next edge where the start node matches the current end node
            auto it = find_if(unexploredEdges.begin(), unexploredEdges.end(),
                              [end](const pair<int, int>& e) { return e.first == end; });

            if (it == unexploredEdges.end()) {
                // No next edge found, break out of the loop (this chain is fully explored)
                break;
            }

            // Move to the next edge
            edge = *it;
            start = edge.first;
            end = edge.second;

            if (visitedInPath.find(end) != visitedInPath.end()) {
                // Cycle detected, record the cycle
                edgeCycles.push_back(explorationPath);
                break;
            }

            // Add the current edge's end node to the exploration path
            explorationPath.push_back(end);
            visitedInPath.insert(end);

            // Remove the current edge from unexploredEdges since we processed it
            unexploredEdges.erase(it);
        }

        // Remove the current edge from the unexplored set (if it was not already removed)
        unexploredEdges.erase(edge);
    }

    return edgeCycles;
}



void KidneyModel::test(){

    vector<pair<int, int>> edgesInSolution = {

        // remove (0, 1) (1, 0) 
        {0,6},
        {1,0},
        {2,3},
        {3,4},
        {4,5},
        {5,3},
        {6,1}};

    // Find violating cycles

    auto result = findViolatingEdgeCycles(edgesInSolution);

        // Print results
        cout << "Violating Edge Cycles:\n";
        for (const auto& cycle : result) {
            printVector(cycle);
        }
}
    