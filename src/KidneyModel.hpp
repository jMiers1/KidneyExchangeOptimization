#ifndef KIDNEYEXCHANGE_HPP
#define KIDNEYEXCHANGE_HPP

#include <iostream>
#include <vector>
#include <map>
#include <ilcplex/ilocplex.h>

using namespace std;

class KidneyExchangeModel {
public:
    KidneyExchangeModel(const IloNumArray2& AdjacencyList, 
                        const vector<vector<int>>& PredList, 
                        const map<pair<int,int>,double>& Weights, 
                        int K, 
                        int L);
    ~KidneyExchangeModel();
    double PC_TSP();

private:
    IloEnv env; 
    IloNumArray2 AdjacencyList;
    vector<vector<int>>PredList;
    map<pair<int,int>,double>Weights;
    int k;
};

#endif // KIDNEYEXCHANGE_HPP
