#include "_own_KidneyModel.hpp"

// Constructor definition
KidneyModel::KidneyModel(IloEnv& _env,
                                        const vector<vector<int>>& _cycles, 
                                        const vector<vector<int>>& _chains, 
                                        const map<pair<int,int>,double>& _weights, 
                                        map<int,pair<vector<int>,vector<int>>> _mapNodes,
                                        vector<int> _ndds,
                                        vector<int> _pdps)
    : env(_env), _Cycles(_cycles), _Chains(_chains), _Weights(_weights), _mapNodes(_mapNodes), _NDDs(_ndds), _PDPs(_pdps){}

KidneyModel::~KidneyModel() {
    env.end();  
}



double KidneyModel::solvePatternFormulation() {
    int _numCycles = _Cycles.size();
    int _numChains = _Chains.size();

    // Decision variables
    IloArray<IloBoolVarArray> z_c(env, _numCycles);  // 1 if cycle i is chosen
    IloArray<IloBoolVarArray> z_p(env, _numChains);  // 1 if chain i is chosen


    //Objective 




    return 0.0;
    // try {

        

        
    //     model.add(IloMaximize(env, objective));
    //     objective.end();

    //     // Constraints

    //     // 1. Flow Conservation: Each node has one incoming and one outgoing edge if included
    //     for (int i = 0; i < numNodes; ++i) {
    //         IloExpr inFlow(env);
    //         IloExpr outFlow(env);
    //         for (int j = 0; j < numNodes; ++j) {
    //             if (i != j) {
    //                 inFlow += x[j][i];
    //                 outFlow += x[i][j];
    //             }
    //         }
    //         model.add(inFlow == y[i]);
    //         model.add(outFlow == y[i]);
    //         inFlow.end();
    //         outFlow.end();
    //     }

    //     // 2. Subtour Elimination (Lazy Constraints or Miller-Tucker-Zemlin (MTZ) Constraints)
    //     IloIntVarArray u(env, numNodes, 0, numNodes - 1); // MTZ ordering variables
    //     for (int i = 1; i < numNodes; ++i) {
    //         for (int j = 1; j < numNodes; ++j) {
    //             if (i != j) {
    //                 model.add(u[i] - u[j] + (numNodes - 1) * x[i][j] <= numNodes - 2);
    //             }
    //         }
    //     }

    //     // 3. Cycle Length Constraint: Total selected nodes should not exceed `k`
    //     IloExpr cycleLength(env);
    //     for (int i = 0; i < numNodes; ++i) {
    //         cycleLength += y[i];
    //     }
    //     model.add(cycleLength <= K);
    //     cycleLength.end();

    //     // Solve the model
    //     IloCplex cplex(model);
    //     cplex.setParam(IloCplex::TiLim, 300);  // Optional: Set a time limit
    //     cplex.solve();

    //     if (cplex.getStatus() == IloAlgorithm::Optimal || cplex.getStatus() == IloAlgorithm::Feasible) {
    //         double solutionValue = cplex.getObjValue();
    //         std::cout << "Optimal solution found with total prize: " << solutionValue << std::endl;
    //         return solutionValue;
    //     } else {
    //         std::cout << "No feasible solution found." << std::endl;
    //         return 0.0;
    //     }
    // } catch (IloException& e) {
    //     std::cerr << "CPLEX Exception: " << e.getMessage() << std::endl;
    //     return 0.0;
    // }
}
