 /******************************************************************************
 * File:        AudioSystem.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Audio system with channels to play audio
 *
 * Description:
 * This file defines the AudioSystem class, which manages all game audio using FMOD.
 * Works with ResourceManager to load and cache sounds. Handles playing, pausing, stopping
 * and updating audio. All audio files are loaded centrally in this file.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "AudioSystem.h"
#include <iostream>
#include <algorithm>

namespace NightSnack {

    // constructor initializes the audio system and loads all game audio
    AudioSystem::AudioSystem(Coordinator& coordinator, EventDispatcher& dispatcher)
        : m_coord(coordinator),
        m_resourceMgr(coordinator.GetSystem<ResourceManager>()),
        m_dispatcher(&dispatcher),
        m_fmodSystem(coordinator.GetSystem<ResourceManager>()->FmodSystem()),
        m_bgmChannel(nullptr),
        m_initialized(false),
        m_bgmPaused(false),
        m_masterVolume(1.0f),
        m_bgmVolume(0.7f),
        m_sfxVolume(1.0f)
    {
        // initialize fmod first
        if (!Init()) {
            std::cerr << "[AudioSystem] Failed to initialize FMOD!\n";
            return;
        }

        // load all game audio
        LoadAllGameAudio();

        // start playing main menu music by default
        PlayBGM(m_audioKeys.mainMenu);
    }

    // destructor cleans up resources and releases fmod system
    AudioSystem::~AudioSystem()
    {
        if (!m_fmodSystem.expired())
        {
            // note: don't release sounds here - resource manager handles that
            m_loadedSFX.clear();
            m_loadedBGM.clear();

            m_fmodSystem.lock()->close();
            m_fmodSystem.lock()->release();
            m_fmodSystem.reset();
        }
    }

    // initializes the fmod system
    bool AudioSystem::Init()
    {
        m_fmodSystem = m_resourceMgr.lock()->FmodSystem();
        if (!m_fmodSystem.expired())
        {
            m_initialized = true;
            //std::cout << "[AudioSystem] Initialized successfully.\n";
            return true;
        }
        else // it is assigned to nullptr if it wasn't loaded properly
        {
            return false;
        }
    }

    // updates the fmod system
    void AudioSystem::Update(int steps)
    {
        (void)steps;

        if (!m_fmodSystem.expired())
            m_fmodSystem.lock()->update();
    }

    // loads all game audio files
    void AudioSystem::LoadAllGameAudio()
    {
        // load sfx
        LoadSoundFile("assets/Sound/SFX/sound_button_temp.wav", m_audioKeys.buttonClick);

        // add more sfx here as needed

        // load bgm
        LoadSoundFile("assets/Sound/Music/MansionTheme.wav", m_audioKeys.mainMenu);

        // add more bgm here as needed
    }

    // loads a single sound file and assigns it a key
    bool AudioSystem::LoadSoundFile(const std::string& filepath, size_t& outKey)
    {
        if (m_resourceMgr.expired() || m_fmodSystem.expired()) 
        {
            //std::cerr << "[AudioSystem] Cannot load sound - system not ready.\n";
            return false;
        }

        LOAD_RESULT result = m_resourceMgr.lock()->LoadSound(filepath, outKey, m_fmodSystem.lock().get());

        if (result == LOAD_SUCCESS || result == LOAD_SUCCESS_STOPPED_UNLOAD) 
        {
            //std::cout << "[AudioSystem] Loaded: " << filepath << " (key: " << outKey << ")\n";
            return true;
        }
        else if (result == LOAD_FAIL_PREEXIST) 
        {
            //std::cout << "[AudioSystem] Already loaded: " << filepath << "\n";
            return true;
        }
        else 
        {
            std::cerr << "[AudioSystem] Failed to load: " << filepath << "\n";
            return false;
        }
    }

    // plays a sound effect using the default sfx volume
    void AudioSystem::PlaySFX(const size_t soundID)
    {
        if (!m_initialized || m_resourceMgr.expired() || m_fmodSystem.expired())
            return;

        FMOD::Sound* sound = nullptr;

        auto it = m_loadedSFX.find(soundID);
        if (it == m_loadedSFX.end())
        {
            sound = m_resourceMgr.lock()->GetSound(soundID);
            if (!sound)
            {
                std::cerr << "[AudioSystem] SFX not found: " << soundID << "\n";
                return;
            }
            m_loadedSFX[soundID] = sound;
        }
        else
        {
            sound = it->second;
        }

        FMOD::Channel* channel = nullptr;
        m_fmodSystem.lock()->playSound(sound, nullptr, false, &channel);

        if (channel) 
        {
            channel->setVolume(m_sfxVolume * m_masterVolume);
        }
    }

    // plays a sound effect with a specified volume
    void AudioSystem::PlaySFXWithVolume(const size_t soundID, float volume)
    {
        if (!m_initialized || m_resourceMgr.expired() || m_fmodSystem.expired())
            return;

        FMOD::Sound* sound = nullptr;

        auto it = m_loadedSFX.find(soundID);
        if (it == m_loadedSFX.end())
        {
            sound = m_resourceMgr.lock()->GetSound(soundID);
            if (!sound)
            {
                std::cerr << "[AudioSystem] SFX not found: " << soundID << "\n";
                return;
            }
            m_loadedSFX[soundID] = sound;
        }
        else
        {
            sound = it->second;
        }

        FMOD::Channel* channel = nullptr;
        m_fmodSystem.lock()->playSound(sound, nullptr, false, &channel);

        if (channel) 
        {
            channel->setVolume(volume * m_masterVolume);
        }
    }

    // plays background music by ID
    void AudioSystem::PlayBGM(const size_t bgmID)
    {
        if (!m_initialized || m_resourceMgr.expired() || m_fmodSystem.expired())
            return;

        FMOD::Sound* bgm = nullptr;

        auto it = m_loadedBGM.find(bgmID);
        if (it == m_loadedBGM.end())
        {
            bgm = m_resourceMgr.lock()->GetSound(bgmID);
            if (!bgm)
            {
                std::cerr << "[AudioSystem] BGM not found: " << bgmID << "\n";
                return;
            }
            bgm->setMode(FMOD_LOOP_NORMAL);
            m_loadedBGM[bgmID] = bgm;
        }
        else
        {
            bgm = it->second;
        }

        if (m_bgmChannel)
        {
            bool isPlaying = false;
            m_bgmChannel->isPlaying(&isPlaying);
            if (isPlaying)
                m_bgmChannel->stop();
        }

        m_fmodSystem.lock()->playSound(bgm, nullptr, false, &m_bgmChannel);

        if (m_bgmChannel) 
        {
            m_bgmChannel->setVolume(m_bgmVolume * m_masterVolume);
        }

        m_bgmPaused = false;
        //std::cout << "[AudioSystem] Playing BGM: " << bgmID << "\n";
    }

    // stops the currently playing background music
    void AudioSystem::StopBGM()
    {
        if (m_bgmChannel)
        {
            bool isPlaying = false;
            m_bgmChannel->isPlaying(&isPlaying);
            if (isPlaying)
            {
                m_bgmChannel->stop();
            }
        }
        m_bgmPaused = false;
    }

    // pauses the currently playing background music
    void AudioSystem::PauseBGM()
    {
        if (m_bgmChannel && !m_bgmPaused)
        {
            bool isPlaying = false;
            m_bgmChannel->isPlaying(&isPlaying);
            if (isPlaying)
            {
                m_bgmChannel->setPaused(true);
                m_bgmPaused = true;
            }
        }
    }

    // resumes the paused background music
    void AudioSystem::ResumeBGM()
    {
        if (m_bgmChannel && m_bgmPaused)
        {
            m_bgmChannel->setPaused(false);
            m_bgmPaused = false;
        }
    }

    // checks if background music is currently playing
    bool AudioSystem::IsBGMPlaying() const
    {
        if (m_bgmChannel)
        {
            bool isPlaying = false;
            m_bgmChannel->isPlaying(&isPlaying);
            return isPlaying && !m_bgmPaused;
        }
        return false;
    }

    // sets the master volume for all audio
    void AudioSystem::SetMasterVolume(float volume)
    {
        m_masterVolume = std::clamp(volume, 0.0f, 1.0f);

        if (m_bgmChannel)
        {
            m_bgmChannel->setVolume(m_bgmVolume * m_masterVolume);
        }
    }

    // sets the volume for background music
    void AudioSystem::SetBGMVolume(float volume)
    {
        m_bgmVolume = std::clamp(volume, 0.0f, 1.0f);

        if (m_bgmChannel)
        {
            m_bgmChannel->setVolume(m_bgmVolume * m_masterVolume);
        }
    }

    // sets the volume for sound effects
    void AudioSystem::SetSFXVolume(float volume)
    {
        m_sfxVolume = std::clamp(volume, 0.0f, 1.0f);
    }

    // gets the master volume
    float AudioSystem::GetMasterVolume() const
    {
        return m_masterVolume;
    }

    // gets the background music volume
    float AudioSystem::GetBGMVolume() const
    {
        return m_bgmVolume;
    }

    // gets the sound effects volume
    float AudioSystem::GetSFXVolume() const
    {
        return m_sfxVolume;
    }

    Signature AudioSystem::GetSignature() const
    {
        Signature sig;
        return sig;
    }

    std::string AudioSystem::GetName() const
    {
        return "AudioSystem";
    }

} // namespace NightSnack
