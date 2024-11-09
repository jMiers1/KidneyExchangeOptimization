#ifndef KIDNEYEXCHANGE_HPP
#define KIDNEYEXCHANGE_HPP

#include <iostream>
#include <vector>
#include <map>
#include <ilcplex/ilocplex.h>

using namespace std;

class KidneyExchangeModel {
public:
    KidneyExchangeModel(const IloNumArray2& _AdjacencyList, 
                        const vector<vector<int>>& _PredList, 
                        const map<pair<int,int>,double>& _Weights, 
                        const int& K, 
                        const int& L);
    ~KidneyExchangeModel();
    double solvePcTsp();

private:
    IloEnv env; 
    IloNumArray2 _AdjacencyList;
    vector<vector<int>>_PredList;
    map<pair<int,int>,double>_Weights;
    int K;
    int L;
};

#endif // KIDNEYEXCHANGE_HPP
