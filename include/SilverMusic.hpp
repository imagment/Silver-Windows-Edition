#ifndef SILVER_MUSIC_HPP
#define SILVER_MUSIC_HPP

#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <memory>

class AudioPlayer {
public:
    explicit AudioPlayer(const std::string& filePath);
    ~AudioPlayer();

    void Play();  // Synchronous playback
    void Stop();
    void SetVolume(DWORD newVolume);  // 0-1000 scale
    void Pause();
    void Resume();

private:
    std::string filePath;
    HWAVEOUT hWaveOut;
    WAVEFORMATEX waveFormat;
    WAVEHDR waveHeader;
    std::unique_ptr<BYTE[]> audioData;
    DWORD dataSize;
    bool isPlaying;
    bool isPaused;

    bool LoadWaveFile();
    void Cleanup();
};

#endif // SILVER_AUDIOPLAYER_HPP