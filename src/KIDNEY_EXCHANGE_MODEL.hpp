#ifndef KIDNEY_EXCHANGE_MODEL_HPP
#define KIDNEY_EXCHANGE_MODEL_HPP

#include <ilcplex/ilocplex.h>
#include <vector>
#include <utility> // for std::pair

//typedef std::pair<int, int> Edge;


struct myEdge {
    int first;
    int second;

    // Overload the < operator to use it as a key in std::map
    bool operator<(const myEdge& other) const {
        return std::tie(first, second) < std::tie(other.first, other.second);
    }
};



class KidneyExchangeModel {
public:
    KidneyExchangeModel(IloEnv env, const std::vector<std::vector<int>>& adjList, 
                        const std::vector<std::vector<int>>& predList, 
                        const std::unordered_map<myEdge, double>& weights);

    void buildModel();
    void solve();

private:
    IloEnv env;
    IloModel model;
    IloNumVarArray yVars; // Binary decision variables for edges
    IloNumVarArray fInVars; // Flow variables for inflow
    IloNumVarArray fOutVars; // Flow variables for outflow

    const std::vector<std::vector<int>>& adjList;
    const std::vector<std::vector<int>>& predList;
    const std::unordered_map<myEdge, double>& weights;

    void addVariables();
    void addConstraints();
    void addObjective();
};

#endif // KIDNEY_EXCHANGE_MODEL_HPP
