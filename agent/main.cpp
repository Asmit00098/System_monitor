#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include "MetricsCollector.hpp"

// --- THE PLATFORM SWITCH ---
#ifdef _WIN32
    #pragma comment(lib, "Psapi.lib") 
    #include "WindowsCollector.cpp"
#elif __linux__
    #include "LinuxCollector.cpp"
#endif

int main() {
    // Factory-style instantiation based on the OS
    MetricsCollector* collector;
    
    #ifdef _WIN32
        collector = new WindowsCollector();
        std::cout << "--- WINDOWS AGENT STARTED ---" << std::endl;
    #elif __linux__
        collector = new LinuxCollector();
        std::cout << "--- LINUX AGENT STARTED ---" << std::endl;
    #endif
    
    std::ofstream outFile("system_logs.csv", std::ios::trunc);
    outFile << "Timestamp,CPU_User,CPU_Kernel,CPU_Idle,RAM_Used_MB,RAM_Total_MB,Process_Count,Thread_Count" << std::endl;

    while (true) {
        SystemMetrics m = collector->getMetrics();
        
        outFile << m.timestamp << "," 
                << m.cpuUser << "," 
                << m.cpuKernel << "," 
                << m.cpuIdle << "," 
                << m.memoryUsedMB << ","
                << m.memoryTotalMB << ","
                << m.processCount << "," 
                << m.threadCount << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    delete collector;
    return 0;
}