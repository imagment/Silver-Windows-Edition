#ifndef SILVER_THREADING_HPP
#define SILVER_THREADING_HPP

#include <windows.h>
#include <queue>
#include <functional>

class SThread {
public:
    SThread();
    ~SThread();

    void Enqueue(std::function<void()> task);
    void StartThread();
    void StopThread();
    void PauseThread();
    void ResumeThread();
    void JoinThread();
    void DetachThread();

private:
    void ThreadFunction();
    static DWORD WINAPI ThreadWrapper(LPVOID lpParam);

    HANDLE hThread;
    HANDLE hQueueEvent;  // Signals when tasks are available
    HANDLE hPauseEvent;  // Signals when thread should pause/resume
    HANDLE hStopEvent;   // Signals when thread should stop
    CRITICAL_SECTION queueCS;
    
    std::queue<std::function<void()>> taskQueue;
    bool isPaused;
    bool isRunning;
};

#endif // SILVER_THREADING_HPP