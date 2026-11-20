#include <windows.h>
#include <string>
#include <sstream>
#include <cstring>

#include "../include/models.h"
#include "../include/routing.h"
#include "../include/ambulance.h"
#include "../include/hospital.h"

using namespace std;

// ============================================================
// IMPORTANT FOR VIVA:
// This file only DRAWS the window and reads the buttons.
// It has NO algorithm. It calls the same functions as the
// console program:
//
//   dijkstra()             (routing.cpp)   - find a route
//   blockRoad()            (routing.cpp)   - close a road
//   unblockRoad()          (routing.cpp)   - open a road
//   findNearestAmbulance() (ambulance.cpp) - pick ambulance
//   findBestHospital()     (hospital.cpp)  - pick hospital
// ============================================================


// ---------------- Button / box IDs ----------------

#define ID_SOURCE       101
#define ID_DESTINATION  102
#define ID_EMERGENCY    103
#define ID_SEVERITY     104
#define ID_REQUIREMENT  105

#define BTN_ROUTE       201
#define BTN_BLOCK       202
#define BTN_UNBLOCK     203
#define BTN_DISPATCH    204
#define BTN_RESET       205


// ---------------- Window controls ----------------

HWND sourceBox;
HWND destinationBox;
HWND emergencyBox;
HWND severityBox;
HWND requirementBox;

HWND routeLabel;        // text under the route buttons
HWND dispatchLabel;     // dispatch result text
HWND ambulanceLabel;    // list of ambulances
HWND hospitalLabel;     // list of hospitals


// ---------------- Where each location is drawn ----------------

int nodeX[N] = { 120, 310, 360, 150, 410, 210, 460, 650 };
int nodeY[N] = { 150, 100, 210, 300, 330, 430, 450, 330 };

// The 11 roads (same roads as graph[][]): road i joins roadFrom[i] and roadTo[i]
int roadFrom[11] = { 0, 0, 0, 1, 2, 3, 3, 4, 5, 5, 6 };
int roadTo[11]   = { 1, 2, 3, 2, 4, 4, 5, 6, 6, 7, 7 };


// ---------------- Names shown in the dropdowns ----------------

const char* severityText[4] = { "Low", "Medium", "High", "Critical" };
const char* requirementText[3] = { "General", "Trauma", "ICU" };


// ---------------- What the map is showing ----------------

RouteResult greenRoute;      // hospital route (or manual route)
bool greenShown = false;

RouteResult orangeRoute;     // ambulance -> patient route
bool orangeShown = false;

int emergencyNode = -1;      // -1 = no emergency on the map

// Copy of the starting data, used by "Reset System"
Ambulance startAmbulances[AMBULANCE_COUNT];
Hospital startHospitals[HOSPITAL_COUNT];


// ============================================================
// Small helpers to create window controls
// ============================================================

HWND makeLabel(HWND parent, const char* text, int x, int y, int w, int h)
{
    return CreateWindowA("STATIC", text, WS_VISIBLE | WS_CHILD,
                         x, y, w, h, parent, NULL, NULL, NULL);
}

HWND makeCombo(HWND parent, int id, int x, int y, int w)
{
    return CreateWindowA("COMBOBOX", "",
                         WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                         x, y, w, 250, parent,
                         (HMENU)(INT_PTR)id, NULL, NULL);
}

void makeButton(HWND parent, const char* text, int id, int x, int y, int w, int h)
{
    CreateWindowA("BUTTON", text, WS_VISIBLE | WS_CHILD,
                  x, y, w, h, parent, (HMENU)(INT_PTR)id, NULL, NULL);
}

void addItem(HWND box, const char* text)
{
    SendMessageA(box, CB_ADDSTRING, 0, (LPARAM)text);
}

void addLocations(HWND box)
{
    for (int i = 0; i < N; i++)
    {
        addItem(box, locationNames[i].c_str());
    }
}

// Which item is selected in a dropdown (0, 1, 2 ...)
int getSelection(HWND box)
{
    return (int)SendMessageA(box, CB_GETCURSEL, 0, 0);
}


// ============================================================
// Route as text: "Clock Tower -> Survey Chowk -> ISBT"
// ============================================================

