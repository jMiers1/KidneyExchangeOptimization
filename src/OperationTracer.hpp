#ifndef OperationTracer_HPP
#define OperationTracer_HPP

#include <map>
#include <string>


// Task | File | Line | Function | TimeUsage | MemoryUsage | CPUUsage | Comment
extern std::vector<std::tuple<std::string, std::string, int, std::string, double, double,  double, std::string>> operationTrace;

#endif // OperationTracer_HPP