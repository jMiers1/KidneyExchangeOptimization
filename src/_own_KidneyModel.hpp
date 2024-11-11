#ifndef KIDNEYEXCHANGE_HPP
#define KIDNEYEXCHANGE_HPP

#include <iostream>
#include <vector>
#include <map>
#include <ilcplex/ilocplex.h>

using namespace std;

class KidneyModel {
public:
    KidneyModel(IloEnv& env,
                        const vector<vector<int>>& _cycles, 
                        const vector<vector<int>>& _chains, 
                        const map<pair<int,int>,double>& _weights, 
                        map<int,pair<vector<int>,vector<int>>> _mapNodes,
                        vector<int> _ndds,
                        vector<int> _pdps);
    ~KidneyModel();
    double solvePatternFormulation();

private:
    IloEnv env;
    vector<vector<int>> _Cycles;
    vector<vector<int>> _Chains; 
    map<pair<int,int>,double> _Weights;
    map<int,pair<vector<int>,vector<int>>> _mapNodes;
    vector<int> _NDDs;
    vector<int> _PDPs;
};

#endif // KIDNEYEXCHANGE_HPP