// SystemSchedulerSimulator.cpp
// Professional CPU Scheduling Simulator (C++17)
// Implements FCFS, SRTF (preemptive), Preemptive Priority, and Round Robin
// Simulates time (no real threads/sleep). Produces Gantt chart and metrics.
//
// Compile: g++ -std=c++17 SystemSchedulerSimulator.cpp -O2 -o scheduler
// Run: ./scheduler
//
#include <bits/stdc++.h>
using namespace std;

struct Process {
    int pid = 0;
    int arrival = 0;
    int burst = 0;
    int remaining = 0;
    int priority = 0;         // Lower value = higher priority
    int start = -1;           // First time it got CPU
    int completion = -1;
    // Derived metrics
    int waiting = 0;
    int turnaround = 0;
    int response = -1;
};

using Timeline = vector<int>; // pid at each time unit, 0 for idle

// Utility: print a nice Gantt chart with time ticks
void printGantt(const Timeline &g) {
    cout << "\nGantt Chart:\n";
    // First line: process symbols
    cout << "|";
    for (size_t t = 0; t < g.size(); ++t) {
        if (g[t] == 0) cout << " Idle |";
        else {
            cout << " P" << g[t] << "  |";
        }
    }
    cout << "\n";
    // Time ticks
    cout << "0";
    for (size_t t = 0; t < g.size(); ++t) {
        cout << setw(6) << (t + 1);
    }
    cout << "\n\n";
}

// Compute and print metrics for final processes and timeline
void computeAndPrintMetrics(vector<Process> procs, const Timeline &g) {
    int n = (int)procs.size();
    double totalWT = 0, totalTAT = 0, totalResp = 0;
    int totalBurst = 0;
    int completed = 0;
    int lastTime = (int)g.size();
    int contextSwitches = 0;
    int prevPID = -1;

    for (int t = 0; t < (int)g.size(); ++t) {
        if ((int)g[t] != prevPID) {
            if (t > 0 && prevPID != 0) contextSwitches++;
            prevPID = g[t];
        }
    }

    for (auto &p : procs) {
        p.turnaround = p.completion - p.arrival;
        p.waiting = p.turnaround - p.burst;
        if (p.response < 0) p.response = p.start - p.arrival;
        totalWT += p.waiting;
        totalTAT += p.turnaround;
        totalResp += p.response;
        totalBurst += p.burst;
        if (p.completion >= 0) completed++;
        cout << "P" << p.pid << " : Arrival=" << p.arrival
             << ", Burst=" << p.burst
             << ", Priority=" << p.priority
             << ", Start=" << p.start
             << ", Completion=" << p.completion
             << ", WT=" << p.waiting
             << ", TAT=" << p.turnaround
             << ", Resp=" << p.response << "\n";
    }

    cout << fixed << setprecision(3);
    cout << "\nSummary:\n";
    cout << "Avg Waiting Time  = " << (totalWT / n) << "\n";
    cout << "Avg Turnaround    = " << (totalTAT / n) << "\n";
    cout << "Avg Response Time = " << (totalResp / n) << "\n";
    cout << "Context Switches  = " << contextSwitches << "\n";
    cout << "Throughput (proc/unit time) = " << (double)completed / max(1, lastTime) << "\n";
    cout << "CPU Utilization = " << (double)totalBurst / max(1, lastTime) * 100.0 << " %\n\n";
}

// Reset helpers
void resetProcesses(vector<Process> &procs) {
    for (auto &p : procs) {
        p.remaining = p.burst;
        p.start = -1;
        p.completion = -1;
        p.waiting = 0;
        p.turnaround = 0;
        p.response = -1;
    }
}

// FCFS - Non-preemptive (time simulated)
Timeline FCFS(vector<Process> procs) {
    cout << "=== FCFS (Non-preemptive) ===\n";
    int cur = 0;
    Timeline gantt;
    // sort by arrival then pid
    sort(procs.begin(), procs.end(), [](const Process &a, const Process &b){
        if (a.arrival != b.arrival) return a.arrival < b.arrival;
        return a.pid < b.pid;
    });
    for (auto &p : procs) {
        if (cur < p.arrival) {
            // CPU idle until arrival
            while (cur < p.arrival) { gantt.push_back(0); cur++; }
        }
        // start if first time
        if (p.start == -1) p.start = cur, p.response = p.start - p.arrival;
        // run to completion
        for (int i = 0; i < p.burst; ++i) {
            gantt.push_back(p.pid);
            cur++;
        }
        p.completion = cur;
    }
    // compute completion times back into procs vector used to compute metrics
    // note: copy was by value so compute metrics directly here by mapping
    // For simplicity, recompute metrics by reconstructing using pid
    // Reconstruct final procs with computed completion/start info
    // We'll compute metrics by re-running a mapping
    // Build map pid->process info from last run
    // To keep consistent, we'll recompute metrics outside using original ordering:
    // For that, we need procs with up-to-date fields, so return gantt and compute using helper:
    // But helper expects procs with completion/start fields; as we modified local procs, it's fine.
    computeAndPrintMetrics(procs, gantt);
    printGantt(gantt);
    return gantt;
}

