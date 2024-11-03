#ifndef OperationTracer_HPP
#define OperationTracer_HPP

#include <map>
#include <string>


// Task | File | Line | Function | TimeUsage | MemoryUsage | CPUUsage | Comment
extern std::chrono::time_point<std::chrono::high_resolution_clock> global_start_time;
extern double global_start_memory;
extern std::vector<std::tuple<std::string, std::string, int, std::string, double, double,  double, std::string>> operationTraces;


// writes into operationTraces
std::chrono::time_point<std::chrono::high_resolution_clock> logTraces(
    const std::string& description, 
    std::string extraInfo, 
    std::chrono::time_point<std::chrono::high_resolution_clock> prevTimeStamp,
    const std::string& file, 
    const std::string& function,
    const int& line
);

// prints out operationTraces

void printTraces();
void initializeTraces();
double helperGetMemory();

#endif // OperationTracer_HPP