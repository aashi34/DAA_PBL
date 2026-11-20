#include <iostream>

#include "../include/models.h"
#include "../include/routing.h"
#include "../include/ambulance.h"
#include "../include/hospital.h"
#include "../include/records.h"
#include "../include/workbench.h"

using namespace std;


// ============================================================
// HELPERS
// ============================================================

static const char* severityName(int value)
{
    if (value == 1) return "LOW";
    if (value == 2) return "MEDIUM";
    if (value == 3) return "HIGH";
    return "CRITICAL";
}

static const char* requirementName(int value)
{
    if (value == 1) return "GENERAL";
    if (value == 2) return "TRAUMA";
    return "ICU";
}

// Reads a whole number between low and high.
// Returns false for letters or out-of-range numbers.
static bool readInt(int& value, int low, int high)
{
    if (!(cin >> value))
    {
        cin.clear();
        cin.ignore(1000, '\n');
        return false;
    }

    return value >= low && value <= high;
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

static void showLocations()
{
    cout << "\nCITY LOCATIONS\n";

    for (int i = 0; i < N; i++)
    {
        cout << i << ". " << locationNames[i] << "\n";
    }
}


// ============================================================
// SHOW AMBULANCES AND HOSPITALS
// ============================================================

static void showStatus()
{
    cout << "\n========== AMBULANCES ==========\n";

    for (int i = 0; i < AMBULANCE_COUNT; i++)
    {
        cout << "A" << ambulances[i].id
             << " | " << locationNames[ambulances[i].location]
             << " | " << (ambulances[i].available ? "AVAILABLE" : "BUSY")
             << "\n";
    }

    cout << "\n========== HOSPITALS ==========\n";

    for (int i = 0; i < HOSPITAL_COUNT; i++)
    {
        cout << hospitals[i].name
             << " | " << locationNames[hospitals[i].location]
             << " | Beds: " << hospitals[i].beds
             << " | Trauma: " << (hospitals[i].trauma ? "YES" : "NO")
             << " | ICU: " << (hospitals[i].icu ? "YES" : "NO")
             << "\n";
    }
}


// ============================================================
// FIND A MANUAL ROUTE
// ============================================================

static void findManualRoute()
{
    int source;
    int destination;

    showLocations();

    cout << "\nEnter source number: ";
    if (!readInt(source, 0, N - 1)) { cout << "\nInvalid location!\n"; return; }

    cout << "Enter destination number: ";
    if (!readInt(destination, 0, N - 1)) { cout << "\nInvalid location!\n"; return; }

    RouteResult route = dijkstra(source, destination);

    if (!route.found)
    {
        cout << "\nNo available route.\n";
        return;
    }

    cout << "\nRoute: ";
    printRoute(route);
    cout << "\nDistance: " << route.distance << " km\n";
}


// ============================================================
// BLOCK A ROAD
// After this, Dijkstra ignores the road and finds another route.
// ============================================================

static void blockSelectedRoad()
{
    int first;
    int second;

    showLocations();

    cout << "\nEnter first location number: ";
    if (!readInt(first, 0, N - 1)) { cout << "\nInvalid location!\n"; return; }

    cout << "Enter second location number: ";
    if (!readInt(second, 0, N - 1)) { cout << "\nInvalid location!\n"; return; }

    if (!roadExists(first, second))
    {
        cout << "\nThere is no direct road between these locations.\n";
        return;
    }

    if (blocked[first][second])
    {
        cout << "\nThis road is already blocked!\n";
        return;
    }

    blockRoad(first, second);

    cout << "\nROAD BLOCKED: " << locationNames[first]
         << " <X> " << locationNames[second]
         << "\nDijkstra will now avoid this road.\n";
}


// ============================================================
// REPORT EMERGENCY  (the MAIN workflow)
// Emergency -> ambulance -> patient route -> hospital -> hospital route
// ============================================================

static void reportEmergency()
{
    int location;
    int severity;
    int requirement;


    // ---------- INPUT ----------

    showLocations();

    cout << "\nEnter emergency location: ";
    if (!readInt(location, 0, N - 1))
    {
        cout << "\nInvalid emergency location!\n";
        return;
    }

    cout << "\nSeverity: 1=Low 2=Medium 3=High 4=Critical\nEnter severity: ";
    if (!readInt(severity, 1, 4))
    {
        cout << "\nInvalid severity!\n";
        return;
    }

    cout << "\nRequirement: 1=General 2=Trauma 3=ICU\nEnter requirement: ";
    if (!readInt(requirement, 1, 3))
    {
        cout << "\nInvalid medical requirement!\n";
        return;
    }

    Emergency emergency;
    emergency.location = location;
    emergency.severity = severity;
    emergency.requirement = requirement;

    // Save the emergency in the records (see records.cpp).
    // It is saved as soon as it is received, even if no ambulance
    // or hospital can be found later.
    int emergencyId = addEmergencyRecord(emergency);

    cout << "\n================================================\n";
    cout << "              EMERGENCY RECEIVED\n";
    cout << "================================================\n";

    if (emergencyId != -1)
        cout << "Emergency ID: " << emergencyId << "\n";
    else
        cout << "(Record storage is full - this emergency was not saved)\n";

    cout << "Location:    " << locationNames[emergency.location] << "\n";
    cout << "Severity:    " << severityName(emergency.severity) << "\n";
    cout << "Requirement: " << requirementName(emergency.requirement) << "\n";


    // ---------- STEP 1: CHECK EVERY AMBULANCE ----------

    cout << "\n--------- CHECKING AMBULANCES ---------\n";

    for (int i = 0; i < AMBULANCE_COUNT; i++)
    {
        cout << "A" << ambulances[i].id << " - "
             << locationNames[ambulances[i].location];

        if (!ambulances[i].available)
        {
            cout << " - BUSY\n";
            continue;
        }

        RouteResult route =
            dijkstra(ambulances[i].location, emergency.location);

        if (!route.found)
        {
            cout << " - UNREACHABLE\n";
            continue;
        }

        cout << " - Distance: " << route.distance << " km\n";
    }


    // ---------- STEP 2: SELECT NEAREST AMBULANCE ----------

    AmbulanceResult ambulanceResult =
        findNearestAmbulance(emergency.location);

    if (!ambulanceResult.found)
    {
        cout << "\nNO AVAILABLE AMBULANCE FOUND!\n";
        return;
    }

    int ambulanceIndex = ambulanceResult.ambulanceIndex;

    cout << "\n--------- AMBULANCE SELECTED ---------\n";
    cout << "Ambulance: A" << ambulances[ambulanceIndex].id << "\n";
    cout << "From: " << locationNames[ambulances[ambulanceIndex].location] << "\n";
    cout << "Distance to patient: " << ambulanceResult.route.distance << " km\n";
    cout << "Route: ";
    printRoute(ambulanceResult.route);
    cout << "\n";


    // ---------- STEP 3: CHECK EVERY HOSPITAL ----------

    cout << "\n--------- CHECKING HOSPITALS ---------\n";

    for (int i = 0; i < HOSPITAL_COUNT; i++)
    {
        cout << hospitals[i].name;

        if (hospitals[i].beds <= 0)
        {
            cout << " - REJECTED (No beds)\n";
            continue;
        }

        if (!meetsRequirement(hospitals[i], emergency.requirement))
        {
            cout << " - REJECTED (Required facility unavailable)\n";
            continue;
        }

        RouteResult route =
            dijkstra(emergency.location, hospitals[i].location);

        if (!route.found)
        {
            cout << " - REJECTED (Unreachable)\n";
            continue;
        }

        cout << " - VALID - Distance: " << route.distance << " km\n";
    }


    // ---------- STEP 4: SELECT BEST HOSPITAL ----------

    HospitalResult hospitalResult =
        findBestHospital(emergency.location, emergency.requirement);

    if (!hospitalResult.found)
    {
        cout << "\nNO SUITABLE HOSPITAL FOUND!\n";
        return;
    }

    int hospitalIndex = hospitalResult.hospitalIndex;

    cout << "\n--------- HOSPITAL SELECTED ---------\n";
    cout << "Hospital: " << hospitals[hospitalIndex].name << "\n";
    cout << "Distance from patient: " << hospitalResult.route.distance << " km\n";
    cout << "Route: ";
    printRoute(hospitalResult.route);
    cout << "\n";


    // ---------- STEP 5: COMPLETE DISPATCH ----------

    dispatchAmbulance(ambulanceIndex);
    admitPatient(hospitalIndex);

    cout << "\n================================================\n";
    cout << "              DISPATCH COMPLETE\n";
    cout << "================================================\n";
    cout << "Ambulance A" << ambulances[ambulanceIndex].id
         << " dispatched.\n";
    cout << "Patient allocated to " << hospitals[hospitalIndex].name << ".\n";
    cout << "Remaining beds: " << hospitals[hospitalIndex].beds << "\n";
}


// ============================================================
// MAIN MENU
// ============================================================

int main()
{
    int choice = 0;

    do
    {
        cout << "\n\n==========================================\n";
        cout << " SMART EMERGENCY AMBULANCE ROUTING SYSTEM\n";
        cout << "==========================================\n";
        cout << "1. Report Emergency (full dispatch)\n";
        cout << "2. Find Shortest Route\n";
        cout << "3. Block a Road\n";
        cout << "4. View Ambulances and Hospitals\n";
        cout << "5. Emergency Records\n";
        cout << "6. Algorithm Workbench\n";
        cout << "7. Exit\n";
        cout << "\nEnter your choice: ";

        if (!readInt(choice, 1, 7))
        {
            cout << "\nInvalid choice!\n";
            choice = 0;
            continue;
        }

        if (choice == 1)      reportEmergency();
        else if (choice == 2) findManualRoute();
        else if (choice == 3) blockSelectedRoad();
        else if (choice == 4) showStatus();
        else if (choice == 5) recordsMenu();
        else if (choice == 6) runWorkbench();
        else                  cout << "\nExiting system...\n";
    }
    while (choice != 7);

    return 0;
}
