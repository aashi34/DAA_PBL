#ifndef MODELS_H
#define MODELS_H

#include <string>
using namespace std;

// Total city locations
const int N = 8;

// Represents one ambulance
struct Ambulance
{
    int id;
    int location;
    bool available;
};

// Represents one hospital
struct Hospital
{
    int id;
    string name;
    int location;
    int beds;
    bool trauma;
    bool icu;
};

// Represents one emergency
struct Emergency
{
    int location;
    int severity;       // 1=Low, 2=Medium, 3=High, 4=Critical
    int requirement;    // 1=General, 2=Trauma, 3=ICU
};

#endif
