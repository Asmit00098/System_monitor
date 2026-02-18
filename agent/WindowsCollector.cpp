#include "MetricsCollector.hpp"
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <ctime>

class WindowsCollector : public MetricsCollector {
private:
    unsigned __int64 FileTimeToQuadWord(PFILETIME ft) {
        return (static_cast<unsigned __int64>(ft->dwHighDateTime) << 32) | ft->dwLowDateTime;
    }

public:
    SystemMetrics getMetrics() override {
        static unsigned __int64 lastIdle = 0, lastKernel = 0, lastUser = 0;
        
        SystemMetrics m;
        m.timestamp = std::time(nullptr);

        // 1. REAL SYSTEM CPU USAGE
        FILETIME fIdle, fKernel, fUser;
        if (GetSystemTimes(&fIdle, &fKernel, &fUser)) {
            unsigned __int64 currentIdle = FileTimeToQuadWord(&fIdle);
            unsigned __int64 currentKernel = FileTimeToQuadWord(&fKernel);
            unsigned __int64 currentUser = FileTimeToQuadWord(&fUser);

            unsigned __int64 idleDiff = currentIdle - lastIdle;
            unsigned __int64 kernelDiff = currentKernel - lastKernel;
            unsigned __int64 userDiff = currentUser - lastUser;
            
            // KernelTime includes IdleTime on Windows, so we subtract it
            unsigned __int64 totalDiff = (kernelDiff - idleDiff) + userDiff + idleDiff;

            if (totalDiff > 0) {
                m.cpuUser = (100.0 * userDiff) / totalDiff;
                m.cpuKernel = (100.0 * (kernelDiff - idleDiff)) / totalDiff;
                m.cpuIdle = (100.0 * idleDiff) / totalDiff;
            } else {
                m.cpuUser = 0.0; m.cpuKernel = 0.0; m.cpuIdle = 100.0;
            }

            lastIdle = currentIdle; lastKernel = currentKernel; lastUser = currentUser;
        }

        // 2. REAL SYSTEM RAM USAGE (Global, not just this process)
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo)) {
            m.memoryTotalMB = memInfo.ullTotalPhys / (1024 * 1024);
            m.memoryUsedMB = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024);
        } else {
            m.memoryUsedMB = 0; m.memoryTotalMB = 0;
        }

        // 3. REAL PROCESS & THREAD COUNTS
        PERFORMANCE_INFORMATION perfInfo;
        if (GetPerformanceInfo(&perfInfo, sizeof(perfInfo))) {
            m.processCount = perfInfo.ProcessCount;
            m.threadCount = perfInfo.ThreadCount;
        } else {
            m.processCount = 0; m.threadCount = 0;
        }

        return m;
    }
};