string routeToText(const RouteResult& route)
{
    stringstream text;

    for (int i = 0; i < route.pathLength; i++)
    {
        text << locationNames[route.path[i]];

        if (i < route.pathLength - 1)
        {
            text << " -> ";
        }
    }

    return text.str();
}


// ============================================================
// Is the road u-v part of this route? (used to colour it)
// ============================================================

bool isOnRoute(const RouteResult& route, bool shown, int u, int v)
{
    if (!shown)
    {
        return false;
    }

    for (int i = 0; i < route.pathLength - 1; i++)
    {
        int a = route.path[i];
        int b = route.path[i + 1];

        if ((a == u && b == v) || (a == v && b == u))
        {
            return true;
        }
    }

    return false;
}


// ============================================================
// Update the ambulance and hospital lists (REAL data)
// ============================================================

void refreshStatus()
{
    stringstream a;

    for (int i = 0; i < AMBULANCE_COUNT; i++)
    {
        a << "A" << ambulances[i].id << " | "
          << locationNames[ambulances[i].location] << " | "
          << (ambulances[i].available ? "AVAILABLE" : "BUSY") << "\r\n";
    }

    SetWindowTextA(ambulanceLabel, a.str().c_str());


    stringstream h;

    for (int i = 0; i < HOSPITAL_COUNT; i++)
    {
        h << hospitals[i].name
          << " | Beds: " << hospitals[i].beds
          << " | Trauma: " << (hospitals[i].trauma ? "Y" : "N")
          << " | ICU: " << (hospitals[i].icu ? "Y" : "N") << "\r\n";
    }

    SetWindowTextA(hospitalLabel, h.str().c_str());
}


// ============================================================
// BUTTON: FIND ROUTE  (calls dijkstra)
// ============================================================

void findRoute(HWND hwnd)
{
    int source = getSelection(sourceBox);
    int destination = getSelection(destinationBox);

    if (source == destination)
    {
        MessageBoxA(hwnd, "Source and destination cannot be the same.",
                    "Input Error", MB_OK | MB_ICONWARNING);
        return;
    }

    RouteResult result = dijkstra(source, destination);

    orangeShown = false;
    greenShown = result.found;
    greenRoute = result;

    if (!result.found)
    {
        SetWindowTextA(routeLabel, "No available route.");
    }
    else
    {
        stringstream text;

        text << "Algorithm: Dijkstra\r\n"
             << "Route: " << routeToText(result) << "\r\n"
             << "Distance: " << result.distance << " km";

        SetWindowTextA(routeLabel, text.str().c_str());
    }

    InvalidateRect(hwnd, NULL, TRUE);       // redraw the map
}


// ============================================================
// BUTTONS: BLOCK ROAD / UNBLOCK ROAD
// ============================================================

void changeRoad(HWND hwnd, bool block)
{
    int source = getSelection(sourceBox);
    int destination = getSelection(destinationBox);

    if (!roadExists(source, destination))
    {
        MessageBoxA(hwnd, "There is no direct road between these locations.",
                    "Road Error", MB_OK | MB_ICONWARNING);
        return;
    }

    if (block)
    {
        if (blocked[source][destination])
        {
            MessageBoxA(hwnd, "This road is already blocked.",
                        "Road Status", MB_OK | MB_ICONINFORMATION);
            return;
        }

        blockRoad(source, destination);
        SetWindowTextA(routeLabel, "Road blocked. Press Find Route to reroute.");
    }
    else
    {
        if (!blocked[source][destination])
        {
            MessageBoxA(hwnd, "This road is already open.",
                        "Road Status", MB_OK | MB_ICONINFORMATION);
            return;
        }

        unblockRoad(source, destination);
        SetWindowTextA(routeLabel, "Road opened. Press Find Route to recalculate.");
    }

    greenShown = false;         // old route may not be valid any more
    orangeShown = false;

    InvalidateRect(hwnd, NULL, TRUE);
}


// ============================================================
// BUTTON: DISPATCH EMERGENCY
// Emergency -> ambulance -> hospital -> update the system
// ============================================================

