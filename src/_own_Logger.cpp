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
using std::ostringstream;


#include "_own_Logger.hpp"

// for % distribution of subtasks
auto global_start_time{std::chrono::high_resolution_clock::now()};  
auto global_start_memory{99.9};
auto global_used_time{99.9};
auto global_used_memory{99.9};
std::map<string, string> userInput = {{"0_Information", "0_Value"}};
auto prevSectionEnd{std::chrono::high_resolution_clock::now()}; 


int colWidth{15};
std::vector<std::tuple<string, string, int, string, double, double, double, string>> operationTraces;
std::tuple<string, string, string, string,string, string, string, string, string, string, string> logHeader{"Step", "Task", "File", "Line","Function", "Time(s)", "Time(%)","Memory(GB)", "Memory(%)", "CPU(%)", "Comment"};

void startLogging(const char * argv[]){
    mapUserInput(argv);
    global_start_time = std::chrono::high_resolution_clock::now();
    prevSectionEnd = std::chrono::high_resolution_clock::now(); 
    global_start_memory = getMemory();
}

void finishLogging(){
    global_used_time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - global_start_time).count();
    global_used_memory = getMemory();
    writeLogs_Terminal();
    writeLog_Files();
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
    double CPUUsage = 0; // todo

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
        << setw(colWidth + 20) << get<1>(logHeader)<<separator  // task
        << setw(colWidth + 20) << get<2>(logHeader)<<separator   // file
        << setw(colWidth - 5) << get<3>(logHeader)<<separator  // line
        << setw(colWidth + 10) << get<4>(logHeader)<<separator   // function
        << setw(colWidth) << get<5>(logHeader)<<separator       // time(s)
        << setw(colWidth) << get<6>(logHeader)<<separator       // time(%)
        << setw(colWidth) << get<7>(logHeader)<<separator       // memory(gb)
        << setw(colWidth) << get<8>(logHeader)<<separator       // memory(%)
        << setw(colWidth) << get<9>(logHeader) <<separator       // cpu
        << setw(colWidth-10) <<  get<10>(logHeader) <<separator       // comment
        << '\n';
    return oss;
}

std::ostringstream extractLogEntry(
    const std::tuple<string, string, int, string, double, double, double, string>& log_entry, 
    int step,
    string separator) 
{
    std::ostringstream oss;

    // Write formatted output for the single tuple to the ostringstream
    oss << std::fixed << std::setprecision(4)
        << setw(colWidth - 10) << step <<separator                                     // step
        << setw(colWidth + 20) << get<0>(log_entry)  <<separator                       // task
        << setw(colWidth + 20) << get<1>(log_entry)  <<separator                        // file
        << setw(colWidth - 5) << get<2>(log_entry) << separator                       // line
        << setw(colWidth + 10) << get<3>(log_entry) <<separator                         // function
        << setw(colWidth) << get<4>(log_entry) <<separator                             // time(s)
        << setw(colWidth) << (get<4>(log_entry) / global_used_time) * 100 <<separator  // time(%)
        << setw(colWidth) << get<5>(log_entry) <<separator                              // memory(gb)
        << setw(colWidth) << (get<5>(log_entry) / global_used_memory) * 100 <<separator // memory(%)
        << setw(colWidth) << get<6>(log_entry) <<separator                              // cpu
        << setw(colWidth-10) << get<7>(log_entry)  <<separator                             // comment
        << '\n';
    return oss;
}

std::ostringstream extractUserInput(
    std::map<string, string>& userInput, int colWidth){

    std::ostringstream oss;
    for (auto it = userInput.begin(); it != userInput.end(); ++it) {
        // if (it != userInput.begin()) { 
        //     oss << setw(colWidth); 
        // }
        oss << setw(colWidth) << it->first << setw(colWidth) << it->second << "\n"; 
    }
    return oss;
}

std::ostringstream fileName(std::map<string, string>& userInput, string fileType){
    std::ostringstream oss;
    oss << "../logs/" << userInput.at("Execution date") << "_:_" << userInput["Folder"] <<"_:_" << userInput["File"] << "." << fileType;
    return oss;
}

void createFile(ostringstream header, string fileType){
    string filename = fileName(userInput, fileType).str();
    std::ofstream file(filename);
    if (file.is_open()) {
        file << header.str();
        if (fileType == "csv"){
            int step = 0;
            for (const auto& entry : operationTraces) {
                file << extractLogEntry(entry, step, ";").str();
                ++step;}
        }
        file.close();
    }else {
        std::cerr << "Error: Could not open the file." << std::endl;    
    }
}

void writeLog_Files(){
    createFile(extractLogHeader(logHeader = logHeader, ";"), "csv");
    createFile(extractUserInput(userInput, 30), "txt");
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
    //userInput["Input path"] = string(argv[1]);
    //userInput["Output path"] = string(argv[2]);
    userInput["Max cycle length"] = string(argv[3]);
    userInput["Max chain length"] = string(argv[4]);
    userInput["Degree type"] = string(argv[5]);
    userInput["Time limit"] = string(argv[6]);
    userInput["Cycle/chain mode"] = string(argv[7]);
    

    // find path, folder, file from inpiut_path 
    const string& inputPath = string(argv[1]);
    size_t lastSlash = inputPath.find_last_of('/');            
    size_t secondLastSlash = inputPath.find_last_of('/', lastSlash - 1);  
    //userInput["Path"] = inputPath.substr(0, secondLastSlash);          
    userInput["Folder"] = inputPath.substr(secondLastSlash + 1, lastSlash - secondLastSlash - 1); 
    string filename = inputPath.substr(lastSlash + 1);
    size_t dotPos = filename.find('.');
    userInput["File"] = filename.substr(0, dotPos);
    cout << userInput["File"] << endl;

    return userInput;
};


// void writeLogs_CSV(){
//     string filename = fileName(userInput, "csv").str();
//     std::ofstream file(filename);
//     int step = 0;

//     if (file.is_open()) {
//         //file << extractUserInput(userInput, ";").str();
//         file << extractLogHeader(logHeader = logHeader, ";").str();
//         for (const auto& entry : operationTraces) {
//             file << extractLogEntry(entry, step, ";").str();
//             ++step;}
//         file.close();
//     } else {
//         std::cerr << "Error: Could not open the file." << std::endl;
//     }

//     createFile(extractUserInput(userInput, ";"), "txt");
//}