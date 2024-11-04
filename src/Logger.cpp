#include <mach/mach.h>
#include <mach/mach_time.h>
#include <chrono>
#include <thread> 
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using std::cout;
using std::cin;
using std::endl;
using std::setw;
using std::get;
using std::string;


#include "Logger.hpp"

// for % distribution of subtasks
auto global_start_time{std::chrono::high_resolution_clock::now()};  
auto global_start_memory{99.9};
auto global_used_time{99.9};
auto global_used_memory{99.9};
std::map<string, string> userInput = {{}};


int colWidth{15};
std::vector<std::tuple<string, string, int, string, double, double, double, string>> operationTraces;
std::tuple<string, string, string, string,string, string, string, string, string, string, string> logHeader{"Step", "Task", "File", "Line","Function", "Time(s)", "Time(%)","Memory(GB)", "Memory(%)", "CPU(%)", "Comment"};

void startLogging(){
    global_start_time = std::chrono::high_resolution_clock::now();
    global_start_memory = getMemory();
}

void finishLogging(){
    global_used_time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - global_start_time).count();
    global_used_memory = getMemory();
    writeLogs_Terminal();
    writeLogs_CSV();
}

// get memory at certain time
double getMemory(){
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

// logging function
std::chrono::time_point<std::chrono::high_resolution_clock> logging(const string& description, 
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
    double memoryUsage = getMemory() - global_start_memory;


    // CPU Usage
    double CPUUsage = 1.0; // todo

    operationTraces.emplace_back(description, filename, line, function, timeUsage, memoryUsage, CPUUsage, extraInfo);
    return std::chrono::high_resolution_clock::now();  // end of section = start of new section
}

std::ostringstream extractLogHeader(
    const std::tuple<string, string, string, string,string, string, string, string, string, string, string>& logHeader,
    string separator) 
{
    std::ostringstream oss;

    oss << std::fixed 
        << setw(colWidth - 10) << get<0>(logHeader) <<separator // step
        << setw(colWidth + 10) << get<1>(logHeader)<<separator  // task
        << setw(colWidth - 5) << get<2>(logHeader)<<separator   // file
        << setw(colWidth - 10) << get<3>(logHeader)<<separator  // line
        << setw(colWidth - 5) << get<4>(logHeader)<<separator   // function
        << setw(colWidth) << get<5>(logHeader)<<separator       // time(s)
        << setw(colWidth) << get<6>(logHeader)<<separator       // time(%)
        << setw(colWidth) << get<7>(logHeader)<<separator       // memory(gb)
        << setw(colWidth) << get<8>(logHeader)<<separator       // memory(%)
        << setw(colWidth) << get<9>(logHeader) <<separator       // cpu
        << setw(colWidth) <<  get<10>(logHeader) <<separator       // comment
        << '\n';
    return oss;
}

std::ostringstream extractLogEntry(
    const std::tuple<std::string, std::string, int, std::string, double, double, double, std::string>& log_entry, 
    int step,
    string separator) 
{
    std::ostringstream oss;

    // Write formatted output for the single tuple to the ostringstream
    oss << std::fixed << std::setprecision(3)
        << setw(colWidth - 10) << step <<separator                                     // step
        << setw(colWidth + 10) << get<0>(log_entry)  <<separator                       // task
        << setw(colWidth - 5) << get<1>(log_entry)  <<separator                        // file
        << setw(colWidth - 10) << get<2>(log_entry) << separator                       // line
        << setw(colWidth - 5) << get<3>(log_entry) <<separator                         // function
        << setw(colWidth) << get<4>(log_entry) <<separator                             // time(s)
        << setw(colWidth) << (get<4>(log_entry) / global_used_time) * 100 <<separator  // time(%)
        << setw(colWidth) << get<5>(log_entry) <<separator                              // memory(gb)
        << setw(colWidth) << (get<5>(log_entry) / global_used_memory) * 100 <<separator // memory(%)
        << setw(colWidth) << get<6>(log_entry) <<separator                              // cpu
        << setw(colWidth) << get<7>(log_entry)  <<separator                             // comment
        << '\n';
    return oss;
}

std::ostringstream extractUserInput_CSV(
    std::map<string, string>& userInput, 
    string separator){

    std::ostringstream oss;

    for (auto it = userInput.begin(); it != userInput.end(); ++it) {
        if (it != userInput.begin()) { 
            oss << separator; 
        }
        oss << it->first; 
    }
    oss << "\n";
for (auto it = userInput.begin(); it != userInput.end(); ++it) {
        if (it != userInput.begin()) { 
            oss << separator; 
        }
        oss << it->second; 
    }
    oss << "\n\n\n";
    return oss;
}



void writeLogs_CSV(){
    std::ostringstream oss;
    std::string inputPath = userInput.at("Input path"); // Get the input path
    std::replace(inputPath.begin(), inputPath.end(), '/', '+'); // Replace '/' with ':'
    oss << "../logs/" << userInput.at("Execution date") << "__" << inputPath << ".csv";
    std::string filename = oss.str();
    std::ofstream file(filename);
    int step = 0;

    if (file.is_open()) {
        file << extractUserInput_CSV(userInput, ";").str();
        file << extractLogHeader(logHeader = logHeader, ";").str();
        for (const auto& entry : operationTraces) {
            file << extractLogEntry(entry, step, ";").str();
            ++step;}
        file.close();
        std::cout << "Data written to " << filename << std::endl;
    } else {
        std::cerr << "Error: Could not open the file." << std::endl;
    }
}

void writeLogs_Terminal(){
    cout << "Logs \n" << std::endl;
    cout << "Global elapsed time: " << global_used_time << " s" <<endl;
    cout << "Global used memory: " << global_used_memory << " GB" <<endl;
    int step = 0;
    cout<< extractLogHeader(logHeader, "").str();
    for (const auto& entry : operationTraces) {
        std::cout << extractLogEntry(entry, step, "").str();
        ++step;
    }
    cout << "\n" << endl;
}

std::map<string, string> mapUserInput(const char * argv[]){

    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d");
    userInput["Execution date"] = oss.str();
    userInput["Input path"] = string(argv[1]);
    userInput["Output path"] = string(argv[2]);
    userInput["Max cycle length"] = string(argv[3]);
    userInput["Max chain length"] = string(argv[4]);
    userInput["Degree type"] = string(argv[5]);
    userInput["Time limit"] = string(argv[6]);
    userInput["Cycle/chain mdoe"] = string(argv[7]);
    return userInput;
};
