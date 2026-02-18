#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <atomic>

// Global flag to stop threads safely
std::atomic<bool> keepRunning(true);

// ---------------------------------------------------------
// 1. CPU STRESSER: Calculates heavy math to max out cores
// ---------------------------------------------------------
void stressCPU(int id) {
    double result = 0;
    while (keepRunning) {
        // Heavy floating point math
        result += std::sin(id) * std::tan(id);
    }
}

// ---------------------------------------------------------
// 2. RAM STRESSER: Eats 100MB chunks of RAM
// ---------------------------------------------------------
void stressRAM() {
    std::vector<char*> memory_blocks;
    long long total_mb = 0;

    std::cout << "Allocating RAM... (Press Ctrl+C to stop)" << std::endl;

    while (keepRunning) {
        try {
            // Allocate 100MB
            size_t size = 100 * 1024 * 1024; 
            char* ptr = new char[size];
            
            // CRITICAL: We must WRITE to the memory, otherwise Windows 
            // "Lazily" allocates it and it won't show up in usage stats immediately.
            memset(ptr, 1, size); 

            memory_blocks.push_back(ptr);
            total_mb += 100;

            std::cout << "Allocated: " << total_mb << " MB" << std::endl;
            
            // Slow down slightly to watch the graph climb
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
            
            // Safety cap: Stop at 4GB to prevent crashing your PC
            if (total_mb >= 4000) {
                std::cout << "Safety limit (4GB) reached. Holding memory..." << std::endl;
                while(keepRunning) std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } catch (std::bad_alloc& e) {
            std::cerr << "Out of memory!" << std::endl;
            break;
        }
    }

    // Cleanup (OS will do this anyway when process ends, but good practice)
    for (char* ptr : memory_blocks) delete[] ptr;
}

// ---------------------------------------------------------
// 3. THREAD STRESSER: Spawns sleeping threads
// ---------------------------------------------------------
void stressThreads() {
    std::vector<std::thread> threads;
    std::cout << "Spawning 1000 threads..." << std::endl;

    for (int i = 0; i < 1000; ++i) {
        threads.emplace_back([]() {
            while (keepRunning) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
        if (i % 100 == 0) std::cout << "Threads: " << i << std::endl;
    }
    
    std::cout << "1000 Threads Active. Check your dashboard!" << std::endl;
    
    // Join them so main() doesn't exit
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
}

int main() {
    int choice;
    std::cout << "=== SYSTEM MONITOR STRESS TEST ===" << std::endl;
    std::cout << "1. Stress CPU (Max Usage)" << std::endl;
    std::cout << "2. Stress RAM (Leak Memory)" << std::endl;
    std::cout << "3. Stress Threads (High Thread Count)" << std::endl;
    std::cout << "Enter choice: ";
    std::cin >> choice;

    if (choice == 1) {
        // Detect CPU cores and launch that many threads
        int cores = std::thread::hardware_concurrency();
        std::cout << "Launching " << cores << " CPU stressors..." << std::endl;
        std::vector<std::thread> pool;
        for (int i = 0; i < cores; ++i) pool.emplace_back(stressCPU, i);
        for (auto& t : pool) t.join();
    } 
    else if (choice == 2) {
        stressRAM();
    } 
    else if (choice == 3) {
        stressThreads();
    } 
    else {
        std::cout << "Invalid choice." << std::endl;
    }

    return 0;
}