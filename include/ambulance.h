#ifndef AMBULANCE_H
#define AMBULANCE_H

#include "models.h"
#include "routing.h"

// Total ambulances currently in our simulation
const int AMBULANCE_COUNT = 3;

// Ambulance data
extern Ambulance ambulances[AMBULANCE_COUNT];


// Result of ambulance selection
struct AmbulanceResult
{
    bool found;

    int ambulanceIndex;

    RouteResult route;
};


// Find nearest AVAILABLE ambulance
AmbulanceResult findNearestAmbulance(int emergencyLocation);


// Mark ambulance busy after dispatch
void dispatchAmbulance(int ambulanceIndex);


// Make ambulance available again
void releaseAmbulance(int ambulanceIndex);

#endif