void dispatchEmergency(HWND hwnd)
{
    Emergency emergency;

    // Dropdown position 0,1,2... becomes value 1,2,3...
    emergency.location = getSelection(emergencyBox);
    emergency.severity = getSelection(severityBox) + 1;
    emergency.requirement = getSelection(requirementBox) + 1;

    emergencyNode = emergency.location;
    greenShown = false;
    orangeShown = false;


    // Step 1: nearest available ambulance (Dijkstra for each)
    AmbulanceResult ambulanceResult = findNearestAmbulance(emergency.location);

    if (!ambulanceResult.found)
    {
        SetWindowTextA(dispatchLabel, "DISPATCH FAILED\r\nNo available ambulance.");
        InvalidateRect(hwnd, NULL, TRUE);
        return;
    }


    // Step 2: nearest suitable hospital (filter, then Dijkstra)
    HospitalResult hospitalResult =
        findBestHospital(emergency.location, emergency.requirement);

    if (!hospitalResult.found)
    {
        SetWindowTextA(dispatchLabel, "DISPATCH FAILED\r\nNo suitable hospital.");
        InvalidateRect(hwnd, NULL, TRUE);
        return;
    }

    int a = ambulanceResult.ambulanceIndex;
    int h = hospitalResult.hospitalIndex;


    // Step 3: change the system (ambulance busy, one bed used)
    dispatchAmbulance(a);
    admitPatient(h);


    // Step 4: show the result
    stringstream text;

    text << "DISPATCH COMPLETE\r\n"
         << "Emergency: " << locationNames[emergency.location]
         << " | " << severityText[emergency.severity - 1]
         << " | " << requirementText[emergency.requirement - 1] << "\r\n\r\n"

         << "AMBULANCE: A" << ambulances[a].id << " ("
         << ambulanceResult.route.distance << " km)\r\n"
         << routeToText(ambulanceResult.route) << "\r\n\r\n"

         << "HOSPITAL: " << hospitals[h].name << " ("
         << hospitalResult.route.distance << " km, "
         << hospitals[h].beds << " beds left)\r\n"
         << routeToText(hospitalResult.route);

    SetWindowTextA(dispatchLabel, text.str().c_str());

    orangeRoute = ambulanceResult.route;
    orangeShown = true;

    greenRoute = hospitalResult.route;
    greenShown = true;

    refreshStatus();
    InvalidateRect(hwnd, NULL, TRUE);
}


// ============================================================
// BUTTON: RESET SYSTEM
// ============================================================

void resetSystem(HWND hwnd)
{
    for (int i = 0; i < AMBULANCE_COUNT; i++)
    {
        ambulances[i] = startAmbulances[i];
    }

    for (int i = 0; i < HOSPITAL_COUNT; i++)
    {
        hospitals[i] = startHospitals[i];
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            blocked[i][j] = false;
        }
    }

    greenShown = false;
    orangeShown = false;
    emergencyNode = -1;

    SetWindowTextA(routeLabel, "No route selected.");
    SetWindowTextA(dispatchLabel, "No emergency dispatched.");

    refreshStatus();
    InvalidateRect(hwnd, NULL, TRUE);
}


// ============================================================
// DRAWING THE MAP
// ============================================================

// Draw one road with its distance
void drawRoad(HDC hdc, int u, int v)
{
    COLORREF color = RGB(120, 125, 135);    // normal road: grey
    int width = 2;

    if (blocked[u][v])                              // blocked: red
    {
        color = RGB(210, 45, 45);
        width = 3;
    }
    else if (isOnRoute(greenRoute, greenShown, u, v))       // hospital route: green
    {
        color = RGB(0, 150, 80);
        width = 6;
    }
    else if (isOnRoute(orangeRoute, orangeShown, u, v))     // ambulance route: orange
    {
        color = RGB(235, 140, 0);
        width = 5;
    }

    HPEN pen = CreatePen(PS_SOLID, width, color);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    MoveToEx(hdc, nodeX[u], nodeY[u], NULL);
    LineTo(hdc, nodeX[v], nodeY[v]);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    // Distance in the middle of the road
    int middleX = (nodeX[u] + nodeX[v]) / 2;
    int middleY = (nodeY[u] + nodeY[v]) / 2;

    stringstream text;
    text << graph[u][v] << " km";
    string label = text.str();

    TextOutA(hdc, middleX, middleY, label.c_str(), (int)label.length());

    if (blocked[u][v])
    {
        SetTextColor(hdc, RGB(210, 45, 45));
        TextOutA(hdc, middleX + 4, middleY - 16, "X BLOCKED", 9);
        SetTextColor(hdc, RGB(0, 0, 0));
    }
}


