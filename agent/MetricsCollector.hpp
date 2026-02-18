#ifndef METRICS_COLLECTOR_HPP
#define METRICS_COLLECTOR_HPP

#include <string>

struct SystemMetrics {
    long long timestamp;
    double cpuUser;
    double cpuKernel;
    double cpuIdle;
    long memoryUsedMB;   // Global System RAM Used
    long memoryTotalMB;  // Global System RAM Total
    long processCount;   // Total processes running on PC
    long threadCount;    // Total threads running on PC
};

class MetricsCollector {
public:
    virtual ~MetricsCollector() {}
    virtual SystemMetrics getMetrics() = 0;
};

#endif