// DataReader.cpp
#include "DataReader.hpp"
#include <cmath>  // For floor

int DataReader::ReadData() {
    // Original ReadData implementation, with data members accessed directly
    // Check if the instance file is valid
    std::ifstream inFile(FilePath, std::ifstream::in);
    if (!inFile) {
        std::cout << "\nInstance's files not found: " << FilePath << std::endl;
        return 0;
    }

    // Extract the file name (remove the path)
    std::string new_name = FilePath;
    long pos_str = -1;
    long size_str = FilePath.size();
    while (pos_str <= size_str) {
        new_name = new_name.substr(pos_str + 1);
        pos_str = new_name.find("/");
        if (pos_str < 0) break;
    }

    // Reading data into members
    PairsType = IloNumArray(env);
    inFile >> Nodes >> NDDs >> Pairs >> NumArcs >> AdjacencyList >> WeightMatrix >> PairsType;

    // Build the Predecessors List and Weights matrix, similar to your original function

    // ... rest of your code as-is ...

    return 1;
}