// Draw one location: blue circle = hospital, dark circle = other
void drawNode(HDC hdc, int node)
{
    bool isHospital = false;

    for (int i = 0; i < HOSPITAL_COUNT; i++)
    {
        if (hospitals[i].location == node)
        {
            isHospital = true;
        }
    }

    HBRUSH brush;

    if (isHospital)
        brush = CreateSolidBrush(RGB(55, 120, 220));
    else
        brush = CreateSolidBrush(RGB(35, 40, 50));

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    Ellipse(hdc, nodeX[node] - 9, nodeY[node] - 9,
                 nodeX[node] + 9, nodeY[node] + 9);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);

    // Red ring around the emergency location
    if (node == emergencyNode)
    {
        HPEN ringPen = CreatePen(PS_SOLID, 3, RGB(210, 45, 45));
        HPEN oldPen = (HPEN)SelectObject(hdc, ringPen);
        HGDIOBJ oldBrush2 = SelectObject(hdc, GetStockObject(NULL_BRUSH));

        Ellipse(hdc, nodeX[node] - 17, nodeY[node] - 17,
                     nodeX[node] + 17, nodeY[node] + 17);

        SelectObject(hdc, oldBrush2);
        SelectObject(hdc, oldPen);
        DeleteObject(ringPen);
    }

    // Name under the location
    TextOutA(hdc, nodeX[node] - 45, nodeY[node] + 15,
             locationNames[node].c_str(), (int)locationNames[node].length());
}


void drawCityMap(HDC hdc)
{
    // Roads first, then locations on top
    for (int i = 0; i < 11; i++)
    {
        drawRoad(hdc, roadFrom[i], roadTo[i]);
    }

    for (int i = 0; i < N; i++)
    {
        drawNode(hdc, i);
    }

    // Ambulance labels above their location
    int placed[N] = { 0 };

    for (int i = 0; i < AMBULANCE_COUNT; i++)
    {
        int where = ambulances[i].location;

        stringstream text;
        text << "A" << ambulances[i].id;

        if (ambulances[i].available)
        {
            SetTextColor(hdc, RGB(0, 130, 70));
        }
        else
        {
            text << " (busy)";
            SetTextColor(hdc, RGB(130, 130, 130));
        }

        string label = text.str();

        TextOutA(hdc, nodeX[where] - 20, nodeY[where] - 32 - 15 * placed[where],
                 label.c_str(), (int)label.length());

        placed[where]++;
    }

    SetTextColor(hdc, RGB(0, 0, 0));
}


