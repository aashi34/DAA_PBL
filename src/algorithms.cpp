#include "../include/algorithms.h"


// ============================================================
// 1. BELLMAN-FORD (shortest path)
// Relax every road V-1 times.
// Time: O(V^3) with an adjacency matrix (O(V*E) with an edge list)
// Auxiliary space: O(V)
// ============================================================

RouteResult bellmanFord(int source, int destination)
{
    RouteResult result;
    result.found = false;
    result.distance = INF;
    result.pathLength = 0;

    if (source < 0 || source >= N || destination < 0 || destination >= N)
    {
        return result;
    }

    int distance[N];
    int parent[N];

    for (int i = 0; i < N; i++)
    {
        distance[i] = INF;
        parent[i] = -1;
    }

    distance[source] = 0;

    // Repeat V-1 times
    for (int count = 0; count < N - 1; count++)
    {
        bool changed = false;

        for (int u = 0; u < N; u++)
        {
            for (int v = 0; v < N; v++)
            {
                if (graph[u][v] == INF) continue;     // no road
                if (blocked[u][v]) continue;          // blocked road
                if (distance[u] == INF) continue;     // u not reached yet

                // Relaxation
                if (distance[u] + graph[u][v] < distance[v])
                {
                    distance[v] = distance[u] + graph[u][v];
                    parent[v] = u;
                    changed = true;
                }
            }
        }

        // Nothing changed: distances are final
        if (!changed)
        {
            break;
        }
    }

    if (distance[destination] == INF)
    {
        return result;
    }

    // Build the path backwards using parent[], then reverse it
    int reversePath[N];
    int reverseLength = 0;
    int current = destination;

    while (current != -1)
    {
        reversePath[reverseLength] = current;
        reverseLength++;
        current = parent[current];
    }

    for (int i = reverseLength - 1; i >= 0; i--)
    {
        result.path[result.pathLength] = reversePath[i];
        result.pathLength++;
    }

    result.found = true;
    result.distance = distance[destination];

    return result;
}


// ============================================================
// 2. FLOYD-WARSHALL (all-pairs shortest paths)
// Try every location k as a middle stop between i and j.
// Time: O(V^3)   Auxiliary space: O(V^2)
// ============================================================

void floydWarshall(int distance[N][N])
{
    // Start from the road distances (blocked roads count as no road)
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (blocked[i][j])
            {
                distance[i][j] = INF;
            }
            else
            {
                distance[i][j] = graph[i][j];
            }
        }
    }

    for (int k = 0; k < N; k++)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (distance[i][k] == INF || distance[k][j] == INF)
                {
                    continue;
                }

                if (distance[i][k] + distance[k][j] < distance[i][j])
                {
                    distance[i][j] = distance[i][k] + distance[k][j];
                }
            }
        }
    }
}


// ============================================================
// 3. PRIM'S MINIMUM SPANNING TREE
// Grow the tree from location 0, always adding the cheapest road.
// Time: O(V^2)   Auxiliary space: O(V)
// ============================================================

int primMST()
{
    int key[N];          // cheapest road found so far to reach each location
    bool included[N];    // is the location already in the tree?

    for (int i = 0; i < N; i++)
    {
        key[i] = INF;
        included[i] = false;
    }

    key[0] = 0;

    int totalCost = 0;

    for (int count = 0; count < N; count++)
    {
        // Pick the cheapest location not yet in the tree
        int minimum = INF;
        int u = -1;

        for (int i = 0; i < N; i++)
        {
            if (!included[i] && key[i] < minimum)
            {
                minimum = key[i];
                u = i;
            }
        }

        if (u == -1)
        {
            return -1;      // graph is not connected
        }

        included[u] = true;
        totalCost += key[u];

        // Update the cheapest road to u's neighbours
        for (int v = 0; v < N; v++)
        {
            if (graph[u][v] != INF &&
                graph[u][v] != 0 &&
                !blocked[u][v] &&
                !included[v] &&
                graph[u][v] < key[v])
            {
                key[v] = graph[u][v];
            }
        }
    }

    return totalCost;
}


// ============================================================
// 4. KRUSKAL'S MINIMUM SPANNING TREE
// Sort all roads by distance, then take each road that does
// not create a cycle.
//
// Time: O(E^2) in this version, because of the simple selection
//       sort (standard Kruskal with a good sort is O(E log E)).
// Auxiliary space: O(V + E)
// ============================================================

struct Edge
{
    int source;
    int destination;
    int weight;
};


// Find which group a location belongs to
static int findSet(int parent[], int node)
{
    if (parent[node] == node)
    {
        return node;
    }

    return findSet(parent, parent[node]);
}


int kruskalMST()
{
    Edge edges[N * N];
    int edgeCount = 0;

    // Step 1: make a list of all open roads
    // (j starts at i+1 so each road is stored once)
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            if (graph[i][j] != INF && !blocked[i][j])
            {
                edges[edgeCount].source = i;
                edges[edgeCount].destination = j;
                edges[edgeCount].weight = graph[i][j];
                edgeCount++;
            }
        }
    }

    // Step 2: selection sort the roads by distance
    for (int i = 0; i < edgeCount - 1; i++)
    {
        int smallest = i;

        for (int j = i + 1; j < edgeCount; j++)
        {
            if (edges[j].weight < edges[smallest].weight)
            {
                smallest = j;
            }
        }

        Edge temp = edges[i];
        edges[i] = edges[smallest];
        edges[smallest] = temp;
    }

    // Step 3: every location starts in its own group
    int parent[N];

    for (int i = 0; i < N; i++)
    {
        parent[i] = i;
    }

    int selectedEdges = 0;
    int totalCost = 0;

    // Step 4: take a road only if its two ends are in different groups
    for (int i = 0; i < edgeCount && selectedEdges < N - 1; i++)
    {
        int rootA = findSet(parent, edges[i].source);
        int rootB = findSet(parent, edges[i].destination);

        if (rootA != rootB)
        {
            parent[rootA] = rootB;      // merge the two groups
            totalCost += edges[i].weight;
            selectedEdges++;
        }
    }

    if (selectedEdges != N - 1)
    {
        return -1;      // graph is not connected
    }

    return totalCost;
}


