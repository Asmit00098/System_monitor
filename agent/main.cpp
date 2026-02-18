#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

// Link the library for MinGW/G++
#pragma comment(lib, "Psapi.lib") 

#include "WindowsCollector.cpp"

int main() {
    MetricsCollector* collector = new WindowsCollector();
    
    // Open in truncation mode to clear old logs on startup
    std::ofstream outFile("system_logs.csv", std::ios::trunc);
    
    // Write the NEW header
    outFile << "Timestamp,CPU_User,CPU_Kernel,CPU_Idle,RAM_Used_MB,RAM_Total_MB,Process_Count,Thread_Count" << std::endl;

    std::cout << "--- REAL SYSTEM MONITOR AGENT ---" << std::endl;
    std::cout << "Collecting global system metrics..." << std::endl;
    std::cout << "Press Ctrl+C to stop." << std::endl;

    while (true) {
        SystemMetrics m = collector->getMetrics();
        
        outFile << m.timestamp << "," 
                << m.cpuUser << "," 
                << m.cpuKernel << "," 
                << m.cpuIdle << "," 
                << m.memoryUsedMB << ","
                << m.memoryTotalMB << ","
                << m.processCount << "," 
                << m.threadCount << std::endl; // Flush to disk

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    delete collector;
    return 0;
}