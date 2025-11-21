/******************************************************************************
 * File:        AudioSystem.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Audio system with channels to play audio
 *
 * Description:
 * This file defines the AudioSystem class, which manages all game audio using FMOD.
 * Works with ResourceManager to load and cache sounds. Handles playing, pausing, stopping
 * and updating audio. Automatically scans audio directories and loads all files.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "AudioSystem.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace NightSnack {

    namespace fs = std::filesystem;

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

        // load all game audio from directories
        LoadAllGameAudio();

        // start playing main menu music by default
        PlayBGM(AudioKeys::mainMenu);
    }

    // destructor cleans up resources and releases fmod system
    AudioSystem::~AudioSystem()
    {
        if (!m_fmodSystem.expired())
        {
            // note: don't release sounds here - resource manager handles that
            m_loadedSFX.clear();
            m_loadedBGM.clear();
            m_keyToID.clear();

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
            return true;
        }
        else
        {
            return false;
        }
    }

    // updates the fmod system
    void AudioSystem::Update()
    {
        if (!m_fmodSystem.expired())
            m_fmodSystem.lock()->update();
    }

    // loads all game audio files by scanning directories
    void AudioSystem::LoadAllGameAudio()
    {
        //std::cout << "[AudioSystem] Scanning audio directories...\n";

        // Scan SFX directory
        ScanAudioDirectory("../assets/Sound/SFX", false);

        // Scan BGM directory
        ScanAudioDirectory("../assets/Sound/Music", true);

        //std::cout << "[AudioSystem] Audio loading complete.\n";
    }

    // scans a directory and loads all audio files
    void AudioSystem::ScanAudioDirectory(const std::string& folderPath, bool isBGM)
    {
        try {
            if (!fs::exists(folderPath)) {
                std::cerr << "[AudioSystem] Directory not found: " << folderPath << "\n";
                return;
            }

            int loadedCount = 0;
            for (const auto& entry : fs::directory_iterator(folderPath)) {
                if (entry.is_regular_file() && IsValidAudioFile(entry.path().string())) {

                    if (LoadSoundFile(entry.path().string(), isBGM)) {
                        loadedCount++;
                    }
                }
            }

            std::string audioType = isBGM ? "BGM" : "SFX";
            //std::cout << "[AudioSystem] Loaded " << loadedCount << " " << audioType
            //    << " files from: " << folderPath << "\n";
        }
        catch (const std::exception& e) {
            std::cerr << "[AudioSystem] Error scanning directory " << folderPath
                << ": " << e.what() << "\n";
        }
    }

    // checks if a file has a valid audio extension
    bool AudioSystem::IsValidAudioFile(const std::string& filepath) const
    {
        static const std::string validExtensions[] = { ".wav", ".mp3", };

        std::string ext = fs::path(filepath).extension().string();

        // convert to lowercase for comparison
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        for (const auto& validExt : validExtensions) 
        {
            if (ext == validExt) 
            {
                return true;
            }
        }
        return false;
    }

    // extracts filename without extension to use as key
    std::string AudioSystem::ExtractKeyFromPath(const std::string& filepath) const
    {
        fs::path p(filepath);
        std::string filename = p.stem().string();
        return filename;
    }

    // loads a single sound file and assigns it a string key based on filename
    bool AudioSystem::LoadSoundFile(const std::string& filepath, bool isBGM)
    {
        if (m_resourceMgr.expired() || m_fmodSystem.expired())
        {
            std::cerr << "[AudioSystem] Cannot load sound - system not ready.\n";
            return false;
        }

        std::string key = ExtractKeyFromPath(filepath);

        size_t soundID = 0;
        LOAD_RESULT result = m_resourceMgr.lock()->LoadSound(filepath, soundID, m_fmodSystem.lock().get());

        if (result == LOAD_SUCCESS || result == LOAD_SUCCESS_STOPPED_UNLOAD)
        {
            m_keyToID[key] = soundID;
            FMOD::Sound* sound = m_resourceMgr.lock()->GetSound(soundID);

            if (isBGM) {
                m_loadedBGM[key] = sound;
                if (sound) {
                    sound->setMode(FMOD_LOOP_NORMAL);
                }
            }
            else {
                m_loadedSFX[key] = sound;
            }

            //std::cout << "[AudioSystem] Loaded: " << filepath << " (key: " << key << ")\n";
            return true;
        }
        else if (result == LOAD_FAIL_PREEXIST)
        {
            m_keyToID[key] = soundID;
            FMOD::Sound* sound = m_resourceMgr.lock()->GetSound(soundID);

            if (isBGM) {
                m_loadedBGM[key] = sound;
                if (sound) {
                    sound->setMode(FMOD_LOOP_NORMAL);
                }
            }
            else {
                m_loadedSFX[key] = sound;
            }

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
    void AudioSystem::PlaySFX(const std::string& soundKey)
    {
        if (!m_initialized || m_resourceMgr.expired() || m_fmodSystem.expired())
            return;

        auto it = m_loadedSFX.find(soundKey);
        if (it == m_loadedSFX.end()) {
            std::cerr << "[AudioSystem] SFX not found: " << soundKey << "\n";
            return;
        }

        FMOD::Sound* sound = it->second;
        FMOD::Channel* channel = nullptr;
        m_fmodSystem.lock()->playSound(sound, nullptr, false, &channel);

        if (channel)
        {
            channel->setVolume(m_sfxVolume * m_masterVolume);
        }
    }

    // plays a sound effect with a specified volume
    void AudioSystem::PlaySFXWithVolume(const std::string& soundKey, float volume)
    {
        if (!m_initialized || m_resourceMgr.expired() || m_fmodSystem.expired())
            return;

        auto it = m_loadedSFX.find(soundKey);
        if (it == m_loadedSFX.end()) {
            std::cerr << "[AudioSystem] SFX not found: " << soundKey << "\n";
            return;
        }

        FMOD::Sound* sound = it->second;
        FMOD::Channel* channel = nullptr;
        m_fmodSystem.lock()->playSound(sound, nullptr, false, &channel);

        if (channel)
        {
            channel->setVolume(volume * m_masterVolume);
        }
    }

    // plays background music by string key
    void AudioSystem::PlayBGM(const std::string& bgmKey)
    {
        if (!m_initialized || m_resourceMgr.expired() || m_fmodSystem.expired())
            return;

        auto it = m_loadedBGM.find(bgmKey);
        if (it == m_loadedBGM.end()) {
            std::cerr << "[AudioSystem] BGM not found: " << bgmKey << "\n";
            return;
        }

        FMOD::Sound* bgm = it->second;

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
        //std::cout << "[AudioSystem] Playing BGM: " << bgmKey << "\n";
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