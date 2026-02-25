#include "MetricsCollector.hpp"
#include <fstream>
#include <string>
#include <ctime>
#include <sys/sysinfo.h>

class LinuxCollector : public MetricsCollector {
public:
    SystemMetrics getMetrics() override {
        static long long prevUser = 0, prevNice = 0, prevSystem = 0, prevIdle = 0;
        
        SystemMetrics m;
        m.timestamp = std::time(nullptr);

        // 1. REAL SYSTEM CPU USAGE (/proc/stat)
        std::ifstream statFile("/proc/stat");
        std::string cpuLabel;
        long long user, nice, system, idle, iowait, irq, softirq, steal;

        if (statFile >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal) {
            long long diffUser = user - prevUser;
            long long diffSystem = system - prevSystem;
            long long diffIdle = idle - prevIdle;
            long long totalDiff = diffUser + diffSystem + diffIdle + (nice - prevNice);

            if (totalDiff > 0) {
                m.cpuUser = (100.0 * diffUser) / totalDiff;
                m.cpuKernel = (100.0 * diffSystem) / totalDiff;
                m.cpuIdle = (100.0 * diffIdle) / totalDiff;
            } else {
                m.cpuUser = 0.0; m.cpuKernel = 0.0; m.cpuIdle = 100.0;
            }

            prevUser = user; prevNice = nice; prevSystem = system; prevIdle = idle;
        }

        // 2. REAL SYSTEM RAM & PROCESSES (sysinfo)
        struct sysinfo info;
        if (sysinfo(&info) == 0) {
            // Convert bytes to MB
            m.memoryTotalMB = (info.totalram * info.mem_unit) / (1024 * 1024);
            
            // Used RAM = Total - Free - Buffers
            long long free_ram = (info.freeram + info.bufferram) * info.mem_unit;
            m.memoryUsedMB = m.memoryTotalMB - (free_ram / (1024 * 1024));
            
            m.processCount = info.procs;
        }

        // 3. THREAD COUNT (/proc/loadavg)
        // Format looks like: 0.10 0.20 0.30 2/850 12345 (where 850 is total threads)
        std::ifstream loadavg("/proc/loadavg");
        std::string t1, t2, t3, threadsStr;
        if (loadavg >> t1 >> t2 >> t3 >> threadsStr) {
            size_t slashPos = threadsStr.find('/');
            if (slashPos != std::string::npos) {
                m.threadCount = std::stol(threadsStr.substr(slashPos + 1));
            } else {
                m.threadCount = 0;
            }
        }

        return m;
    }
};