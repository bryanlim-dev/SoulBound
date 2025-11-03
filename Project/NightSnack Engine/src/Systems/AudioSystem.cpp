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
        // Initialize FMOD first
        if (!Init()) {
            std::cerr << "[AudioSystem] Failed to initialize FMOD!\n";
            return;
        }

        // Load all game audio
        LoadAllGameAudio();

        // Start playing main menu music by default
        //PlayBGM(m_audioKeys.mainMenu);
    }

    AudioSystem::~AudioSystem()
    {
        if (!m_fmodSystem.expired())
        {
            // Note: Don't release sounds here - ResourceManager handles that
            m_loadedSFX.clear();
            m_loadedBGM.clear();

            m_fmodSystem.lock()->close();
            m_fmodSystem.lock()->release();
            m_fmodSystem.reset();
        }
    }

    bool AudioSystem::Init()
    {
        m_fmodSystem = m_resourceMgr.lock()->FmodSystem();
        if (!m_fmodSystem.expired())
        {
            m_initialized = true;
            std::cout << "[AudioSystem] Initialized successfully.\n";
            return true;
        }
        else //It assigned to nullptr if it wasnt loaded properly
        {
            return false;
        }
    }

    void AudioSystem::Update(int steps)
    {
        (void)steps;

        if (!m_fmodSystem.expired())
            m_fmodSystem.lock()->update();
    }

    // CENTRALIZED AUDIO LOADING - ALL GAME AUDIO LOADED HERE
    void AudioSystem::LoadAllGameAudio()
    {
        //std::cout << "[AudioSystem] Loading all game audio...\n";

        // LOAD SFX
        LoadSoundFile("assets/Sound/SFX/sound_button_temp.wav", m_audioKeys.buttonClick);

        // Add more SFX here as needed:
        // LoadSoundFile("assets/Sound/SFX/explosion.wav", m_audioKeys.explosion);
        // LoadSoundFile("assets/Sound/SFX/powerup.wav", m_audioKeys.powerup);

        // LOAD BGM
        LoadSoundFile("assets/Sound/Music/MansionTheme.wav", m_audioKeys.mainMenu);

        // Add more BGM here as needed:
        // LoadSoundFile("assets/Sound/BGM/boss_battle.mp3", m_audioKeys.bossBattle);
        // LoadSoundFile("assets/Sound/BGM/credits.mp3", m_audioKeys.credits);

        //std::cout << "[AudioSystem] All game audio loaded.\n";
    }

    bool AudioSystem::LoadSoundFile(const std::string& filepath, size_t& outKey)
    {
        if (m_resourceMgr.expired() || m_fmodSystem.expired()) {
            std::cerr << "[AudioSystem] Cannot load sound - system not ready.\n";
            return false;
        }

        LOAD_RESULT result = m_resourceMgr.lock()->LoadSound(filepath, outKey, m_fmodSystem.lock().get());

        if (result == LOAD_SUCCESS || result == LOAD_SUCCESS_STOPPED_UNLOAD) {
            std::cout << "[AudioSystem] Loaded: " << filepath << " (key: " << outKey << ")\n";
            return true;
        }
        else if (result == LOAD_FAIL_PREEXIST) {
            std::cout << "[AudioSystem] Already loaded: " << filepath << "\n";
            return true;
        }
        else {
            std::cerr << "[AudioSystem] Failed to load: " << filepath << "\n";
            return false;
        }
    }

    // SFX FUNCTIONS
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

        if (channel) {
            channel->setVolume(m_sfxVolume * m_masterVolume);
        }
    }

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

        if (channel) {
            channel->setVolume(volume * m_masterVolume);
        }
    }

    // ============================================================================
    // BGM FUNCTIONS
    // ============================================================================

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

        if (m_bgmChannel) {
            m_bgmChannel->setVolume(m_bgmVolume * m_masterVolume);
        }

        m_bgmPaused = false;
        //std::cout << "[AudioSystem] Playing BGM: " << bgmID << "\n";
    }

    void AudioSystem::StopBGM()
    {
        if (m_bgmChannel)
        {
            bool isPlaying = false;
            m_bgmChannel->isPlaying(&isPlaying);
            if (isPlaying)
            {
                m_bgmChannel->stop();
                //std::cout << "[AudioSystem] BGM stopped.\n";
            }
        }
        m_bgmPaused = false;
    }

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
                //std::cout << "[AudioSystem] BGM paused.\n";
            }
        }
    }

    void AudioSystem::ResumeBGM()
    {
        if (m_bgmChannel && m_bgmPaused)
        {
            m_bgmChannel->setPaused(false);
            m_bgmPaused = false;
            //std::cout << "[AudioSystem] BGM resumed.\n";
        }
    }

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

    // ============================================================================
    // VOLUME CONTROL
    // ============================================================================

    void AudioSystem::SetMasterVolume(float volume)
    {
        m_masterVolume = std::clamp(volume, 0.0f, 1.0f);

        if (m_bgmChannel)
        {
            m_bgmChannel->setVolume(m_bgmVolume * m_masterVolume);
        }
    }

    void AudioSystem::SetBGMVolume(float volume)
    {
        m_bgmVolume = std::clamp(volume, 0.0f, 1.0f);

        if (m_bgmChannel)
        {
            m_bgmChannel->setVolume(m_bgmVolume * m_masterVolume);
        }
    }

    void AudioSystem::SetSFXVolume(float volume)
    {
        m_sfxVolume = std::clamp(volume, 0.0f, 1.0f);
    }

    float AudioSystem::GetMasterVolume() const
    {
        return m_masterVolume;
    }

    float AudioSystem::GetBGMVolume() const
    {
        return m_bgmVolume;
    }

    float AudioSystem::GetSFXVolume() const
    {
        return m_sfxVolume;
    }

    // ============================================================================
    // SYSTEM REQUIRED
    // ============================================================================

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