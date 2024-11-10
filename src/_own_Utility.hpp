// Utils.h
#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <map>
#include <iostream>
#include <ilcplex/ilocplex.h>

using namespace std;

void print2DArray(const std::vector<std::vector<int>>& array);
std::vector<std::vector<int>> convertIloNumArray2To2DArray(const IloNumArray2& iloArray);
std::vector<std::vector<int>> buildPredecessorList(const vector<vector<int>>& array);
void printVector(const std::vector<int>& vec);
void print2DMap(const map<int, pair<vector<int>, vector<int>>>& map);
vector<int> sortVector(const vector<int>& vec);
bool containedInVector(const int& target, const vector<int>& vec);
bool containedInSet(const int& target, const set<int>& set);

#endif // UTILS_H
