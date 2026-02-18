# 🖥️ System Resource Monitor

![C++](https://img.shields.io/badge/Agent-C++17-blue) ![Python](https://img.shields.io/badge/Dashboard-Python%203.10+-yellow) ![Streamlit](https://img.shields.io/badge/UI-Streamlit-red) ![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey)

A professional-grade system monitoring tool that visualizes **CPU**, **RAM**, **Process**, and **Thread** metrics in real-time. 

The project is designed with a decoupled architecture:
* **The Engine (C++):** A lightweight, high-performance background agent that queries the OS kernel using native APIs (Win32 API / Linux `/proc`).
* **The Dashboard (Python):** A modern, interactive web interface built with Streamlit and Plotly.

---

## 🚀 Key Features
* **Real-Time Monitoring:** Tracks System RAM (GB), CPU Usage (User/Kernel/Idle), and Active Threads.
* **Cross-Platform Architecture:** Uses the **Strategy Design Pattern** to switch between `WindowsCollector` and `LinuxCollector` seamlessly.
* **High Performance:** The C++ agent uses minimal resources (~4MB RAM).
* **Interactive Visualizations:** Live updating Gauge, Area, and Pie charts.
* **Stress Testing Tool:** Includes a custom C++ stress tester to simulate high CPU/RAM load for validation.

---

## 🛠️ Architecture
The system uses a producer-consumer model via a shared log file:

```mermaid
graph LR
⚡ Quick Start
Prerequisites
C++ Compiler: g++ (MinGW for Windows)

Python: 3.x with pip

1. Build & Run the Agent (C++)
Navigate to the agent folder and compile the backend.

Windows:

Bash
g++ -O2 agent/main.cpp -o agent/output/main.exe -lpsapi
./agent/output/main.exe
Linux:

Bash
g++ -O2 agent/main.cpp -o agent
./agent
You should see a message: "Agent started. Logging to system_logs.csv..."

2. Launch the Dashboard (Python)
In a new terminal, install dependencies and run the UI.

Bash
pip install streamlit pandas plotly
streamlit run dashboard/app.py
3. Run the Stress Test (Optional)
To see the charts spike, compile and run the stress tool in a third terminal:

Bash
g++ agent/stress_test.cpp -o stress_test.exe
./stress_test.exe
    A[C++ Agent] -- Writes Metrics --> B(system_logs.csv)
    B -- Reads Data --> C[Python Dashboard]
    C -- Renders --> D[Web Browser]