// ============================================================
// WINDOW PROCEDURE: Windows calls this for every event
// ============================================================

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // ---------- Window is created: build all controls ----------
        case WM_CREATE:
        {
            // Remember the starting data for "Reset System"
            for (int i = 0; i < AMBULANCE_COUNT; i++)
                startAmbulances[i] = ambulances[i];

            for (int i = 0; i < HOSPITAL_COUNT; i++)
                startHospitals[i] = hospitals[i];

            makeLabel(hwnd, "SMART EMERGENCY AMBULANCE ROUTING & HOSPITAL ALLOCATION",
                      25, 15, 750, 25);
            makeLabel(hwnd, "CITY NETWORK", 25, 85, 200, 25);

            // Left side: route management
            makeLabel(hwnd, "ROUTE MANAGEMENT", 25, 525, 200, 25);

            makeLabel(hwnd, "Source:", 25, 560, 70, 25);
            sourceBox = makeCombo(hwnd, ID_SOURCE, 90, 555, 190);

            makeLabel(hwnd, "Destination:", 300, 560, 90, 25);
            destinationBox = makeCombo(hwnd, ID_DESTINATION, 390, 555, 190);

            addLocations(sourceBox);
            addLocations(destinationBox);
            SendMessageA(sourceBox, CB_SETCURSEL, 0, 0);        // Clock Tower
            SendMessageA(destinationBox, CB_SETCURSEL, 6, 0);   // City Hospital

            makeButton(hwnd, "Find Route",   BTN_ROUTE,   25, 600, 130, 38);
            makeButton(hwnd, "Block Road",   BTN_BLOCK,   170, 600, 130, 38);
            makeButton(hwnd, "Unblock Road", BTN_UNBLOCK, 315, 600, 130, 38);
            makeButton(hwnd, "Reset System", BTN_RESET,   460, 600, 130, 38);

            routeLabel = makeLabel(hwnd, "No route selected.", 25, 655, 700, 80);

            // Right side: report emergency
            makeLabel(hwnd, "REPORT EMERGENCY", 750, 85, 300, 25);

            makeLabel(hwnd, "Location:", 750, 125, 150, 25);
            emergencyBox = makeCombo(hwnd, ID_EMERGENCY, 900, 120, 220);
            addLocations(emergencyBox);
            SendMessageA(emergencyBox, CB_SETCURSEL, 4, 0);     // ISBT

            makeLabel(hwnd, "Severity:", 750, 170, 100, 25);
            severityBox = makeCombo(hwnd, ID_SEVERITY, 900, 165, 220);
            for (int i = 0; i < 4; i++) addItem(severityBox, severityText[i]);
            SendMessageA(severityBox, CB_SETCURSEL, 3, 0);      // Critical

            makeLabel(hwnd, "Requirement:", 750, 215, 120, 25);
            requirementBox = makeCombo(hwnd, ID_REQUIREMENT, 900, 210, 220);
            for (int i = 0; i < 3; i++) addItem(requirementBox, requirementText[i]);
            SendMessageA(requirementBox, CB_SETCURSEL, 1, 0);   // Trauma

            makeButton(hwnd, "DISPATCH EMERGENCY", BTN_DISPATCH, 750, 260, 370, 45);

            // Right side: status lists (updated from the real data)
            makeLabel(hwnd, "AMBULANCES", 750, 330, 200, 25);
            ambulanceLabel = makeLabel(hwnd, "", 750, 360, 370, 60);

            makeLabel(hwnd, "HOSPITALS", 750, 430, 200, 25);
            hospitalLabel = makeLabel(hwnd, "", 750, 460, 400, 60);

            makeLabel(hwnd, "DISPATCH RESULT", 750, 535, 250, 25);
            dispatchLabel = makeLabel(hwnd, "No emergency dispatched.",
                                      750, 565, 400, 200);

            refreshStatus();
            break;
        }

        // ---------- Window needs drawing: draw the map ----------
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            SetBkMode(hdc, TRANSPARENT);

            drawCityMap(hdc);

            const char* legend1 =
                "Green = Hospital route | Orange = Ambulance route | Red + X = Blocked road";
            const char* legend2 =
                "Blue circle = Hospital | Red ring = Emergency | A101 = Ambulance";

            TextOutA(hdc, 25, 485, legend1, (int)strlen(legend1));
            TextOutA(hdc, 25, 502, legend2, (int)strlen(legend2));

            EndPaint(hwnd, &ps);
            break;
        }

        // ---------- A button was pressed ----------
        case WM_COMMAND:
        {
            int button = LOWORD(wParam);

            if (button == BTN_ROUTE)         findRoute(hwnd);
            else if (button == BTN_BLOCK)    changeRoad(hwnd, true);
            else if (button == BTN_UNBLOCK)  changeRoad(hwnd, false);
            else if (button == BTN_DISPATCH) dispatchEmergency(hwnd);
            else if (button == BTN_RESET)    resetSystem(hwnd);

            break;
        }

        // ---------- Window is closed ----------
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }

        default:
        {
            return DefWindowProcA(hwnd, message, wParam, lParam);
        }
    }

    return 0;
}


// ============================================================
// WINMAIN: where the program starts
// ============================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "DAAEmergencySystem";

    // Describe the window
    WNDCLASSA windowClass = {};
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = CLASS_NAME;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&windowClass);

    // Create the window
    HWND hwnd = CreateWindowExA(
        0, CLASS_NAME,
        "Smart Emergency Ambulance Routing & Hospital Allocation",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 850,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Keep running and sending events to WindowProcedure until closed
    MSG message = {};

    while (GetMessageA(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return 0;
}
