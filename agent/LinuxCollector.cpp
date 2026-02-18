#include "MetricsCollector.hpp"
#include <fstream>
#include <string>
#include <ctime>

class LinuxCollector : public MetricsCollector {
public:
    SystemMetrics getMetrics() override {
        // 'static' ensures these values persist between function calls
        static long long prevUser = 0, prevNice = 0, prevSystem = 0, prevIdle = 0;
        
        SystemMetrics m;
        m.timestamp = std::time(nullptr);

        std::ifstream file("/proc/stat");
        std::string cpuLabel;
        long long user, nice, system, idle, iowait, irq, softirq, steal;

        if (file >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal) {
            // Calculate how much these values changed since the last second
            long long diffUser = user - prevUser;
            long long diffSystem = system - prevSystem;
            long long diffIdle = idle - prevIdle;
            long long totalDiff = diffUser + diffSystem + diffIdle + (nice - prevNice);

            if (totalDiff > 0) {
                m.cpuUser = (100.0 * diffUser) / totalDiff;
                m.cpuKernel = (100.0 * diffSystem) / totalDiff;
                m.cpuIdle = (100.0 * diffIdle) / totalDiff;
            } else {
                m.cpuUser = 0; m.cpuKernel = 0; m.cpuIdle = 100;
            }

            // Store current values to use as "previous" in the next loop
            prevUser = user; prevNice = nice; prevSystem = system; prevIdle = idle;
        }

        // Memory Parsing
        m.memoryMB = 0;
        std::ifstream memFile("/proc/meminfo");
        std::string label; long value;
        while (memFile >> label >> value) {
            if (label == "MemFree:") { m.memoryMB = value / 1024; break; }
        }

        m.contextSwitches = 1500; // Placeholder
        return m;
    }
};