#include <iostream>

#include "../include/models.h"
#include "../include/routing.h"
#include "../include/algorithms.h"
#include "../include/sorting_search.h"

using namespace std;


// ============================================================
// SMALL PRINTING HELPERS
// ============================================================

static void printHeader(const char* title)
{
    cout << "\n\n================================================\n";
    cout << " " << title << "\n";
    cout << "================================================\n";
}

static void printRoute(const RouteResult& route)
{
    for (int i = 0; i < route.pathLength; i++)
    {
        cout << locationNames[route.path[i]];

        if (i < route.pathLength - 1)
        {
            cout << " -> ";
        }
    }
}

static void printArray(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        cout << arr[i] << " ";
    }
}


// ============================================================
// 1. DIJKSTRA vs BELLMAN-FORD vs FLOYD-WARSHALL
// (uses the current road status, so try blocking a road first)
// ============================================================

static void shortestPathSection()
{
    printHeader("SHORTEST PATH: DIJKSTRA vs BELLMAN-FORD vs FLOYD-WARSHALL");

    int source = 0;         // Clock Tower
    int destination = 6;    // City Hospital

    cout << "\nFrom " << locationNames[source]
         << " to " << locationNames[destination] << "\n";


    // ---------- Dijkstra ----------
    RouteResult d = dijkstra(source, destination);

    cout << "\nDIJKSTRA (the algorithm used by the emergency system)\n";

    if (d.found)
    {
        cout << "  Route:    ";
        printRoute(d);
        cout << "\n  Distance: " << d.distance << " km\n";
    }
    else
    {
        cout << "  No route found.\n";
    }

    cout << "  Time:  O(V^2)\n";
    cout << "  Space: O(V) extra, O(V^2) to store the graph\n";


    // ---------- Bellman-Ford ----------
    RouteResult b = bellmanFord(source, destination);

    cout << "\nBELLMAN-FORD\n";

    if (b.found)
    {
        cout << "  Route:    ";
        printRoute(b);
        cout << "\n  Distance: " << b.distance << " km\n";
    }
    else
    {
        cout << "  No route found.\n";
    }

    cout << "  Time:  O(V^3) with an adjacency matrix (O(V*E) with an edge list)\n";
    cout << "  Space: O(V)\n";


    // ---------- Floyd-Warshall ----------
    int all[N][N];
    floydWarshall(all);

    cout << "\nFLOYD-WARSHALL (distance between EVERY pair of locations)\n";

    if (all[source][destination] != INF)
    {
        cout << "  Distance: " << all[source][destination] << " km\n";
    }
    else
    {
        cout << "  No route found.\n";
    }

    cout << "  Time:  O(V^3)\n";
    cout << "  Space: O(V^2)\n";

    cout << "\nAll three give the same distance. Dijkstra is the best fit here\n";
    cout << "because road distances are never negative.\n";
}


// ============================================================
// 2. PRIM vs KRUSKAL (minimum road network that connects the city)
// ============================================================

static void mstSection()
{
    printHeader("MINIMUM SPANNING TREE: PRIM vs KRUSKAL");

    int prim = primMST();
    int kruskal = kruskalMST();

    cout << "\nPRIM\n";

    if (prim == -1)
        cout << "  MST cannot be formed (city is not connected).\n";
    else
        cout << "  Minimum network cost: " << prim << " km\n";

    cout << "  Time:  O(V^2)\n";
    cout << "  Space: O(V)\n";


    cout << "\nKRUSKAL\n";

    if (kruskal == -1)
        cout << "  MST cannot be formed (city is not connected).\n";
    else
        cout << "  Minimum network cost: " << kruskal << " km\n";

    cout << "  Time:  O(E^2) in this version (selection sort of the roads)\n";
    cout << "         Standard Kruskal with a good sort is O(E log E)\n";
    cout << "  Space: O(V + E)\n";


    if (prim != -1 && kruskal != -1)
    {
        if (prim == kruskal)
            cout << "\nBoth algorithms found the same minimum cost.\n";
        else
            cout << "\nThe costs are different. Check the code!\n";
    }
}


// ============================================================
// 3. MERGE SORT vs QUICK SORT + BINARY SEARCH
// ============================================================

