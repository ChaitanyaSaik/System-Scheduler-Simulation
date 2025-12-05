# 🖥️ System Scheduler Simulator

A professional, fully deterministic CPU Scheduling Simulator built in **C++17**, implementing both **preemptive** and **non‑preemptive** scheduling algorithms.  
Designed with clean modular structure, detailed performance metrics, and readable Gantt chart visualization — making it ideal for **operating systems coursework**, **systems programming portfolios**, and **recruiter evaluation**.

---

## 🚀 Overview

This project simulates how an operating system CPU scheduler dispatches processes.  
It supports classical scheduling algorithms, computes all major OS metrics, and displays a clear time‑unit Gantt chart.

This version uses **true simulated CPU time** (no threads/sleep delays), making results deterministic, fast, and accurate — exactly what recruiters expect in a systems project.

---

## 🔧 Supported Scheduling Algorithms

### **Non‑Preemptive**
- **FCFS** – First Come First Served

### **Preemptive**
- **SRTF (Shortest Remaining Time First)**  
- **Preemptive Priority Scheduling**  
- **Round Robin (RR)** with configurable time quantum  

---

## 🎯 Key Features

- ✔️ **Deterministic time‑unit CPU simulation**  
- ✔️ **Preemption logic accurately modeled**  
- ✔️ **Dynamic process arrival handling**  
- ✔️ **Gantt chart visualization with timestamps**  
- ✔️ **Robust input supports both Console & CSV file**  
- ✔️ **Well‑structured metrics output**  
- ✔️ **Industry‑ready code structure (C++17)**  

---

## 📊 Metrics Computed

For every scheduling algorithm:

| Metric | Description |
|--------|-------------|
| **WT (Waiting Time)** | Time spent waiting in the ready queue |
| **TAT (Turnaround Time)** | Completion - Arrival |
| **Response Time** | First CPU access - Arrival |
| **CPU Utilization** | Active CPU time % |
| **Context Switch Count** | Number of process switches |
| **Throughput** | Processes per unit time |

---

## 📁 Project Structure

```
SystemSchedulerSimulator/
│
├── src/
│   └── SystemSchedulerSimulator.cpp   # Main simulator implementation
│
├── README.md                          # Documentation
│
└── sample_inputs/                     # Optional CSV-based test cases
```

---

## ▶️ Compilation & Execution

### **Compile**
```bash
g++ -std=c++17 SystemSchedulerSimulator.cpp -O2 -o scheduler
```

### **Run**
```bash
./scheduler
```

---

## 📥 Input Options

### **Console Mode**
The program interactively requests:

- Arrival Time  
- Burst Time  
- Priority  
- Time Quantum (for RR)

### **CSV Mode**
Format:

```
pid,arrival,burst,priority
1,0,8,2
2,1,4,1
3,2,2,3
4,3,1,4
```

Run mode **2** and provide file path.

---

## 📤 Sample Output (Excerpt)

```
=== FCFS (Non-preemptive) ===
P1 : WT=0, TAT=8, Resp=0
P2 : WT=7, TAT=11, Resp=7
...

Gantt Chart:
| P1 | P1 | P1 | P1 | P1 | P1 | P1 | P1 | P2 | P2 | P2 | P2 | ...
0     1     2     3     4     5     6     7     8 ...

CPU Utilization = 100%
Context Switches = 3
```

---

## 🧠 Technical Highlights (Great for Recruiters)

- Demonstrates **strong understanding of OS scheduling internals**
- Implements **preemption**, **context switching**, and **queue dynamics**
- Shows ability in **C++17**, STL, deterministic simulations
- Produces **clear, structured output** useful for debugging and analysis
- Supports **CSV-driven evaluation** — excellent for reproducible test cases

---

## 🔮 Future Enhancements

- Parallel **multi-core CPU simulation**  
- GUI Gantt chart using **Qt / SFML / Python matplotlib**  
- Logging via external files  
- Automatic random testcase generation  

---

## 👤 Author  
**Chaitanya Sai Kurapati**  
📅 June 2024  

---

## ⭐ If you find this project helpful, consider starring the repo!

