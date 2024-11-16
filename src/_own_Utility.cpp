#include "_own_Utility.hpp"
#include <vector>
#include <set>
#include <ilcplex/ilocplex.h>

void print2DArray(const std::vector<std::vector<int>>& array){
    for (int i = 0; i < array.size(); ++i) {
        std::cout << i << " : ";
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

bool containedInVector(const int& target, const vector<int>& vec){
    return std::find(vec.begin(), vec.end(), target) != vec.end();
}

bool containedInSet(const int& target, const std::set<int>& s) {
    return s.find(target) != s.end();
}

void print2DMap(const std::map<int, std::pair<std::vector<int>, std::vector<int>>>& map) {
    for (const auto& [node, vectors] : map) {
        const auto& vector1 = vectors.first;  // First vector in the pair
        const auto& vector2 = vectors.second; // Second vector in the pair
        
        // Print the node (key)
        std::cout << node << ": ";
        
        // Print the first vector
        std::cout << "<";
        for (size_t i = 0; i < vector1.size(); ++i) {
            std::cout << vector1[i];
            if (i < vector1.size() - 1) std::cout << ","; 
        }
        std::cout << "> --- ";

        // Print the second vector
        std::cout << "<";
        for (size_t i = 0; i < vector2.size(); ++i) {
            std::cout << vector2[i];
            if (i < vector2.size() - 1) std::cout << ","; 
        }
        std::cout << ">" << std::endl;
    }
}

void printWeights(const std::map<std::pair<int, int>, double>& weights) {
    for (const auto& entry : weights) {
        std::pair<int, int> key = entry.first;
        double value = entry.second;

        std::cout << "Arc (" << key.first << ", " << key.second << "): " << value << std::endl;
    }
}

void printAdjacency(const std::vector<std::vector<int>>& vecOfVecs) {
    for (size_t i = 0; i < vecOfVecs.size(); ++i) {
        std::cout << "Row " << i << ": [";
        for (size_t j = 0; j < vecOfVecs[i].size(); ++j) {
            std::cout << vecOfVecs[i][j];
            if (j != vecOfVecs[i].size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
}

void printPath(const std::vector<int>& vec){
    printVector(vec);
}

void printCycleWeights(const std::map<int, double>& cycleWeights) {
    for (const auto& entry : cycleWeights) {
        std::cout << entry.first <<": "<<  entry.second << std::endl;
    }
}

void printCycles(const vector<vector<int>>& cycles){
    print2DArray(cycles);
}

void printChains(const vector<vector<int>>& chains){
    print2DArray(chains);
}

void logChainsCycles(const std::vector<std::vector<int>>& chains, const std::vector<std::vector<int>>& cycles,  const std::string& filename) {
    // Create an output file stream (ofstream)
    std::ofstream outFile(filename);
    
    outFile << "Cycles \n";
    for (int i = 0; i < cycles.size(); ++i) {
        for (int j = 0; j < cycles[i].size(); ++j) {
            outFile << cycles[i][j] << " "; 
        }
        outFile << std::endl;  
    }
    outFile << "\n \n"; 


    outFile << "Chains \n";
    for (int i = 0; i < chains.size(); ++i) {
        for (int j = 0; j < chains[i].size(); ++j) {
            outFile << chains[i][j] << " "; 
        }
        outFile << std::endl;  
    }
    outFile << "\n"; 

    outFile.close();

    std::cout << "Array written to " << filename << " successfully." << std::endl;
}

