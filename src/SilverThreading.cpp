#include "SilverThreading.hpp"
#include <iostream>
#include <windows.h>

SThread::SThread() : isPaused(false), isRunning(false), hThread(NULL) {
    InitializeCriticalSection(&queueCS);
    hQueueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);  // Auto-reset, initially non-signaled
    hPauseEvent = CreateEvent(NULL, TRUE, TRUE, NULL);    // Manual-reset, initially signaled
    hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);    // Manual-reset, initially non-signaled
}

SThread::~SThread() {
    StopThread();
    DeleteCriticalSection(&queueCS);
    CloseHandle(hQueueEvent);
    CloseHandle(hPauseEvent);
    CloseHandle(hStopEvent);
}

void SThread::Enqueue(std::function<void()> task) {
    EnterCriticalSection(&queueCS);
    taskQueue.push(task);
    LeaveCriticalSection(&queueCS);
    SetEvent(hQueueEvent);
}

void SThread::StartThread() {
    if (!isRunning) {
        isRunning = true;
        hThread = CreateThread(
            NULL,                   // Default security attributes
            0,                      // Default stack size
            ThreadWrapper,          // Thread function
            this,                   // Argument to thread function
            0,                      // Default creation flags
            NULL                    // Don't need thread ID
        );
    }
}

void SThread::StopThread() {
    if (isRunning) {
        isRunning = false;
        SetEvent(hStopEvent);

        if (hThread) {
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            hThread = NULL;
        }

        ResetEvent(hStopEvent);
        ResetEvent(hQueueEvent);
        SetEvent(hPauseEvent);  // Ensure thread isn't blocked on pause
    }
}

void SThread::PauseThread() {
    isPaused = true;
    ResetEvent(hPauseEvent);
}

void SThread::ResumeThread() {
    isPaused = false;
    SetEvent(hPauseEvent);
}

DWORD WINAPI SThread::ThreadWrapper(LPVOID lpParam) {
    SThread* pThis = static_cast<SThread*>(lpParam);
    pThis->ThreadFunction();
    return 0;
}

void SThread::ThreadFunction() {
    HANDLE waitHandles[3] = { hStopEvent, hQueueEvent, hPauseEvent };

    while (isRunning) {
        // Wait for either stop signal, new task, or pause state change
        DWORD waitResult = WaitForMultipleObjects(3, waitHandles, FALSE, INFINITE);

        if (waitResult == WAIT_OBJECT_0) {  // Stop event
            break;
        }
        else if (waitResult == WAIT_OBJECT_0 + 1) {  // Queue event
            EnterCriticalSection(&queueCS);
            if (!taskQueue.empty()) {
                auto task = taskQueue.front();
                taskQueue.pop();
                LeaveCriticalSection(&queueCS);
                
                task();
            }
            else {
                LeaveCriticalSection(&queueCS);
            }
        }
        else if (waitResult == WAIT_OBJECT_0 + 2) {  // Pause event
            if (isPaused) {
                WaitForSingleObject(hPauseEvent, INFINITE);
            }
        }
    }
}

void SThread::JoinThread() {
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
    }
}

void SThread::DetachThread() {
    if (hThread) {
        CloseHandle(hThread);
        hThread = NULL;
    }
}
