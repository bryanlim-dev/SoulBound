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
    std::unordered_map<int, FMOD::Channel*> activeChannels;
    int nextChannelId;
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

    // Simple audio playback
    bool PlaySound(std::size_t soundKey);
    void StopAllSounds();

    // Getters
    FMOD::System* GetFMODSystem() { return fmodSystem; }
    bool IsInitialized() { return initialized; }
};