// ============================================================
// 5. FRACTIONAL KNAPSACK (Greedy)
// Take items with the best value/weight first.
// The last item may be taken only partly.
//
// Time: O(n^2) (simple sort)   Auxiliary space: O(n)
// ============================================================

double fractionalKnapsack(int weights[], int values[], int itemCount, int capacity)
{
    if (itemCount > MAX_ITEMS)
    {
        return 0;
    }

    int w[MAX_ITEMS];
    int v[MAX_ITEMS];
    double ratio[MAX_ITEMS];

    // Copy the data so the caller's arrays are not changed
    for (int i = 0; i < itemCount; i++)
    {
        w[i] = weights[i];
        v[i] = values[i];
        ratio[i] = (double)values[i] / weights[i];
    }

    // Sort by ratio, highest first
    for (int i = 0; i < itemCount - 1; i++)
    {
        for (int j = i + 1; j < itemCount; j++)
        {
            if (ratio[j] > ratio[i])
            {
                double tempRatio = ratio[i];
                ratio[i] = ratio[j];
                ratio[j] = tempRatio;

                int tempWeight = w[i];
                w[i] = w[j];
                w[j] = tempWeight;

                int tempValue = v[i];
                v[i] = v[j];
                v[j] = tempValue;
            }
        }
    }

    int remaining = capacity;
    double totalValue = 0;

    for (int i = 0; i < itemCount; i++)
    {
        if (w[i] <= remaining)
        {
            // Whole item fits
            remaining = remaining - w[i];
            totalValue = totalValue + v[i];
        }
        else
        {
            // Only a part of the item fits
            totalValue = totalValue + v[i] * ((double)remaining / w[i]);
            break;
        }
    }

    return totalValue;
}


// ============================================================
// 6. 0/1 KNAPSACK (Dynamic Programming)
// table[i][w] = best value using the first i items
//               with capacity w.
// Every item is taken fully or not at all.
//
// Time: O(n * capacity)   Space: O(n * capacity)
// ============================================================

int zeroOneKnapsack(int weights[], int values[], int itemCount, int capacity)
{
    if (itemCount > MAX_ITEMS || capacity > MAX_CAPACITY)
    {
        return 0;
    }

    int table[MAX_ITEMS + 1][MAX_CAPACITY + 1];

    // Row 0 and column 0 are 0 (no items or no capacity)
    for (int i = 0; i <= itemCount; i++)
    {
        for (int w = 0; w <= capacity; w++)
        {
            table[i][w] = 0;
        }
    }

    for (int i = 1; i <= itemCount; i++)
    {
        for (int w = 1; w <= capacity; w++)
        {
            // Option 1: do not take item i
            table[i][w] = table[i - 1][w];

            // Option 2: take item i (only if it fits)
            if (weights[i - 1] <= w)
            {
                int takeValue = values[i - 1] + table[i - 1][w - weights[i - 1]];

                if (takeValue > table[i][w])
                {
                    table[i][w] = takeValue;
                }
            }
        }
    }

    return table[itemCount][capacity];
}


// ============================================================
// 7. MAXIMUM BIPARTITE MATCHING
// Assign ambulances to emergencies so that one ambulance serves
// at most one emergency and one emergency gets at most one
// ambulance. Uses simple recursion (augmenting paths).
//
// m = ambulances, k = emergencies
// Time: O(m * k^2) in this matrix version (O(V*E) with lists)
// Auxiliary space: O(k)
// ============================================================

// Try to find an emergency for this ambulance.
// If the emergency is already taken, try to move the other
// ambulance to a different emergency.
static bool tryAssign(
    int ambulance,
    int canServe[][MATCH_SIZE],
    int emergencyCount,
    bool tried[],
    int assignedTo[])
{
    for (int e = 0; e < emergencyCount; e++)
    {
        // Ambulance cannot serve this emergency
        if (canServe[ambulance][e] == 0)
        {
            continue;
        }

        // Already tried this emergency in this round
        if (tried[e])
        {
            continue;
        }

        tried[e] = true;

        // Emergency is free, or its ambulance can move elsewhere
        if (assignedTo[e] == -1 ||
            tryAssign(assignedTo[e], canServe, emergencyCount, tried, assignedTo))
        {
            assignedTo[e] = ambulance;
            return true;
        }
    }

    return false;
}


int maximumBipartiteMatching(int canServe[][MATCH_SIZE], int ambulanceCount, int emergencyCount)
{
    if (ambulanceCount > MATCH_SIZE || emergencyCount > MATCH_SIZE)
    {
        return 0;
    }

    // assignedTo[e] = ambulance given to emergency e (-1 = none yet)
    int assignedTo[MATCH_SIZE];

    for (int e = 0; e < emergencyCount; e++)
    {
        assignedTo[e] = -1;
    }

    int matches = 0;

    for (int a = 0; a < ambulanceCount; a++)
    {
        bool tried[MATCH_SIZE];

        for (int e = 0; e < emergencyCount; e++)
        {
            tried[e] = false;
        }

        if (tryAssign(a, canServe, emergencyCount, tried, assignedTo))
        {
            matches++;
        }
    }

    return matches;
}