static void sortingSection()
{
    printHeader("SORTING AND SEARCHING (EMERGENCY RECORDS)");

    cout << "\nWHERE THESE ALGORITHMS ARE USED IN THE PROJECT:\n";
    cout << "Every reported emergency is saved as an Emergency Record\n";
    cout << "(main menu, option 5). In that menu:\n";
    cout << "  - Merge Sort and Quick Sort order the records by severity\n";
    cout << "    (Critical first), so the most urgent cases come first.\n";
    cout << "  - Binary Search (iterative and recursive) finds a record by\n";
    cout << "    its Emergency ID. IDs are saved in increasing order, so the\n";
    cout << "    ID array is already sorted.\n";
    cout << "The small array below is only an example to compare the\n";
    cout << "algorithms side by side.\n";

    // The same numbers are given to both sorting algorithms
    int a[8] = { 38, 27, 43, 3, 9, 82, 10, 5 };
    int b[8] = { 38, 27, 43, 3, 9, 82, 10, 5 };

    cout << "\nOriginal array: ";
    printArray(a, 8);
    cout << "\n";

    mergeSort(a, 0, 7);
    quickSort(b, 0, 7);

    cout << "\nMERGE SORT\n";
    cout << "  Sorted: ";
    printArray(a, 8);
    cout << "\n  Time:  O(n log n)\n";
    cout << "  Space: O(n) extra\n";

    cout << "\nQUICK SORT\n";
    cout << "  Sorted: ";
    printArray(b, 8);
    cout << "\n  Time:  O(n log n) average, O(n^2) worst case\n";
    cout << "  Space: O(log n) average (recursion)\n";


    // Binary search works only on the sorted array
    cout << "\nBinary search for 43 in the sorted array:\n";

    cout << "\nITERATIVE BINARY SEARCH\n";
    cout << "  Found at index: " << iterativeBinarySearch(a, 8, 43) << "\n";
    cout << "  Time:  O(log n)\n";
    cout << "  Space: O(1)\n";

    cout << "\nRECURSIVE BINARY SEARCH\n";
    cout << "  Found at index: " << recursiveBinarySearch(a, 0, 7, 43) << "\n";
    cout << "  Time:  O(log n)\n";
    cout << "  Space: O(log n) (recursion)\n";
}


// ============================================================
// 4. FRACTIONAL KNAPSACK vs 0/1 KNAPSACK
// Example: which medical supplies should the ambulance carry?
// ============================================================

static void knapsackSection()
{
    printHeader("RESOURCE ALLOCATION: FRACTIONAL vs 0/1 KNAPSACK");

    int weights[3] = { 10, 20, 30 };
    int values[3] = { 60, 100, 120 };
    int capacity = 50;

    cout << "\nAmbulance carrying capacity: " << capacity << "\n";
    cout << "Oxygen Kit   | weight 10 | value 60\n";
    cout << "Trauma Kit   | weight 20 | value 100\n";
    cout << "Medicine Box | weight 30 | value 120\n";

    double fractional = fractionalKnapsack(weights, values, 3, capacity);
    int zeroOne = zeroOneKnapsack(weights, values, 3, capacity);

    cout << "\nFRACTIONAL KNAPSACK (Greedy)\n";
    cout << "  Maximum value: " << fractional << "\n";
    cout << "  Items can be taken partly.\n";
    cout << "  Time:  O(n^2) in this version\n";
    cout << "  Space: O(n)\n";

    cout << "\n0/1 KNAPSACK (Dynamic Programming)\n";
    cout << "  Maximum value: " << zeroOne << "\n";
    cout << "  Each item is taken fully or not at all.\n";
    cout << "  Time:  O(n * capacity)\n";
    cout << "  Space: O(n * capacity)\n";
}


// ============================================================
// 5. MAXIMUM BIPARTITE MATCHING
// Sample data: which ambulance can serve which emergency
// ============================================================

static void matchingSection()
{
    printHeader("ASSIGNMENT: MAXIMUM BIPARTITE MATCHING");

    // 1 = this ambulance can serve this emergency (sample data)
    int canServe[3][3] =
    {
        // E1 E2 E3
        {  1, 1, 0 },    // A101
        {  0, 1, 1 },    // A102
        {  1, 0, 1 }     // A103
    };

    cout << "\n        E1  E2  E3\n";
    cout << "A101     1   1   0\n";
    cout << "A102     0   1   1\n";
    cout << "A103     1   0   1\n";

    int matches = maximumBipartiteMatching(canServe, 3, 3);

    cout << "\nMaximum ambulance-emergency assignments: " << matches << "\n";
    cout << "(one ambulance per emergency, one emergency per ambulance)\n";
    cout << "  Time:  O(m * k^2) in this matrix version (m ambulances, k emergencies)\n";
    cout << "  Space: O(k)\n";
    cout << "\nNote: this is a demonstration with sample data. The live\n";
    cout << "dispatch uses findNearestAmbulance() with Dijkstra.\n";
}


// ============================================================
// RUN ALL DEMONSTRATIONS
// ============================================================

void runWorkbench()
{
    shortestPathSection();
    mstSection();
    sortingSection();
    knapsackSection();
    matchingSection();

    cout << "\n\n================================================\n";
    cout << " END OF ALGORITHM WORKBENCH\n";
    cout << "================================================\n";
}
