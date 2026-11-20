#include <iostream>
#include <iomanip>

#include "../include/records.h"
#include "../include/routing.h"
#include "../include/sorting_search.h"

using namespace std;


// ============================================================
// EMERGENCY RECORDS
//
// Every reported emergency is saved here. The records are stored
// in the order they arrive, so their IDs are always increasing:
// 101, 102, 103 ...  (that is why the ID array is already sorted
// and Binary Search can be used on it)
// ============================================================

EmergencyRecord records[MAX_RECORDS];
int recordCount = 0;

const int FIRST_ID = 101;


// ---------------- small helpers ----------------

static const char* severityName(int value)
{
    if (value == 1) return "Low";
    if (value == 2) return "Medium";
    if (value == 3) return "High";
    return "Critical";
}

static const char* requirementName(int value)
{
    if (value == 1) return "General";
    if (value == 2) return "Trauma";
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

static void printTableHeader()
{
    cout << "\nTotal Emergencies: " << recordCount << "\n\n";

    cout << left << setw(8) << "ID"
         << setw(16) << "Location"
         << setw(12) << "Severity"
         << "Requirement\n";

    cout << "------------------------------------------------\n";
}

static void printRecord(const EmergencyRecord& r)
{
    cout << left << setw(8) << r.id
         << setw(16) << locationNames[r.location]
         << setw(12) << severityName(r.severity)
         << requirementName(r.requirement) << "\n";
}


// ============================================================
// SAVE A NEW EMERGENCY (called by reportEmergency in main.cpp)
// ============================================================

int addEmergencyRecord(const Emergency& emergency)
{
    if (recordCount >= MAX_RECORDS)
    {
        return -1;      // storage full
    }

    records[recordCount].id = FIRST_ID + recordCount;
    records[recordCount].location = emergency.location;
    records[recordCount].severity = emergency.severity;
    records[recordCount].requirement = emergency.requirement;

    recordCount++;

    return records[recordCount - 1].id;
}


// ============================================================
// 1. VIEW ALL RECORDS (in the order they were reported)
// ============================================================

static void viewRecords()
{
    printTableHeader();

    if (recordCount == 0)
    {
        cout << "No emergencies recorded yet.\n";
        return;
    }

    for (int i = 0; i < recordCount; i++)
    {
        printRecord(records[i]);
    }
}


// ============================================================
// 2 and 3. SORT RECORDS BY SEVERITY (Critical first)
//
// mergeSort() and quickSort() sort a plain int array, so we turn
// each record into ONE number (a "key"):
//
//     key = (5 - severity) * 1000 + id
//
// Critical (4) gives 1xxx, High (3) gives 2xxx, Medium (2) gives
// 3xxx, Low (1) gives 4xxx. So sorting the keys from small to big
// puts Critical first. After sorting, id = key % 1000 tells us
// which record the key belongs to.
//
// The stored records are NOT moved; only the key array is sorted.
// ============================================================

static void sortRecords(bool useMergeSort)
{
    if (recordCount == 0)
    {
        cout << "\nNo emergencies recorded yet.\n";
        return;
    }

    int keys[MAX_RECORDS];

    for (int i = 0; i < recordCount; i++)
    {
        keys[i] = (5 - records[i].severity) * 1000 + records[i].id;
    }

    if (useMergeSort)
    {
        mergeSort(keys, 0, recordCount - 1);
        cout << "\nSorted by severity using MERGE SORT (Critical first)";
    }
    else
    {
        quickSort(keys, 0, recordCount - 1);
        cout << "\nSorted by severity using QUICK SORT (Critical first)";
    }

    printTableHeader();

    for (int i = 0; i < recordCount; i++)
    {
        int id = keys[i] % 1000;                // get the ID back from the key
        printRecord(records[id - FIRST_ID]);    // record number = ID - 101
    }

    if (useMergeSort)
    {
        cout << "\nMerge Sort:  O(n log n) time, O(n) extra space\n";
    }
    else
    {
        cout << "\nQuick Sort:  O(n log n) average, O(n^2) worst case\n";
    }
}


// ============================================================
// 4. SEARCH AN EMERGENCY ID (Binary Search)
//
// Binary search needs a sorted array. The IDs are saved in
// increasing order, so the ID array is already sorted.
// Both versions of binary search are used here.
// ============================================================

static void searchById()
{
    if (recordCount == 0)
    {
        cout << "\nNo emergencies recorded yet.\n";
        return;
    }

    // Copy the IDs into an int array (already sorted)
    int ids[MAX_RECORDS];

    for (int i = 0; i < recordCount; i++)
    {
        ids[i] = records[i].id;
    }

    int target;

    cout << "\nEnter Emergency ID to search: ";

    if (!readInt(target, 0, 99999))
    {
        cout << "\nInvalid ID!\n";
        return;
    }

    int indexIterative = iterativeBinarySearch(ids, recordCount, target);
    int indexRecursive = recursiveBinarySearch(ids, 0, recordCount - 1, target);

    if (indexIterative == -1)
    {
        cout << "\nEmergency ID " << target << " not found.\n";
        return;
    }

    cout << "\nFOUND\n";
    printTableHeader();
    printRecord(records[indexIterative]);

    cout << "\nIterative Binary Search found it at index " << indexIterative << "\n";
    cout << "Recursive Binary Search found it at index " << indexRecursive << "\n";
    cout << "Binary Search: O(log n) time\n";
}


// ============================================================
// THE EMERGENCY RECORDS MENU
// ============================================================

void recordsMenu()
{
    int choice = 0;

    do
    {
        cout << "\n========== EMERGENCY RECORDS ==========\n";
        cout << "1. View all records\n";
        cout << "2. Sort by severity (Merge Sort)\n";
        cout << "3. Sort by severity (Quick Sort)\n";
        cout << "4. Search Emergency ID (Binary Search)\n";
        cout << "5. Back\n";
        cout << "\nEnter your choice: ";

        if (!readInt(choice, 1, 5))
        {
            cout << "\nInvalid choice!\n";
            choice = 0;
            continue;
        }

        if (choice == 1)      viewRecords();
        else if (choice == 2) sortRecords(true);
        else if (choice == 3) sortRecords(false);
        else if (choice == 4) searchById();
    }
    while (choice != 5);
}