// SRTF (Shortest Remaining Time First) - preemptive, 1-unit tick simulation
Timeline SRTF(vector<Process> procs) {
    cout << "=== SRTF (Preemptive SJF) ===\n";
    int n = (int)procs.size();
    Timeline gantt;
    int completed = 0;
    int cur = 0;
    // Keep processes in original order but refer by index
    resetProcesses(procs);

    while (completed < n) {
        // find index with minimum remaining among arrived
        int idx = -1, minRem = INT_MAX;
        for (int i = 0; i < n; ++i) {
            if (procs[i].arrival <= cur && procs[i].remaining > 0) {
                if (procs[i].remaining < minRem) {
                    minRem = procs[i].remaining;
                    idx = i;
                }
            }
        }
        if (idx == -1) {
            // idle
            gantt.push_back(0);
            cur++;
            continue;
        }
        // if first time on CPU
        if (procs[idx].start == -1) {
            procs[idx].start = cur;
            procs[idx].response = procs[idx].start - procs[idx].arrival;
        }
        // execute 1 unit
        gantt.push_back(procs[idx].pid);
        procs[idx].remaining -= 1;
        cur++;
        if (procs[idx].remaining == 0) {
            procs[idx].completion = cur;
            completed++;
        }
    }

    computeAndPrintMetrics(procs, gantt);
    printGantt(gantt);
    return gantt;
}

// Preemptive Priority Scheduling (lower number = higher priority)
Timeline PreemptivePriority(vector<Process> procs) {
    cout << "=== Preemptive Priority Scheduling ===\n";
    int n = (int)procs.size();
    Timeline gantt;
    int completed = 0;
    int cur = 0;
    resetProcesses(procs);

    while (completed < n) {
        int idx = -1, bestPr = INT_MAX;
        for (int i = 0; i < n; ++i) {
            if (procs[i].arrival <= cur && procs[i].remaining > 0) {
                if (procs[i].priority < bestPr) {
                    bestPr = procs[i].priority;
                    idx = i;
                } else if (procs[i].priority == bestPr) {
                    // tie-breaker: lower remaining burst or earlier arrival
                    if (idx == -1 || procs[i].remaining < procs[idx].remaining) idx = i;
                }
            }
        }
        if (idx == -1) { gantt.push_back(0); cur++; continue; }
        if (procs[idx].start == -1) {
            procs[idx].start = cur;
            procs[idx].response = procs[idx].start - procs[idx].arrival;
        }
        // execute 1 unit
        gantt.push_back(procs[idx].pid);
        procs[idx].remaining -= 1;
        cur++;
        if (procs[idx].remaining == 0) {
            procs[idx].completion = cur;
            completed++;
        }
    }

    computeAndPrintMetrics(procs, gantt);
    printGantt(gantt);
    return gantt;
}

// Round Robin scheduling (time quantum tq)
Timeline RoundRobin(vector<Process> procs, int tq) {
    cout << "=== Round Robin (Quantum=" << tq << ") ===\n";
    int n = (int)procs.size();
    Timeline gantt;
    queue<int> q;
    vector<bool> inQ(n, false);
    int cur = 0, completed = 0;
    resetProcesses(procs);

    while (completed < n) {
        // enqueue newly arrived processes
        for (int i = 0; i < n; ++i) {
            if (!inQ[i] && procs[i].arrival <= cur && procs[i].remaining > 0) {
                q.push(i);
                inQ[i] = true;
            }
        }

        if (q.empty()) {
            // CPU idle
            gantt.push_back(0);
            cur++;
            continue;
        }

        int idx = q.front(); q.pop();
        // If first time scheduled
        if (procs[idx].start == -1) {
            procs[idx].start = cur;
            procs[idx].response = procs[idx].start - procs[idx].arrival;
        }
        int exec = min(tq, procs[idx].remaining);
        for (int i = 0; i < exec; ++i) {
            gantt.push_back(procs[idx].pid);
            procs[idx].remaining -= 1;
            cur++;
            // enqueue new arrivals that come while CPU is executing
            for (int j = 0; j < n; ++j) {
                if (!inQ[j] && procs[j].arrival <= cur && procs[j].remaining > 0) {
                    q.push(j);
                    inQ[j] = true;
                }
            }
        }
        if (procs[idx].remaining > 0) {
            q.push(idx);
        } else {
            procs[idx].completion = cur;
            completed++;
        }
    }

    computeAndPrintMetrics(procs, gantt);
    printGantt(gantt);
    return gantt;
}

