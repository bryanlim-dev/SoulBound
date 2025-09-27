#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <unordered_map>
#include <string>

// Forward declaration
class ResourceManager;

enum AUDIO_RESULT
{
    AUDIO_SUCCESS,
    AUDIO_FAIL_INIT,
    AUDIO_FAIL_SOUND_NOT_FOUND,
    AUDIO_FAIL_ERROR
};

class AudioSystem
{
private:
    static AudioSystem* instance;

    FMOD::System* fmodSystem;
    ResourceManager* resourceManager;

    // Track active channels separately
    std::unordered_map<std::size_t, FMOD::Channel*> sfxChannels;
    std::unordered_map<std::size_t, FMOD::Channel*> musicChannels;

    bool initialized;

    // Private constructor for singleton
    AudioSystem();

public:
    ~AudioSystem();

    // Singleton methods
    static AudioSystem* GetInstance();
    static void DestroyInstance();

    // Core methods
    bool Initialize();
    void Update();
    void SetResourceManager(ResourceManager* rm);

    // Loading
    std::size_t LoadSFX(const std::string& path);
    std::size_t LoadMusic(const std::string& path, bool loop = true);

    // Playback control
    bool PlaySound(std::size_t soundKey);
    bool StopSound(std::size_t soundKey);
    void StopAllSounds();
    bool PauseSound(std::size_t soundKey, bool pause);

    // Getters
    FMOD::System* GetFMODSystem() { return fmodSystem; }
    bool IsInitialized() { return initialized; }
};
