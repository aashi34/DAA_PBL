# Smart Emergency Ambulance Routing and Hospital Allocation System

**Course:** Design and Analysis of Algorithms (DAA), PBL Project
**Student:** [Your Name]  |  **Roll No:** [Roll No]  |  **Section:** [Section]
**Faculty:** [Faculty Name]
**Language:** C++ (console version and Win32 GUI)

---

## 1. Project Overview

This project simulates an emergency-response system. When an emergency is
reported, the program:

1. finds the nearest available ambulance,
2. finds the shortest route from the ambulance to the patient,
3. selects the nearest suitable hospital (beds + trauma/ICU facility),
4. finds the shortest route from the patient to that hospital,
5. dispatches the ambulance and reduces the hospital's bed count.

If a road is blocked, the program recalculates the routes automatically.

The city is a **weighted undirected graph**:

| Graph concept | Meaning in the project |
|---------------|------------------------|
| Vertex        | City location          |
| Edge          | Road                   |
| Edge weight   | Distance in km         |
| No edge (INF) | No direct road         |

The city has 8 locations and 11 roads:

| No. | Location      | No. | Location      |
|-----|---------------|-----|---------------|
| 0   | Clock Tower   | 4   | ISBT          |
| 1   | Rajpur Road   | 5   | Prem Nagar    |
| 2   | Survey Chowk  | 6   | City Hospital |
| 3   | Ballupur      | 7   | Max Hospital  |

The main workflow:

```
Emergency -> Graph -> Dijkstra -> Best Ambulance Route -> Patient
          -> Suitable Hospital -> Best Hospital Route
```

---

## 2. Features

- Weighted graph stored as an **adjacency matrix**
- **Dijkstra's algorithm** with path reconstruction (`distance[]`, `visited[]`, `parent[]`)
- **Road blocking and unblocking** with automatic rerouting
  (a blocked road is never deleted; Dijkstra just ignores it)
- **Nearest available ambulance** selection (busy and unreachable ambulances are skipped)
- **Hospital allocation**: rejects hospitals with no beds, without the required
  facility, or unreachable; picks the nearest valid one
- Emergency severity (Low, Medium, High, Critical) and requirement (General, Trauma, ICU)
- Input validation in the console version
- **Algorithm Workbench** that compares the algorithms listed below
- Windows GUI with a city map (green = hospital route, orange = ambulance route,
  red + X = blocked road)

---

## 3. Algorithms Implemented

| No. | Algorithm | Technique | Used for | Where |
|-----|-----------|-----------|----------|-------|
| 1 | Dijkstra | Greedy | **Main routing** (live system) | `routing.cpp` |
| 2 | Bellman-Ford | DP / relaxation | Shortest-path comparison | `algorithms.cpp` |
| 3 | Floyd-Warshall | Dynamic programming | All-pairs shortest paths | `algorithms.cpp` |
| 4 | Prim | Greedy | Minimum road network (MST) | `algorithms.cpp` |
| 5 | Kruskal | Greedy | MST comparison | `algorithms.cpp` |
| 6 | Merge Sort | Divide and conquer | Sorting | `sorting_search.cpp` |
| 7 | Quick Sort | Divide and conquer | Sorting comparison | `sorting_search.cpp` |
| 8 | Iterative Binary Search | Divide and conquer | Searching | `sorting_search.cpp` |
| 9 | Recursive Binary Search | Divide and conquer | Searching | `sorting_search.cpp` |
| 10 | Fractional Knapsack | Greedy | Medical-supply loading | `algorithms.cpp` |
| 11 | 0/1 Knapsack | Dynamic programming | Medical-supply loading | `algorithms.cpp` |
| 12 | Maximum Bipartite Matching | Augmenting paths | Ambulance-emergency assignment | `algorithms.cpp` |

**Two parts of the project**

- **Emergency Simulator (main product):** Dijkstra, used by the ambulance
  selection, hospital selection and route finding.
