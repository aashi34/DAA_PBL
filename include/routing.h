#ifndef ROUTING_H
#define ROUTING_H

#include "models.h"

const int INF = 99999;

// City information
extern int graph[N][N];
extern bool blocked[N][N];
extern std::string locationNames[N];

// Result produced by shortest-path algorithm
struct RouteResult
{
    bool found;
    int distance;

    int path[N];
    int pathLength;
};

// Routing functions
RouteResult dijkstra(int source, int destination);

void blockRoad(int source, int destination);

void unblockRoad(int source, int destination);

bool roadExists(int source, int destination);

#endif
