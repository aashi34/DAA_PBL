#include "../include/ambulance.h"


// ============================================================
// AMBULANCE DATA
// ============================================================

Ambulance ambulances[AMBULANCE_COUNT] =
{
    {101, 0, true},      // Clock Tower
    {102, 3, true},      // Ballupur
    {103, 5, false}      // Prem Nagar - currently busy
};


// ============================================================
// FIND NEAREST AVAILABLE AMBULANCE
// ============================================================

AmbulanceResult findNearestAmbulance(int emergencyLocation)
{
    AmbulanceResult result;

    result.found = false;
    result.ambulanceIndex = -1;
    result.route.found = false;
    result.route.distance = INF;
    result.route.pathLength = 0;

    int bestDistance = INF;


    // Check every ambulance
    for (int i = 0; i < AMBULANCE_COUNT; i++)
    {
        // Ignore busy ambulances
        if (!ambulances[i].available)
        {
            continue;
        }


        // Calculate route:
        // Ambulance location -> Emergency location

        RouteResult route =
            dijkstra(
                ambulances[i].location,
                emergencyLocation
            );


        // Ignore ambulance if emergency is unreachable
        if (!route.found)
        {
            continue;
        }


        // Is this ambulance closer than our current best?
        if (route.distance < bestDistance)
        {
            bestDistance = route.distance;

            result.found = true;

            result.ambulanceIndex = i;

            result.route = route;
        }
    }


    return result;
}


// ============================================================
// DISPATCH AMBULANCE
// ============================================================

void dispatchAmbulance(int ambulanceIndex)
{
    if (
        ambulanceIndex >= 0 &&
        ambulanceIndex < AMBULANCE_COUNT
    )
    {
        ambulances[ambulanceIndex].available = false;
    }
}


// ============================================================
// RELEASE AMBULANCE
// ============================================================

void releaseAmbulance(int ambulanceIndex)
{
    if (
        ambulanceIndex >= 0 &&
        ambulanceIndex < AMBULANCE_COUNT
    )
    {
        ambulances[ambulanceIndex].available = true;
    }
}
