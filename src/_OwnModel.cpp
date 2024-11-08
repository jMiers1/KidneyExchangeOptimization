// ModelSolver.cpp
#include "_OwnModel.hpp"
#include <iostream>

ILOSTLBEGIN  // Required for CPLEX to use standard C++ library features

// Constructor: Initializes model, variables, objective, and constraints
ModelSolver::ModelSolver() : env(), model(env), x(env, 0.0, IloInfinity, ILOFLOAT, "x"), y(env, 0.0, IloInfinity, ILOFLOAT, "y") {
    // Define the objective function: Maximize Z = 3x + 2y
    IloObjective objective = IloMaximize(env, 3 * x + 2 * y);
    model.add(objective);

    // Add constraints
    model.add(x + 2 * y <= 4);  // x + 2y <= 4
    model.add(3 * x + y <= 5);  // 3x + y <= 5
}

// Destructor: Ends the CPLEX environment to release resources
ModelSolver::~ModelSolver() {
    env.end();
}

// Solve the model and print results
void ModelSolver::solve() {
    IloCplex cplex(model);  // Create a CPLEX solver

    // Solve the model
    if (cplex.solve()) {
        // Output results if solution is found
        std::cout << "Objective Value (Z) = " << cplex.getObjValue() << std::endl;
        std::cout << "x = " << cplex.getValue(x) << std::endl;
        std::cout << "y = " << cplex.getValue(y) << std::endl;
    } else {
        std::cout << "No solution found." << std::endl;
    }
}
