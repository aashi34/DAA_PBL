#ifndef HOSPITAL_H
#define HOSPITAL_H

#include "models.h"
#include "routing.h"

const int HOSPITAL_COUNT = 3;

// Hospital data
extern Hospital hospitals[HOSPITAL_COUNT];


// Result of hospital selection
struct HospitalResult
{
    bool found;

    int hospitalIndex;

    RouteResult route;
};


// Check whether hospital provides required facility
bool meetsRequirement(
    const Hospital& hospital,
    int requirement
);


// Find nearest suitable hospital
HospitalResult findBestHospital(
    int patientLocation,
    int requirement
);


// Use one hospital bed
void admitPatient(int hospitalIndex);

#endif
