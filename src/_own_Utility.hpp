// Utils.h
#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <iostream>
#include <ilcplex/ilocplex.h>

using namespace std;

void print2DArray(const std::vector<std::vector<int>>& array);
std::vector<std::vector<int>> convertIloNumArray2To2DArray(const IloNumArray2& iloArray);
std::vector<std::vector<int>> buildPredecessorList(const vector<vector<int>>& array);
void printVector(const std::vector<int>& vec);
vector<int> sortVector(const vector<int>& vec);

#endif // UTILS_H
