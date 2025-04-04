#include "SilverMusic.hpp"
#include <fstream>
#include <iostream>
#include <cstring>


AudioPlayer::AudioPlayer(const std::string& filePath) 
    : filePath(filePath),
      hWaveOut(NULL),
      audioData(nullptr),
      dataSize(0),
      isPlaying(false),
      isPaused(false) {
    memset(&waveFormat, 0, sizeof(WAVEFORMATEX));
    memset(&waveHeader, 0, sizeof(WAVEHDR));
}

AudioPlayer::~AudioPlayer() {
    Stop();
    Cleanup();
}

bool AudioPlayer::LoadWaveFile() {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }

    // Read RIFF header
    char riffHeader[4];
    file.seekg(0);
    file.read(riffHeader, 4);
    if (strncmp(riffHeader, "RIFF", 4) != 0) {
        std::cerr << "Not a valid WAV file" << std::endl;
        return false;
    }

    // Skip to format chunk
    file.seekg(16);
    file.read(reinterpret_cast<char*>(&waveFormat), sizeof(WAVEFORMATEX));

    // Find data chunk
    char chunkHeader[4];
    DWORD chunkSize;
    while (true) {
        file.read(chunkHeader, 4);
        file.read(reinterpret_cast<char*>(&chunkSize), 4);
        
        if (strncmp(chunkHeader, "data", 4) == 0) {
            break;
        }
        
        file.seekg(chunkSize, std::ios::cur);
        if (file.eof()) {
            std::cerr << "No data chunk found" << std::endl;
            return false;
        }
    }

    // Read audio data
    dataSize = chunkSize;
    audioData.reset(new BYTE[dataSize]);
    file.read(reinterpret_cast<char*>(audioData.get()), dataSize);

    return true;
}

void AudioPlayer::Play() {
    Stop();
    
    if (!LoadWaveFile()) {
        return;
    }

    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio device" << std::endl;
        return;
    }

    waveHeader.lpData = reinterpret_cast<LPSTR>(audioData.get());
    waveHeader.dwBufferLength = dataSize;
    waveHeader.dwFlags = 0;

    if (waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to prepare header" << std::endl;
        return;
    }

    isPlaying = true;
    isPaused = false;

    if (waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to start playback" << std::endl;
        isPlaying = false;
        return;
    }

    // Wait for playback to complete
    while (isPlaying && (waveHeader.dwFlags & WHDR_DONE) == 0) {
        Sleep(100);
    }

    Stop();
}

void AudioPlayer::Stop() {
    if (hWaveOut && isPlaying) {
        waveOutReset(hWaveOut);
        isPlaying = false;
    }
}

void AudioPlayer::SetVolume(DWORD newVolume) {
    if (hWaveOut) {
        // Volume is 0-0xFFFF (left/right channels)
        DWORD volume = (newVolume << 16) | newVolume;
        waveOutSetVolume(hWaveOut, volume);
    }
}

void AudioPlayer::Pause() {
    if (hWaveOut && isPlaying && !isPaused) {
        waveOutPause(hWaveOut);
        isPaused = true;
    }
}

void AudioPlayer::Resume() {
    if (hWaveOut && isPlaying && isPaused) {
        waveOutRestart(hWaveOut);
        isPaused = false;
    }
}

void AudioPlayer::Cleanup() {
    if (hWaveOut) {
        if (waveHeader.dwFlags & WHDR_PREPARED) {
            waveOutUnprepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        }
        waveOutClose(hWaveOut);
        hWaveOut = NULL;
    }
    audioData.reset();
    dataSize = 0;
}