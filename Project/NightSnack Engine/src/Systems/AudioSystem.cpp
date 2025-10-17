/******************************************************************************
 * File:        AudioSystem.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Audio system with channels to play audio
 *
 * Description:
 * This file defines the AudioSystem class, which manages all game audio using FMOD.
 * Works with ResourceManager to load and cache sounds. Handles playing, pausing, stopping
 * and updating audio. Logs messages to cout for debug purposes.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "AudioSystem.h"
#include "ResourceManager/ResourceManager.h"
#include <vector>
#include <algorithm>

namespace NightSnack {

    // Static member initialization
    AudioSystem* AudioSystem::instance = nullptr;

    // Private constructor
    AudioSystem::AudioSystem()
    {
        fmodSystem = nullptr;
        resourceManager = nullptr;
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

    // Singleton getter
    AudioSystem* AudioSystem::GetInstance()
    {
        if (instance == nullptr)
        {
            instance = new AudioSystem();
        }
        return instance;
    }

    // Singleton destroyer
    void AudioSystem::DestroyInstance()
    {
        if (instance != nullptr)
        {
            delete instance;
            instance = nullptr;
        }
    }

    // Initialize FMOD
    bool AudioSystem::Initialize()
    {
        FMOD_RESULT result = FMOD::System_Create(&fmodSystem);
        if (result != FMOD_OK)
        {
            std::cout << "AudioSystem: Failed to create FMOD system - "
                << FMOD_ErrorString(result) << std::endl;
            return false;
        }

        result = fmodSystem->init(32, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
        {
            std::cout << "AudioSystem: Failed to initialize FMOD system - "
                << FMOD_ErrorString(result) << std::endl;
            return false;
        }

        initialized = true;
        std::cout << "AudioSystem: Initialized successfully with 32 channels" << std::endl;
        return true;
    }

    // Update FMOD + cleanup finished channels
    void AudioSystem::Update()
    {
        if (!fmodSystem || !initialized)
            return;

        fmodSystem->update();

        // Clean up finished SFX
        std::vector<std::size_t> sfxToRemove;
        for (auto& [key, channel] : sfxChannels)
        {
            if (channel)
            {
                bool isPlaying = false;
                channel->isPlaying(&isPlaying);
                if (!isPlaying)
                    sfxToRemove.push_back(key);
            }
        }
        for (auto key : sfxToRemove)
            sfxChannels.erase(key);

        // Clean up finished Music
        std::vector<std::size_t> musicToRemove;
        for (auto& [key, channel] : musicChannels)
        {
            if (channel)
            {
                bool isPlaying = false;
                channel->isPlaying(&isPlaying);
                if (!isPlaying)
                    musicToRemove.push_back(key);
            }
        }
        for (auto key : musicToRemove)
            musicChannels.erase(key);
    }

    // Set resource manager
    void AudioSystem::SetResourceManager(ResourceManager* rm)
    {
        resourceManager = rm;
        if (rm)
            std::cout << "AudioSystem: ResourceManager set successfully" << std::endl;
        else
            std::cout << "AudioSystem: ResourceManager set to nullptr" << std::endl;
    }

    // Loading
    std::size_t AudioSystem::LoadSFX(const std::string& path)
    {
        if (!resourceManager || !fmodSystem)
            return 0;

        std::size_t key = 0;
        if (resourceManager->LoadSound(path, key, fmodSystem) == LOAD_SUCCESS)
            return key;

        return 0;
    }

    std::size_t AudioSystem::LoadMusic(const std::string& path, bool loop)
    {
        if (!resourceManager || !fmodSystem)
            return 0;

        std::size_t key = 0;
        if (resourceManager->LoadSound(path, key, fmodSystem) == LOAD_SUCCESS)
        {
            FMOD::Sound* music = resourceManager->GetSound(key);
            if (music && loop)
            {
                music->setMode(FMOD_LOOP_NORMAL);
                music->setLoopCount(-1); // infinite
            }
            return key;
        }
        return 0;
    }

    // Playback
    bool AudioSystem::PlaySound(std::size_t soundKey)
    {
        if (!initialized || !resourceManager || !fmodSystem)
        {
            std::cout << "AudioSystem: system not properly initialized" << std::endl;
            return false;
        }

        FMOD::Sound* sound = resourceManager->GetSound(soundKey);
        if (!sound)
        {
            std::cout << "AudioSystem: Sound not found" << soundKey << std::endl;
            return false;
        }

        FMOD::Channel* channel = nullptr;
        FMOD_RESULT result = fmodSystem->playSound(sound, nullptr, false, &channel);
        if (result != FMOD_OK)
        {
            std::cout << "AudioSystem: Failed to play sound - "
                << FMOD_ErrorString(result) << std::endl;
            return false;
        }

        // Detect if this is music (looping) or SFX
        FMOD_MODE mode;
        sound->getMode(&mode);
        if (mode & FMOD_LOOP_NORMAL)
        {
            musicChannels[soundKey] = channel;
            std::cout << "AudioSystem: Playing music Key: " << soundKey << std::endl;
        }
        else
        {
            sfxChannels[soundKey] = channel;
            std::cout << "AudioSystem: Playing SFX Key: " << soundKey << std::endl;
        }

        return true;
    }

    bool AudioSystem::StopSound(std::size_t soundKey)
    {
        // Look in SFX
        auto itSfx = sfxChannels.find(soundKey);
        if (itSfx != sfxChannels.end() && itSfx->second)
        {
            itSfx->second->stop();
            sfxChannels.erase(itSfx);
            std::cout << "AudioSystem: Stopped SFX Key: " << soundKey << std::endl;
            return true;
        }

        // Look in Music
        auto itMusic = musicChannels.find(soundKey);
        if (itMusic != musicChannels.end() && itMusic->second)
        {
            itMusic->second->stop();
            musicChannels.erase(itMusic);
            std::cout << "AudioSystem: Stopped Music Key: " << soundKey << std::endl;
            return true;
        }

        return false;
    }

    bool AudioSystem::PauseSound(std::size_t soundKey, bool pause)
    {
        auto itSfx = sfxChannels.find(soundKey);
        if (itSfx != sfxChannels.end() && itSfx->second)
        {
            itSfx->second->setPaused(pause);
            std::cout << "AudioSystem: " << (pause ? "Paused" : "Resumed")
                << " SFX (Key: " << soundKey << ")" << std::endl;
            return true;
        }

        auto itMusic = musicChannels.find(soundKey);
        if (itMusic != musicChannels.end() && itMusic->second)
        {
            itMusic->second->setPaused(pause);
            std::cout << "AudioSystem: " << (pause ? "Paused" : "Resumed")
                << " Music (Key: " << soundKey << ")" << std::endl;
            return true;
        }

        return false;
    }

    void AudioSystem::StopAllSounds()
    {
        std::cout << "AudioSystem: Stopping all sounds "
            << sfxChannels.size() + musicChannels.size() << " active" << std::endl;

        for (auto& [key, channel] : sfxChannels)
            if (channel) channel->stop();
        for (auto& [key, channel] : musicChannels)
            if (channel) channel->stop();

        sfxChannels.clear();
        musicChannels.clear();
        std::cout << "AudioSystem: All sounds stopped" << std::endl;
    }

}   // NightSnack