// Utility to read processes from console
vector<Process> readFromConsole() {
    int n;
    while (true) {
        cout << "Enter number of processes: ";
        if (cin >> n && n > 0) break;
        cout << "Invalid input. Enter a positive integer.\n";
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    vector<Process> procs(n);
    for (int i = 0; i < n; ++i) {
        procs[i].pid = i + 1;
        cout << "=== Process " << procs[i].pid << " ===\n";
        cout << "Arrival time: "; cin >> procs[i].arrival;
        cout << "Burst time  : "; cin >> procs[i].burst;
        cout << "Priority    : "; cin >> procs[i].priority;
        procs[i].remaining = procs[i].burst;
    }
    return procs;
}

// Optionally read CSV file: pid,arrival,burst,priority (pid optional)
vector<Process> readFromCSV(const string &path) {
    ifstream fin(path);
    if (!fin.is_open()) {
        cerr << "Failed to open CSV file: " << path << "\n";
        return {};
    }
    vector<Process> procs;
    string line;
    // attempt to skip header if exists
    if (!getline(fin, line)) return procs;
    // check if header contains non-digit letters
    bool header = false;
    for (char c : line) if (isalpha(c)) header = true;
    if (!header) {
        // first line is data; process it
        stringstream ss(line);
        vector<int> vals;
        string tok;
        while (getline(ss, tok, ',')) vals.push_back(stoi(tok));
        Process p;
        if (vals.size() == 3) { p.pid = (int)procs.size() + 1; p.arrival = vals[0]; p.burst = vals[1]; p.priority = vals[2]; }
        else if (vals.size() == 4) { p.pid = vals[0]; p.arrival = vals[1]; p.burst = vals[2]; p.priority = vals[3]; }
        else throw runtime_error("CSV format invalid. Expected 3 or 4 columns.");
        p.remaining = p.burst;
        procs.push_back(p);
    }
    while (getline(fin, line)) {
        if (line.size() == 0) continue;
        stringstream ss(line);
        vector<int> vals;
        string tok;
        while (getline(ss, tok, ',')) vals.push_back(stoi(tok));
        Process p;
        if (vals.size() == 3) { p.pid = (int)procs.size() + 1; p.arrival = vals[0]; p.burst = vals[1]; p.priority = vals[2]; }
        else if (vals.size() == 4) { p.pid = vals[0]; p.arrival = vals[1]; p.burst = vals[2]; p.priority = vals[3]; }
        else throw runtime_error("CSV format invalid. Expected 3 or 4 columns.");
        p.remaining = p.burst;
        procs.push_back(p);
    }
    return procs;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "System Scheduler Simulator - Professional Edition (C++17)\n";
    cout << "Options:\n1) Input from console\n2) Input from CSV file (pid,arrival,burst,priority)\nChoose input mode (1/2): ";
    int mode;
    cin >> mode;
    vector<Process> procs;
    if (mode == 2) {
        cout << "Enter CSV file path: ";
        string path; cin >> path;
        procs = readFromCSV(path);
        if (procs.empty()) {
            cout << "Failed to read CSV or file empty. Exiting.\n";
            return 1;
        }
    } else {
        procs = readFromConsole();
    }

    // Sort by pid to preserve consistent reporting
    sort(procs.begin(), procs.end(), [](const Process &a, const Process &b){ return a.pid < b.pid; });

    // Provide an option to run all algorithms or selected ones
    cout << "\nSelect algorithms to run (e.g., 1 2 3 4) or 0 for all:\n"
         << "1: FCFS\n2: SRTF (preemptive SJF)\n3: Preemptive Priority\n4: Round Robin\nChoice: ";
    string line;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, line);
    vector<int> choices;
    if (line.empty() || line == "0") { choices = {1,2,3,4}; }
    else {
        stringstream ss(line);
        int x;
        while (ss >> x) choices.push_back(x);
    }

    // Run chosen algorithms
    for (int c : choices) {
        if (c == 1) {
            auto copyP = procs;
            resetProcesses(copyP);
            FCFS(copyP);
            cout << "---------------------------------------------\n";
        } else if (c == 2) {
            auto copyP = procs;
            resetProcesses(copyP);
            SRTF(copyP);
            cout << "---------------------------------------------\n";
        } else if (c == 3) {
            auto copyP = procs;
            resetProcesses(copyP);
            PreemptivePriority(copyP);
            cout << "---------------------------------------------\n";
        } else if (c == 4) {
            auto copyP = procs;
            resetProcesses(copyP);
            int tq;
            cout << "Enter time quantum for Round Robin (positive integer): ";
            while (!(cin >> tq) || tq <= 0) {
                cout << "Invalid quantum. Enter positive integer: ";
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            RoundRobin(copyP, tq);
            cout << "---------------------------------------------\n";
        } else {
            cout << "Unknown choice: " << c << "\n";
        }
    }

    cout << "Simulation complete.\n";
    return 0;
}
