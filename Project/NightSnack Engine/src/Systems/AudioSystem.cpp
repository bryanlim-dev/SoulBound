#include "AudioSystem.h"
#include "ResourceManager/ResourceManager.h"
#include <vector>
#include <algorithm>

// Static member initialization
AudioSystem* AudioSystem::instance = nullptr;

// Private constructor
AudioSystem::AudioSystem()
{
    fmodSystem = nullptr;
    resourceManager = nullptr;
    nextChannelId = 1;
    initialized = false;
    std::cout << "AudioSystem created" << std::endl;
}

// Destructor
AudioSystem::~AudioSystem()
{
    if (fmodSystem)
    {
        StopAllSounds();
        fmodSystem->release();
        fmodSystem = nullptr;
    }
    initialized = false;
    std::cout << "AudioSystem destroyed" << std::endl;
}

// Static singleton getter
AudioSystem* AudioSystem::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new AudioSystem();
    }
    return instance;
}

// Static singleton destroyer
void AudioSystem::DestroyInstance()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

// Initialize FMOD system
bool AudioSystem::Initialize()
{
    // Create FMOD system
    FMOD_RESULT result = FMOD::System_Create(&fmodSystem);
    if (result != FMOD_OK)
    {
        std::cout << "AudioSystem: Failed to create FMOD system - " << FMOD_ErrorString(result) << std::endl;
        return false;
    }

    // Initialize FMOD system with 32 channels
    result = fmodSystem->init(16, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK)
    {
        std::cout << "AudioSystem: Failed to initialize FMOD system - " << FMOD_ErrorString(result) << std::endl;
        return false;
    }

    initialized = true;
    std::cout << "AudioSystem: Initialized successfully with 16 channels" << std::endl;
    return true;
}

// Update FMOD and clean up finished channels
void AudioSystem::Update()
{
    if (!fmodSystem || !initialized)
        return;

    // Update FMOD system
    fmodSystem->update();

    // Clean up finished channels
    std::vector<int> channelsToRemove;
    for (auto& pair : activeChannels)
    {
        if (pair.second)
        {
            bool isPlaying = false;
            pair.second->isPlaying(&isPlaying);
            if (!isPlaying)
            {
                channelsToRemove.push_back(pair.first);
            }
        }
    }

    // Remove finished channels from the map
    for (int channelId : channelsToRemove)
    {
        activeChannels.erase(channelId);
    }
}

// Set the ResourceManager pointer
void AudioSystem::SetResourceManager(ResourceManager* rm)
{
    resourceManager = rm;
    if (rm)
    {
        std::cout << "AudioSystem: ResourceManager set successfully" << std::endl;
    }
    else
    {
        std::cout << "AudioSystem: ResourceManager set to nullptr" << std::endl;
    }
}

// Play a sound using ResourceManager key
bool AudioSystem::PlaySound(std::size_t soundKey)
{
    // Check if system is properly initialized
    if (!initialized || !resourceManager || !fmodSystem)
    {
        std::cout << "AudioSystem: Cannot play sound - system not properly initialized" << std::endl;
        return false;
    }

    // Get sound from ResourceManager using the key
    FMOD::Sound* sound = resourceManager->GetSound(soundKey);
    if (!sound)
    {
        std::cout << "AudioSystem: Sound not found for key " << soundKey << std::endl;
        return false;
    }

    // Play the sound
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result = fmodSystem->playSound(sound, nullptr, false, &channel);
    if (result != FMOD_OK)
    {
        std::cout << "AudioSystem: Failed to play sound - " << FMOD_ErrorString(result) << std::endl;
        return false;
    }

    // Store the channel for tracking
    if (channel)
    {
        activeChannels[nextChannelId] = channel;
        std::cout << "AudioSystem: Playing sound (Key: " << soundKey << ", Channel: " << nextChannelId << ")" << std::endl;
        nextChannelId++;
    }

    return true;
}

// Stop all currently playing sounds
void AudioSystem::StopAllSounds()
{
    std::cout << "AudioSystem: Stopping all sounds (" << activeChannels.size() << " active channels)" << std::endl;

    for (auto& pair : activeChannels)
    {
        if (pair.second)
        {
            pair.second->stop();
        }
    }

    activeChannels.clear();
    std::cout << "AudioSystem: All sounds stopped" << std::endl;
}