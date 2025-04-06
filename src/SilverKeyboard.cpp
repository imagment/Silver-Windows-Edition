#include "SilverKeyboard.hpp"
#include <windows.h>


void InitializeKeyboardModule() {
    if (!isInitialized) {
        // Initialize all key states to false
        for (int i = 0; i < 256; i++) {
            keyStates[i] = false;
            keyDownStates[i] = false;
            keyUpStates[i] = false;
        }
        isInitialized = true;
        setNonBlockingMode(true);
    }
}

void PollEvents() {
    if (!isInitialized) {
        InitializeKeyboardModule();
    }

    for (int i = 0; i < 256; i++) {
        bool currentState = (GetAsyncKeyState(i) & 0x8000) != 0;
        
        // Detect key down (transition from not pressed to pressed)
        if (currentState && !keyStates[i]) {
            keyDownStates[i] = true;
        } else {
            keyDownStates[i] = false;
        }

        // Detect key up (transition from pressed to not pressed)
        if (!currentState && keyStates[i]) {
            keyUpStates[i] = true;
        } else {
            keyUpStates[i] = false;
        }

        keyStates[i] = currentState;
    }
}

bool IsKey(int key) {
    return keyStates[key];
}

bool IsKeyDown(int key) {
    return keyDownStates[key];
}

bool IsKeyUp(int key) {
    return keyUpStates[key];
}