- **Algorithm Workbench (menu option 6):** the other algorithms, demonstrated
  on the city graph or on small sample data.
  The workbench does not change the live dispatch.

Note: Maximum Bipartite Matching is a standalone demonstration with sample data
(which ambulance can serve which emergency). The live dispatch selects the
ambulance with `findNearestAmbulance()` using Dijkstra.

---

## 4. Complexity Table

V = vertices (locations), E = edges (roads), n = number of elements/items,
W = knapsack capacity, m = ambulances, k = emergencies.
The complexities below are for **this implementation**, not only the textbook version.

| Algorithm | Time | Auxiliary space | Remark |
|-----------|------|-----------------|--------|
| Dijkstra | O(V^2) | O(V) | adjacency matrix, no heap |
| Bellman-Ford | O(V^3) | O(V) | matrix scan; O(V*E) with an edge list |
| Floyd-Warshall | O(V^3) | O(V^2) | |
| Prim | O(V^2) | O(V) | adjacency matrix |
| Kruskal | O(E^2) | O(V + E) | simple selection sort of edges; O(E log E) with a good sort |
| Merge Sort | O(n log n) | O(n) | |
| Quick Sort | O(n log n) average, O(n^2) worst | O(log n) average | last element as pivot |
| Iterative Binary Search | O(log n) | O(1) | array must be sorted |
| Recursive Binary Search | O(log n) | O(log n) | recursion stack |
| Fractional Knapsack | O(n^2) | O(n) | simple sort by value/weight |
| 0/1 Knapsack | O(n * W) | O(n * W) | table of (n+1) x (W+1) |
| Max Bipartite Matching | O(m * k^2) | O(k) | matrix version; O(V*E) with lists |

Other costs in the system:

- Nearest ambulance: Dijkstra once per available ambulance, O(A * V^2)
- Hospital selection: Dijkstra once per valid hospital, O(H * V^2)

---

## 5. Project Structure

```
DAA_PBL/
|-- include/
|   |-- models.h            Ambulance, Hospital, Emergency structures
|   |-- routing.h           graph, blocked roads, RouteResult, Dijkstra
|   |-- ambulance.h         ambulance data and selection
|   |-- hospital.h          hospital data and allocation
|   |-- sorting_search.h    sorting and searching
|   |-- algorithms.h        Bellman-Ford, Floyd-Warshall, MST, knapsack, matching
|   `-- workbench.h         algorithm workbench
|-- src/
|   |-- main.cpp            console menu and emergency workflow
|   |-- routing.cpp         city graph, blocking, Dijkstra
|   |-- ambulance.cpp       nearest-ambulance logic
|   |-- hospital.cpp        hospital filtering and allocation
|   |-- sorting_search.cpp  Merge/Quick Sort, Binary Search
|   |-- algorithms.cpp      other DAA algorithms
|   `-- workbench.cpp       algorithm demonstrations
|-- gui/
|   `-- gui.cpp             Win32 GUI
|-- tests/
|   `-- test_cases.txt      test scenarios with expected results
|-- results/
|   `-- algorithm_comparison.txt
`-- README.md
```

**Design rule:** the algorithms exist only in the `src/` files. The GUI and the
console menu both call the same functions, so no algorithm is written twice.
`dijkstra()` returns a `RouteResult` (found, distance, path, pathLength) that
both the console and the GUI use.

---

## 6. How to Build and Run

Requires MinGW (g++) on Windows. Run the commands from the `DAA_PBL` folder.
Close any running copy of the program before building.

**Console version**
```
g++ -Iinclude src/main.cpp src/workbench.cpp src/routing.cpp src/ambulance.cpp src/hospital.cpp src/algorithms.cpp src/sorting_search.cpp -o emergency.exe
.\emergency.exe
```

**GUI version (Windows only)**
```
g++ -Iinclude gui/gui.cpp src/routing.cpp src/ambulance.cpp src/hospital.cpp -o emergency_gui.exe -mwindows -lgdi32
.\emergency_gui.exe
```

---

## 7. How to Use

**Console menu**

