#include <ilcplex/ilocplex.h>
#include <iostream>

int main() {
    IloEnv env;  // Create an environment for CPLEX
    try {
        IloModel model(env);  // Create a model in that environment

        // Create decision variables
        IloNumVar x(env, 0, IloInfinity, ILOFLOAT, "x");  // x >= 0
        IloNumVar y(env, 0, IloInfinity, ILOFLOAT, "y");  // y >= 0

        // Objective function: Minimize 3x + 4y
        model.add(IloMinimize(env, 3 * x + 4 * y));

        // Constraints:
        // 2x + y >= 14
        model.add(2 * x + y >= 14);
        
        // 3x + 4y <= 28
        model.add(3 * x + 4 * y <= 28);
        
        // x + 2y <= 20
        model.add(x + 2 * y <= 20);

        // Solve the model
        IloCplex cplex(model);
        if (cplex.solve()) {
            std::cout << "Solution status = " << cplex.getStatus() << std::endl;
            std::cout << "Optimal value of x = " << cplex.getValue(x) << std::endl;
            std::cout << "Optimal value of y = " << cplex.getValue(y) << std::endl;
            std::cout << "Optimal objective value = " << cplex.getObjValue() << std::endl;
        } else {
            std::cout << "Problem not solved!" << std::endl;
        }
    } catch (IloException &e) {
        std::cerr << "IloException caught: " << e.getMessage() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception caught!" << std::endl;
    }
    
    env.end();  // Clean up the environment
    return 0;
}
