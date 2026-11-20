#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "models.h"
#include "routing.h"

// Limits used by the knapsack and matching functions
const int MAX_ITEMS = 20;       // most items in a knapsack
const int MAX_CAPACITY = 100;   // biggest knapsack capacity
const int MATCH_SIZE = 3;       // ambulances / emergencies in matching


// ---------------- SHORTEST PATH ----------------
RouteResult bellmanFord(int source, int destination);
void floydWarshall(int distance[N][N]);

// ---------------- MINIMUM SPANNING TREE ----------------
// Both return the total cost, or -1 if the graph is not connected
int primMST();
int kruskalMST();

// ---------------- RESOURCE ALLOCATION (KNAPSACK) ----------------
double fractionalKnapsack(int weights[], int values[], int itemCount, int capacity);
int zeroOneKnapsack(int weights[], int values[], int itemCount, int capacity);

// ---------------- ASSIGNMENT ----------------
// canServe[a][e] = 1 if ambulance a can serve emergency e
int maximumBipartiteMatching(int canServe[][MATCH_SIZE], int ambulanceCount, int emergencyCount);

#endif
