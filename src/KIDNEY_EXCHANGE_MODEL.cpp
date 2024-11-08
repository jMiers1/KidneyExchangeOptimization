#include "KIDNEY_EXCHANGE_MODEL.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <ilcplex/ilocplex.h>


// Constructor: initialize environment and model, set adjacency lists and weights
KidneyExchangeModel::KidneyExchangeModel(IloEnv env, 
    const std::vector<std::vector<int>>& adjList, 
    const std::vector<std::vector<int>>& predList, 
    const std::unordered_map<myEdge, double>& weights)
    : env(env), model(env), adjList(adjList), predList(predList), weights(weights) {
}

// Initialize decision variables
void KidneyExchangeModel::addVariables() {
    // Binary variables for edges
    for (const auto& edge : weights) {
        yVars.add(IloNumVar(env, 0, 1, ILOBOOL));
    }
    
    // Continuous variables for inflows and outflows at each node
    for (size_t i = 0; i < adjList.size(); ++i) {
        fInVars.add(IloNumVar(env, 0, IloInfinity, ILOFLOAT));
        fOutVars.add(IloNumVar(env, 0, IloInfinity, ILOFLOAT));
    }
}

// Define constraints
void KidneyExchangeModel::addConstraints() {
    // Inflow and outflow constraints for each node
    for (size_t v = 0; v < adjList.size(); ++v) {
        // Inflow constraint
        IloExpr inflowSum(env);
        for (int pred : predList[v]) {
            inflowSum += yVars[pred];
        }
        model.add(fInVars[v] == inflowSum);

        // Outflow constraint
        IloExpr outflowSum(env);
        for (int succ : adjList[v]) {
            outflowSum += yVars[succ];
        }
        model.add(fOutVars[v] == outflowSum);

        // Additional flow constraints
        model.add(fOutVars[v] <= fInVars[v]);
        model.add(fInVars[v] <= 1);
        model.add(fOutVars[v] <= 1);
    }
}

// Objective function
void KidneyExchangeModel::addObjective() {
    IloExpr obj(env);
    for (const auto& [edge, weight] : weights) {
        int from = edge.first;
        int to = edge.second;
        obj += weight * yVars[from];
    }
    model.add(IloMaximize(env, obj));
}

// Build and solve the model
void KidneyExchangeModel::buildModel() {
    addVariables();
    addConstraints();
    addObjective();
}

void KidneyExchangeModel::solve() {
    IloCplex cplex(model);
    if (cplex.solve()) {
        std::cout << "Objective Value: " << cplex.getObjValue() << std::endl;
        for (size_t i = 0; i < yVars.getSize(); ++i) {
            std::cout << "y[" << i << "] = " << cplex.getValue(yVars[i]) << std::endl;
        }
    } else {
        std::cout << "No solution found." << std::endl;
    }
}
