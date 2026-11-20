#include "../include/routing.h"

// ============================================================
// CITY LOCATIONS
// ============================================================

string locationNames[N] =
{
    "Clock Tower",
    "Rajpur Road",
    "Survey Chowk",
    "Ballupur",
    "ISBT",
    "Prem Nagar",
    "City Hospital",
    "Max Hospital"
};


// ============================================================
// WEIGHTED CITY GRAPH
// Adjacency Matrix
// ============================================================

int graph[N][N] =
{
    {0,   4,   2,   6,   INF, INF, INF, INF},
    {4,   0,   3,   INF, INF, INF, INF, INF},
    {2,   3,   0,   INF, 4,   INF, INF, INF},
    {6,   INF, INF, 0,   3,   5,   INF, INF},
    {INF, INF, 4,   3,   0,   INF, 2,   INF},
    {INF, INF, INF, 5,   INF, 0,   4,   6},
    {INF, INF, INF, INF, 2,   4,   0,   6},
    {INF, INF, INF, INF, INF, 6,   6,   0}
};


// ============================================================
// ROAD STATUS
// false = open
// true  = blocked
// ============================================================

bool blocked[N][N] = {false};


// ============================================================
// CHECK IF DIRECT ROAD EXISTS
// ============================================================

bool roadExists(int source, int destination)
{
    if (source < 0 || source >= N)
        return false;

    if (destination < 0 || destination >= N)
        return false;

    if (source == destination)
        return false;

    return graph[source][destination] != INF;
}


// ============================================================
// BLOCK ROAD
// ============================================================

void blockRoad(int source, int destination)
{
    if (!roadExists(source, destination))
        return;

    blocked[source][destination] = true;
    blocked[destination][source] = true;
}


// ============================================================
// UNBLOCK ROAD
// ============================================================

void unblockRoad(int source, int destination)
{
    if (!roadExists(source, destination))
        return;

    blocked[source][destination] = false;
    blocked[destination][source] = false;
}


// ============================================================
// FIND NEAREST UNVISITED NODE
// "static" = this helper is private to routing.cpp, so it cannot
// clash with a function of the same name in another file.
// ============================================================

static int findMinimumDistance(int distance[], bool visited[])
{
    int minimum = INF;
    int minimumNode = -1;

    for (int i = 0; i < N; i++)
    {
        if (!visited[i] && distance[i] < minimum)
        {
            minimum = distance[i];
            minimumNode = i;
        }
    }

    return minimumNode;
}


// ============================================================
// DIJKSTRA SHORTEST PATH
// ============================================================

RouteResult dijkstra(int source, int destination)
{
    RouteResult result;

    result.found = false;
    result.distance = INF;
    result.pathLength = 0;

    // Validate input
    if (source < 0 || source >= N ||
        destination < 0 || destination >= N)
    {
        return result;
    }


    int distance[N];
    bool visited[N];
    int parent[N];


    // --------------------------------------------------------
    // INITIALIZATION
    // --------------------------------------------------------

    for (int i = 0; i < N; i++)
    {
        distance[i] = INF;
        visited[i] = false;
        parent[i] = -1;
    }

    distance[source] = 0;


    // --------------------------------------------------------
    // DIJKSTRA
    // --------------------------------------------------------

    for (int count = 0; count < N - 1; count++)
    {
        int u = findMinimumDistance(distance, visited);

        if (u == -1)
            break;

        visited[u] = true;


        // Relax all possible neighbours
        for (int v = 0; v < N; v++)
        {
            if (
                !visited[v] &&
                graph[u][v] != INF &&
                !blocked[u][v] &&
                distance[u] != INF &&
                distance[u] + graph[u][v] < distance[v]
            )
            {
                distance[v] =
                    distance[u] + graph[u][v];

                parent[v] = u;
            }
        }
    }


    // --------------------------------------------------------
    // NO ROUTE
    // --------------------------------------------------------

    if (distance[destination] == INF)
    {
        return result;
    }


    // --------------------------------------------------------
    // RECONSTRUCT PATH
    // --------------------------------------------------------

    int reversePath[N];
    int reverseLength = 0;

    int current = destination;

    while (current != -1)
    {
        reversePath[reverseLength] = current;
        reverseLength++;

        current = parent[current];
    }


    // Reverse it into correct source -> destination order

    for (int i = reverseLength - 1; i >= 0; i--)
    {
        result.path[result.pathLength] =
            reversePath[i];

        result.pathLength++;
    }


    result.found = true;
    result.distance = distance[destination];

    return result;
}
