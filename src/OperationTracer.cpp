#include <mach/mach.h>
#include <mach/mach_time.h>
#include <chrono>
#include <thread> 
#include <filesystem>
#include <iostream>
#include <string>

using std::cout;
using std::cin;
using std::endl;
using std::setw;
using std::get;
using std::string;


#include "OperationTracer.hpp"


auto global_start_time{std::chrono::high_resolution_clock::now()};
auto global_start_memory{1.0};

std::vector<std::tuple<string, string, int, string, double, double, double, string>> operationTraces;


void initializeTraces(){
    global_start_time = std::chrono::high_resolution_clock::now();
    global_start_memory = helperGetMemory();
}

double helperGetMemory(){
    task_basic_info info;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT; 
    kern_return_t result = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &count);
    double memoryUsage = 0.0;
    if (result != KERN_SUCCESS) {
        return -1.0;
    } else {
        memoryUsage = info.resident_size / (1024.0 * 1024.0);  // Convert to MB
    }
    return memoryUsage;
}

std::chrono::time_point<std::chrono::high_resolution_clock> logTraces(const string& description, 
                                                                      string extraInfo, //can be used for error logging
                                                                      std::chrono::time_point<std::chrono::high_resolution_clock> prevTimeStamp, 
                                                                      const string& file, 
                                                                      const string& function,
                                                                      const int& line) {
    // File Name
    string filename = std::filesystem::path(file).filename().string();
    
    // Time Usage
    double timeUsage = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - prevTimeStamp).count();

    // Memory Usage
    double memoryUsage = helperGetMemory() - global_start_memory;


    // CPU Usage
    double CPUUsage = 1.0; // todo

    operationTraces.emplace_back(description, filename, line, function, timeUsage, memoryUsage, CPUUsage, extraInfo);
    return std::chrono::high_resolution_clock::now();  // end of section = start of new section
}

void printTraces(){
    cout << "Traces \n" << std::endl;

    //globally elapsed time since program strat
    double global_time_usage = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - global_start_time).count();
    double global_memory_usage = helperGetMemory();

    cout << "Global elapsed time: " << global_time_usage << " s" <<endl;
    cout << "Global used memory: " << global_memory_usage << " GB" <<endl;

    int step = 0;
    int colWidth{15};
    cout<< setw(colWidth-10) << "Step" 
        << setw(colWidth+10) << "Task" 
        << setw(colWidth-5) << "File" 
        << setw(colWidth-10) << "Line" 
        << setw(colWidth-5) << "Function" 
        << setw(colWidth) << "Time(s)" 
        << setw(colWidth) << "Time(%)" 
        << setw(colWidth) << "Memory(GB)" 
        << setw(colWidth) << "Memory(%)" 
        << setw(colWidth) << "CPU(%)" 
        << setw(colWidth) << "Comment" 
        << '\n';

    for (const auto& entry : operationTraces) {
        cout << std::fixed << std::setprecision(3) 
             << setw(colWidth-10) << step     // step
             << setw(colWidth+10) << get<0>(entry) //task
             << setw(colWidth-5) << get<1>(entry)  //file
             << setw(colWidth-10) << get<2>(entry) //line
             << setw(colWidth-5) << get<3>(entry) //function
             << setw(colWidth) << get<4>(entry)  //time(s)
             << setw(colWidth) << (get<4>(entry) / global_time_usage)*100 //time(%)
             << setw(colWidth) << get<5>(entry) //memory(gb)
             << setw(colWidth) << (get<5>(entry) / global_memory_usage)*100 //memory(%)
             << setw(colWidth) << get<6>(entry)  //cpu
             << setw(colWidth) << get<7>(entry)  //comment
             << '\n';
        ++step;
    }
    cout << "\n" << endl;
}



