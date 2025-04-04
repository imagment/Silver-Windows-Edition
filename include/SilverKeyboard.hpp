#ifndef SILVER_KEYBOARD_HPP
#define SILVER_KEYBOARD_HPP

#include <windows.h>
#include <unordered_map>

// Virtual key codes (Windows VK_* codes)
#define KEY_UP VK_UP
#define KEY_DOWN VK_DOWN
#define KEY_LEFT VK_LEFT
#define KEY_RIGHT VK_RIGHT

// Function Keys
#define KEY_F1 VK_F1
#define KEY_F2 VK_F2
#define KEY_F3 VK_F3
#define KEY_F4 VK_F4
#define KEY_F5 VK_F5
#define KEY_F6 VK_F6
#define KEY_F7 VK_F7
#define KEY_F8 VK_F8
#define KEY_F9 VK_F9
#define KEY_F10 VK_F10
#define KEY_F11 VK_F11
#define KEY_F12 VK_F12

// Escape Keys
#define KEY_ESC VK_ESCAPE

// Backspace and Delete Keys
#define KEY_BACKSPACE VK_BACK
#define KEY_DELETE VK_DELETE

// Other special keys
#define KEY_ENTER VK_RETURN
#define KEY_SPACE VK_SPACE

static std::unordered_map<int, bool> keyStates;
static std::unordered_map<int, bool> keyDownStates;
static std::unordered_map<int, bool> keyUpStates;
static bool isInitialized;

// Initializes the keyboard input system
void InitializeKeyboardModule();

// Polls keyboard state (call once per frame)
void PollEvents();

// Key state checkers
bool IsKey(int key);     // Checks if a key is currently pressed
bool IsKeyDown(int key); // Checks if a key was pressed this frame
bool IsKeyUp(int key);   // Checks if a key was released this frame


#endif // SILVER_KEYBOARD_HPP