| Option | Action |
|--------|--------|
| 1 | Report emergency (full dispatch workflow) |
| 2 | Find shortest route between two locations |
| 3 | Block a road |
| 4 | Unblock a road |
| 5 | View ambulances and hospitals |
| 6 | Algorithm Workbench |
| 7 | Exit |

**GUI**

- *Report Emergency*: choose location, severity, requirement, then press
  DISPATCH EMERGENCY.
- *Route Management*: choose source and destination, then Find Route,
  Block Road or Unblock Road.
- *Reset System*: restores ambulances, hospital beds and roads.

**Example (Critical trauma patient at ISBT)**

```
Ambulance selected : A102 (3 km)   Ballupur -> ISBT
City Hospital      : rejected (no beds)
General Care       : rejected (no trauma facility)
Max Hospital       : valid (8 km)  ISBT -> City Hospital -> Max Hospital
```

**Example (rerouting)**

```
Normal route    : Clock Tower -> Survey Chowk -> ISBT -> City Hospital (8 km)
Survey Chowk - ISBT blocked:
New route       : Clock Tower -> Ballupur -> ISBT -> City Hospital (11 km)
```

---

## 8. Sample Data

| Ambulance | Location | Status |
|-----------|----------|--------|
| A101 | Clock Tower | Available |
| A102 | Ballupur | Available |
| A103 | Prem Nagar | Busy |

| Hospital | Location | Beds | Trauma | ICU |
|----------|----------|------|--------|-----|
| City Hospital | City Hospital (6) | 0 | Yes | Yes |
| Max Hospital | Max Hospital (7) | 8 | Yes | Yes |
| General Care | Rajpur Road (1) | 5 | No | No |

---

## 9. Testing

Test scenarios are in `tests/test_cases.txt`. Each test lists the input and the
expected output (calculated by hand from the graph). They cover:

- normal emergency and critical emergency
- nearest ambulance busy
- road blocked and alternative rerouting
- hospital full
- required facility unavailable
- no suitable hospital
- no available ambulance (all busy, and unreachable)
- invalid input
- algorithm workbench results

Test results: [fill in after running, for example "All 12 tests passed on __/__/____"].

---

## 10. Results and Benchmarking

`results/algorithm_comparison.txt` is for measured runtimes.

Runtime measurements: [to be added after benchmarking. Only values measured by
running the program should be recorded here].

The workbench already prints the distance found by each shortest-path algorithm
(all three agree) and the cost found by Prim and Kruskal (both 24 km).

---

## 11. Limitations

- The city is small and fixed (8 locations, 11 roads). The data is hard-coded.
- Dijkstra uses an adjacency matrix and a linear scan, so it is O(V^2). A heap with
  adjacency lists would be faster for large graphs.
- Distance is the only cost. Traffic, speed and travel time are not modelled.
- Severity is recorded and displayed, but it does not change the hospital choice.
  The choice depends on beds, facility and distance.
- Emergencies are handled one at a time. Ambulances do not return to a free state
  automatically (`releaseAmbulance()` exists but is not called by the menus).
- Maximum Bipartite Matching uses sample data. It is not connected to the live dispatch.
- Kruskal uses a simple O(E^2) selection sort, so it is slower than standard Kruskal.
- The GUI runs on Windows only (Win32 API).

---

## 12. Future Scope

- Priority queue (heap) Dijkstra with adjacency lists
- Use severity to prioritise emergencies when several arrive together
- Use bipartite matching or min-cost assignment for multiple simultaneous emergencies
- Ambulance release/return after a trip
- Travel time and traffic as edge weights
- Load the city, ambulances and hospitals from a file (`data/` folder)
- Complete benchmarking of all algorithms on larger random graphs

---

## 13. Conclusion

The project shows how shortest-path and greedy algorithms solve a real
decision problem. Dijkstra's algorithm is the core. Ambulance selection and
hospital allocation are built on it, and blocked roads are handled by ignoring
edges. The remaining algorithms are compared on the same city graph or on small
resource-allocation examples.
