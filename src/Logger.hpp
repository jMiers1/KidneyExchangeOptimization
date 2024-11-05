#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <map>
#include <string>

using std::string;


// Task | File | Line | Function | TimeUsage | MemoryUsage | CPUUsage | Comment
extern std::chrono::time_point<std::chrono::high_resolution_clock> global_start_time;
extern std::chrono::time_point<std::chrono::high_resolution_clock> prevSectionEnd;
extern double global_start_memory;
extern double global_used_time;
extern double global_used_memory;
extern int colWidth;
extern std::vector<std::tuple<string, string, int, string, double, double,  double, string>> operationTraces;
extern std::tuple<string, string, string, string, string, string,
                   string, string, string, string, string> logHeader;
extern std::map<string, string> userInput;

std::ostringstream fileName(std::map<string, string>& userInput, string fileType);
void startLogging(const char * argv[]);
void finishLogging();
double getMemory();
void writeLogs_Terminal();
void writeLog_Files();


std::chrono::time_point<std::chrono::high_resolution_clock> logging(const string& description, 
                                                                      string extraInfo, 
                                                                      std::chrono::time_point<std::chrono::high_resolution_clock> prevTimeStamp, 
                                                                      const string& file, 
                                                                      const string& function,
                                                                      const int& line);
std::map<string, string> mapUserInput(const char * argv[]);

std::ostringstream extractLogHeader(
    const std::tuple<string, string, string, string,string, string, string, string, string, string, string>& logHeader,
    string separator);

std::ostringstream extractLogEntry(
    const std::tuple<std::string, std::string, int, std::string, double, double, double, std::string>& log_entry, 
    int step,
    string separator);

std::ostringstream extractUserInput(std::map<string, string>& userInput, int colWidth);



#endif // OperationTracer_HPP