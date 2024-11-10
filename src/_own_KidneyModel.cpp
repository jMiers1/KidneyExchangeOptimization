// #include "_own_KidneyModel.hpp"

// // Constructor definition
// KidneyExchangeModel::KidneyExchangeModel(const IloNumArray2& adjacencyList, 
//                                          const std::vector<std::vector<int>>& predList, 
//                                          const std::map<std::pair<int, int>, double>& weights, 
//                                          const int& k, 
//                                          const int& l)
//     : env(), _AdjacencyList(adjacencyList), _PredList(predList), _Weights(weights), K(k), L(l) {}

// KidneyExchangeModel::~KidneyExchangeModel() {
//     env.end();  
// }



// double KidneyExchangeModel::solvePcTsp() {
//     try {
//         IloModel model(env);
//         int numNodes = _AdjacencyList.getSize();

//         cout << "numNodes: " << numNodes << endl; 

//         // Decision variables
//         IloArray<IloBoolVarArray> x(env, numNodes);  // x[i][j] indicates if edge (i, j) is chosen
//         for (int i = 0; i < numNodes; ++i) {
//             x[i] = IloBoolVarArray(env, numNodes);
//             for (int j = 0; j < numNodes; ++j) {
//                 x[i][j] = IloBoolVar(env);
//             }
//         }

//         cout << "Decision variables x: " <<x.getSize()<<endl;

//         IloBoolVarArray y(env, numNodes);  // y[i] indi cates if node i is included in the cycle
//         for (int i = 0; i < numNodes; ++i) {
//             y[i] = IloBoolVar(env);
//         }

//         cout << "Decision variables y: " <<y.getSize()<<endl;

//         // Objective: Maximize total prize (sum of weights)
//         IloExpr objective(env);
//         for (int i = 0; i < numNodes; ++i) {
//             for (int j = 0; j < numNodes; ++j) {
//                 if (_AdjacencyList[i][j] > 0) {  // Check if edge (i, j) exists
//                     objective += _Weights.at({i, j}) * x[i][j];
//                 }
//             }
//         }

//         cout << "Objective: " <<endl; 

        
//         model.add(IloMaximize(env, objective));
//         objective.end();

//         // Constraints

//         // 1. Flow Conservation: Each node has one incoming and one outgoing edge if included
//         for (int i = 0; i < numNodes; ++i) {
//             IloExpr inFlow(env);
//             IloExpr outFlow(env);
//             for (int j = 0; j < numNodes; ++j) {
//                 if (i != j) {
//                     inFlow += x[j][i];
//                     outFlow += x[i][j];
//                 }
//             }
//             model.add(inFlow == y[i]);
//             model.add(outFlow == y[i]);
//             inFlow.end();
//             outFlow.end();
//         }

//         // 2. Subtour Elimination (Lazy Constraints or Miller-Tucker-Zemlin (MTZ) Constraints)
//         IloIntVarArray u(env, numNodes, 0, numNodes - 1); // MTZ ordering variables
//         for (int i = 1; i < numNodes; ++i) {
//             for (int j = 1; j < numNodes; ++j) {
//                 if (i != j) {
//                     model.add(u[i] - u[j] + (numNodes - 1) * x[i][j] <= numNodes - 2);
//                 }
//             }
//         }

//         // 3. Cycle Length Constraint: Total selected nodes should not exceed `k`
//         IloExpr cycleLength(env);
//         for (int i = 0; i < numNodes; ++i) {
//             cycleLength += y[i];
//         }
//         model.add(cycleLength <= K);
//         cycleLength.end();

//         // Solve the model
//         IloCplex cplex(model);
//         cplex.setParam(IloCplex::TiLim, 300);  // Optional: Set a time limit
//         cplex.solve();

//         if (cplex.getStatus() == IloAlgorithm::Optimal || cplex.getStatus() == IloAlgorithm::Feasible) {
//             double solutionValue = cplex.getObjValue();
//             std::cout << "Optimal solution found with total prize: " << solutionValue << std::endl;
//             return solutionValue;
//         } else {
//             std::cout << "No feasible solution found." << std::endl;
//             return 0.0;
//         }
//     } catch (IloException& e) {
//         std::cerr << "CPLEX Exception: " << e.getMessage() << std::endl;
//         return 0.0;
//     }
// }
