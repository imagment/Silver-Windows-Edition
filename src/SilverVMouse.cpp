#include "Silver.hpp"
#include "SilverVMouse.hpp"
#include "SilverKeyboard.hpp"
#include <windows.h>

using namespace std;

// Global variables
bool VMouse = false;
HANDLE hVMouseThread = NULL;
CRITICAL_SECTION mouseCS;

int mouseKey;
int cursorPositionX = 0;
int cursorPositionY = 0;
std::string mouseIcon = "O";
bool hideMouse = false;

// Virtual mouse movement function
DWORD WINAPI VMouseOn(LPVOID lpParam) {
    int* keys = (int*)lpParam;
    int leftKey = keys[0], rightKey = keys[1], upKey = keys[2], downKey = keys[3];

    while (VMouse) {
        EnterCriticalSection(&mouseCS);

        if (IsKey(leftKey)) {
            cursorPositionX--;
        }
        if (IsKey(rightKey)) {
            cursorPositionX++;
        }
        if (IsKey(upKey)) {
            cursorPositionY--;
        }
        if (IsKey(downKey)) {
            cursorPositionY++;
        }

        LeaveCriticalSection(&mouseCS);
        Sleep(10);  // Reduce CPU usage
    }

    return 0;
}

// Start virtual mouse
void StartVMouse(int leftKey, int rightKey, int upKey, int downKey, int clickKey) {
    hideMouse = false;
    if (VMouse)
        return;

    VMouse = true;
    mouseKey = clickKey;

    static int keys[4] = { leftKey, rightKey, upKey, downKey };
    hVMouseThread = CreateThread(NULL, 0, VMouseOn, keys, 0, NULL);
}

// Stop virtual mouse
void StopVMouse() {
    hideMouse = true;
    VMouse = false;

    if (hVMouseThread) {
        WaitForSingleObject(hVMouseThread, INFINITE);
        CloseHandle(hVMouseThread);
        hVMouseThread = NULL;
    }
}

// Check if the mouse was clicked
bool WasMouseClicked() {
    return IsKey(mouseKey);
}
