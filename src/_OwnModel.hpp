// ModelSolver.hpp
#ifndef MODELSOLVER_HPP
#define MODELSOLVER_HPP

#include <ilcplex/ilocplex.h>

class ModelSolver {

public:
    ModelSolver();         // Constructor to set up the model
    ~ModelSolver();        // Destructor to clean up CPLEX environment
    void solve();          // Method to solve the model

private:
    IloEnv env;            // CPLEX environment
    IloModel model;        // CPLEX model
    IloNumVar x;           // Decision variable x
    IloNumVar y;           // Decision variable y
};

#endif // MODELSOLVER_HPP
