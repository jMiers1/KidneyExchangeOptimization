#include "_own_KidneyModel.hpp"
#include "_own_Utility.hpp"
#include <vector>
#include <map>


// Constructor definition
KidneyModel::KidneyModel(IloEnv& _env,
                        const vector<vector<int>>& _cycles, 
                        const vector<vector<int>>& _chains, 
                        const map<pair<int,int>,double>& _weights, 
                        const map<int,pair<vector<int>,vector<int>>>& _mapNodes,
                        const map<int,double>& _chainWeights,
                        const map<int,double>& _cycleWeights,
                        const vector<int>& _ndds,
                        const vector<int>& _pdps)
    : env(_env), _Cycles(_cycles), _Chains(_chains), _cycleWeights(_cycleWeights), _chainWeights(_chainWeights), _Weights(_weights), _mapNodes(_mapNodes), _NDDs(_ndds), _PDPs(_pdps){
        _numCycles = _Cycles.size();
        _numChains = _Chains.size();
    }

KidneyModel::~KidneyModel() {
    env.end();  
}


double KidneyModel::solvePatternFormulation() {
  

    // Decision variables
    IloBoolVarArray x_cycle(env, _numCycles);  // 1 if cycle i is chosen
    IloBoolVarArray x_path(env, _numChains);  // 1 if chain i is chosen

    // // Objective
    // IloExpr obj(env);
    // for (int i = 0; i < _numCycles; ++i) {
    //     double cycle_weight = _cycleWeights[i]; 
    //     obj += cycle_weight * x_cycle[i];  
    // }
    // for (int i = 0; i < _numChains; ++i) {
    //     double chain_weight = _chainWeights[i]; 
    //     obj += chain_weight * x_path[i];  
    // }
    // IloObjective objective = IloMaximize(env, obj);

    // Objective
    IloExpr obj(env);
    for (int i = 0; i < _numCycles; ++i) {
        double cycle_weight = _cycleWeights[i]; 
        obj += cycle_weight * x_cycle[i];  
    }
    for (int i = 0; i < _numChains; ++i) {
        double chain_weight = _chainWeights[i];  // Corrected to _chainWeights
        obj += chain_weight * x_path[i];  
    }
    IloObjective objective = IloMaximize(env, obj);

    // Print the objective function
    cout << "Objective function: " << endl;
    for (int i = 0; i < _numCycles; ++i) {
        cout << " + " << _cycleWeights[i] << " * x_cycle[" << i << "]";
    }
    for (int i = 0; i < _numChains; ++i) {
        cout << " + " << _chainWeights[i] << " * x_path[" << i << "]";
    }
    cout << endl;


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
            nodeConstraint += x_path[chain_index];
        }
        constraints.add(IloRange(env, 0, nodeConstraint, 1)); 
        nodeConstraint.end(); 

        cout << "node " << node << " chains " << node_chains.size()<< " cycles " << node_cycles.size() <<endl;
    }

    IloModel model(env);
    model.add(objective);   
    model.add(constraints); 

    IloCplex cplex(model);
    cplex.solve();

    if (cplex.getStatus() == IloAlgorithm::Optimal) {
        double result = cplex.getObjValue();
        cout << "Optimal solution found with value: " << result << endl;
        return result;
    } else {
        cout << "No optimal solution found!" << endl;
        return 0;
    }
}

