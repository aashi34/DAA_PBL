#ifndef RECORDS_H
#define RECORDS_H

#include "models.h"

// Most emergencies we can store
const int MAX_RECORDS = 100;

// One saved emergency
struct EmergencyRecord
{
    int id;             // 101, 102, 103 ...
    int location;       // 0-7 (see locationNames)
    int severity;       // 1=Low, 2=Medium, 3=High, 4=Critical
    int requirement;    // 1=General, 2=Trauma, 3=ICU
};

// Save a new emergency. Returns its ID, or -1 if the storage is full.
int addEmergencyRecord(const Emergency& emergency);

// The "Emergency Records" menu (view, sort, search)
void recordsMenu();

#endif
