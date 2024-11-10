#include "_own_Utility.hpp"
#include <vector>
#include <ilcplex/ilocplex.h>

void print2DArray(const std::vector<std::vector<int>>& array){
    for (int i = 0; i < array.size(); ++i) {
        std::cout << "Sucessor Vetrex " << i << " : ";
        for (int j = 0; j < array[i].size(); ++j) {
            std::cout << array[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "\n "<< std::endl;
}

std::vector<std::vector<int>> convertIloNumArray2To2DArray(const IloNumArray2& iloArray) {
    std::vector<std::vector<int>> result;
    IloEnv env = iloArray.getEnv(); 

    for (IloInt i = 0; i < iloArray.getSize(); ++i) {
        IloNumArray row = iloArray[i];  
        std::vector<int> rowVector;
        for (IloInt j = 0; j < row.getSize(); ++j) {
            rowVector.push_back(static_cast<int>(row[j])); 
        }
        result.push_back(rowVector); 
        row.end(); 
    }
    return result;
}

std::vector<std::vector<int>> buildPredecessorList(const std::vector<std::vector<int>>& array) {
    std::vector<std::vector<int>> result(array.size()); 

    for (int i = 0; i < array.size(); i++) {
        for (int j = 0; j < array[i].size(); j++) {
            int successor = array[i][j];
            result[successor].push_back(i); 
        }
    }

    return result;
}

void printVector(const std::vector<int>& vec) {
    std::stringstream ss;
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        if (i != vec.size() - 1) {
            ss << ", "; 
        }
    }
    std::string vec_str = ss.str();  
    cout << vec_str <<endl;
}

vector<int> sortVector(const vector<int>& vec) {
    auto min_it = min_element(vec.begin(), vec.end());
    int min_index = distance(vec.begin(), min_it);
    vector<int> normalized(vec.begin() + min_index, vec.end());
    normalized.insert(normalized.end(), vec.begin(), vec.begin() + min_index);
    return normalized;
}




