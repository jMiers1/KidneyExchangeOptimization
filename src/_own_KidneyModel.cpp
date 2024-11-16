#include "_own_KidneyModel.hpp"
#include "_own_Logger.hpp"
#include "_own_Utility.hpp"


#include <vector>
#include <map>
#include <utilit>


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
        _numNodes = _NNDs.size() + _PDPs.size();
    }

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


    // Constraints: Each node can appear at most once
    IloRangeArray constraints(env);
    for (int i = 0; i < _numNodes; i++){
        IloExpr out_Constraint(env);
        for (int predecessor : _PredList[i]){
            pair<int,int> edge = {predecessor, i};
            out_Constraint += x_edge[edge]
        }
        
        constraints.add(f_out[i] == out_Constraint);





    }
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




}
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