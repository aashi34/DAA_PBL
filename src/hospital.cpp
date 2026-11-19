#include "../include/hospital.h"


// ============================================================
// HOSPITAL DATA
// ============================================================

Hospital hospitals[HOSPITAL_COUNT] =
{
    // id, name, location, beds, trauma, icu

    {1, "City Hospital", 6, 0, true,  true},
    {2, "Max Hospital",  7, 8, true,  true},
    {3, "General Care",  1, 5, false, false}
};


// ============================================================
// CHECK MEDICAL REQUIREMENT
// ============================================================

bool meetsRequirement(
    const Hospital& hospital,
    int requirement
)
{
    // 1 = GENERAL (every hospital can handle it)
    if (requirement == 1)
    {
        return true;
    }


    // 2 = TRAUMA
    if (requirement == 2)
    {
        return hospital.trauma;
    }


    // 3 = ICU
    if (requirement == 3)
    {
        return hospital.icu;
    }


    // Unknown requirement
    return false;
}


// ============================================================
// FIND BEST HOSPITAL
// Filter hospitals, then pick the nearest using Dijkstra
// ============================================================

HospitalResult findBestHospital(
    int patientLocation,
    int requirement
)
{
    HospitalResult result;

    result.found = false;
    result.hospitalIndex = -1;
    result.route.found = false;
    result.route.distance = INF;
    result.route.pathLength = 0;

    int bestDistance = INF;


    for (int i = 0; i < HOSPITAL_COUNT; i++)
    {
        // CONDITION 1: hospital must have an available bed
        if (hospitals[i].beds <= 0)
        {
            continue;
        }


        // CONDITION 2: hospital must provide required facility
        if (!meetsRequirement(hospitals[i], requirement))
        {
            continue;
        }


        // CONDITION 3: hospital must be reachable
        RouteResult route =
            dijkstra(
                patientLocation,
                hospitals[i].location
            );

        if (!route.found)
        {
            continue;
        }


        // SELECT NEAREST VALID HOSPITAL
        if (route.distance < bestDistance)
        {
            bestDistance = route.distance;

            result.found = true;

            result.hospitalIndex = i;

            result.route = route;
        }
    }


    return result;
}


// ============================================================
// ADMIT PATIENT (uses one bed)
// ============================================================

void admitPatient(int hospitalIndex)
{
    if (
        hospitalIndex >= 0 &&
        hospitalIndex < HOSPITAL_COUNT &&
        hospitals[hospitalIndex].beds > 0
    )
    {
        hospitals[hospitalIndex].beds--;
    }
}
