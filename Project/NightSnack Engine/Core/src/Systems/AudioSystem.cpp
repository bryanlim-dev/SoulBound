/******************************************************************************
 * File:        AudioSystem.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Audio system with channels to play audio
 *
 * Description:
 * This file defines the AudioSystem class, which manages all game audio using FMOD.
 * Works with ResourceManager to load and cache sounds. Handles playing, pausing, stopping
 * and updating audio. Automatically scans audio directories and loads all files.
 * Handles the audio component playing and stopping for entity-specific sounds.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "AudioSystem.h"
#include "Systems/PlayerController.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <random>

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
        m_masterVolume(0.5f),
        m_bgmVolume(0.2f),
        m_sfxVolume(0.4f),
        m_isFading(false),     
        m_fadeTimer(0.0f),      
        m_fadeDuration(0.0f),   
        m_fadeTargetVolume(0.0f)
    {
        m_systemSignature.set(GetComponentTypeID<Audio>());

        // initialize fmod first
        if (!Init()) {
            std::cerr << "[AudioSystem] Failed to initialize FMOD!\n";
            return;
        }

        // load all game audio from directories
        LoadAllGameAudio();
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
        //static bool first = true;
        //if (first) {
        //    // start playing main menu music
        //    PlayBGM("mainMenu");
        //    first = false;
        //}

        if (m_coord.IsPaused()) {
            if (IsBGMPlaying()) {
                PauseBGM();
            }
  
            // Pause all entity audio channels
            for (auto& [entity, channel] : m_entityChannels) {
                if (channel) {
                    bool isPaused;
                    channel->getPaused(&isPaused);
                    if (!isPaused) {
                        channel->setPaused(true);
                    }
                }
            }
            return;
        } else if (!IsBGMPlaying()) {
            ResumeBGM();
        }

		// resume all entity audio channels
        for (auto& [entity, channel] : m_entityChannels) {
            if (channel) {
                bool isPaused;
                channel->getPaused(&isPaused);
                if (isPaused) {
                    channel->setPaused(false);
                }
            }
        }

        ControlFade(1.0f / 60.0f);

        if (!m_fmodSystem.expired())
            m_fmodSystem.lock()->update();

        // Clean up finished channels
        for (auto it = m_entityChannels.begin(); it != m_entityChannels.end(); ) {
            bool isPlaying = false;
            if (it->second) 
            {
                it->second->isPlaying(&isPlaying);
            }

            if (!isPlaying) 
            {
                // Update the audio component state
                if (m_coord.HasComponent<Audio>(it->first)) {
                    auto& audio = m_coord.ModifyComponent<Audio>(it->first);
                    audio.isPlaying = false;
                }
                it = m_entityChannels.erase(it);
            }
            else {
                ++it;
            }
        }

        for (Entity e : m_entities) {
            Audio& a = m_coord.ModifyComponent<Audio>(e);
            
            /*std::cout << "[AudioSystem] Entity " << e
                << " key: " << a.audioKey
                << " isPlaying: " << a.isPlaying
                << " playOnStart: " << a.playOnStart << std::endl;*/

            if (m_coord.HasComponent<Collider>(e) && m_coord.HasComponent<Transform>(e)) {
                auto& col = m_coord.GetComponent<Collider>(e);
                auto& t = m_coord.GetComponent<Transform>(e);

                if (t.name.find("Sprite-PlatformTile1") != std::string::npos) {
                    if (col.type == ColliderType::Trigger &&
                        m_triggeredFallAudio.find(e) == m_triggeredFallAudio.end())
                    {
                        std::cout << "[AudioSystem] Playing fall sound for: " << t.name << std::endl;
                        PlayRandomVariant("Rock_Hit", 4, 1.0f);
                        m_triggeredFallAudio.insert(e);
                    }

                    if (col.type == ColliderType::Solid &&
                        m_triggeredFallAudio.count(e))
                    {
                        m_triggeredFallAudio.erase(e);
                    }
                }
            }

            if (a.playOnStart)
            {
                PlayAudio(a, e);
                a.playOnStart = false; // only auto-play once
                //std::cout << "[AudioSystem] Playing audio: " << a.audioKey << "\n";
            }
            else if (a.isPlaying) 
            {
                PlayAudio(a, e);
                a.isPlaying = false;
            }

            if (!a.loop) 
			{
				// ensure channel is set to no loop
				m_entityChannels[e]->setMode(FMOD_LOOP_OFF);
				m_entityChannels[e]->setLoopCount(0);
            }
        }

        Entity player = m_coord.GetSystem<PlayerControllerSystem>()->GetPlayer();

        if (player != INVALID_ENTITY && m_coord.HasComponent<Transform>(player)) {
            Vec2 playerPos = {
                m_coord.GetComponent<Transform>(player).pos.x,
                m_coord.GetComponent<Transform>(player).pos.y
            };
            UpdateSpatialAudio(playerPos, 500.0f);
        }
    }

    // loads all game audio files by scanning directories
    void AudioSystem::LoadAllGameAudio()
    {
        //std::cout << "[AudioSystem] Scanning audio directories...\n";

        // Scan SFX directory
        ScanAudioDirectory("Sound/SFX", false);

        // Scan BGM directory
        ScanAudioDirectory("Sound/Music", true);

        //std::cout << "[AudioSystem] Audio loading complete.\n";
    }

    // scans a directory and loads all audio files
    void AudioSystem::ScanAudioDirectory(const std::string& folderPath, bool isBGM)
    {
        //std::cout << "[Audio] Current path: " << fs::current_path() << '\n';
        fs::path fp = std::filesystem::canonical(RESOURCE_PATH + folderPath);

        try {
            if (!fs::exists(fp)) {
                std::cerr << "[AudioSystem] Directory not found: " << fp << "\n";
                return;
            }

            int loadedCount = 0;
            for (const auto& entry : fs::directory_iterator(fp)) {
                if (entry.is_regular_file() && IsValidAudioFile(entry.path().filename().string())) {

                    std::string file = folderPath + "/" + entry.path().filename().string();
                    //std::cout << "[Audio] Attempting to load audio: " << file << "\n\n";
                    if (LoadSoundFile(file, isBGM)) {
                        loadedCount++;
                    }
                }
            }

            std::string audioType = isBGM ? "BGM" : "SFX";
            /*std::cout << "[AudioSystem] Loaded " << loadedCount << " " << audioType
                << " files from: " << folderPath << "\n";*/
        }
        catch (const std::exception& e) {
            std::cerr << "[AudioSystem] Error scanning directory " << folderPath
                << ": " << e.what() << "\n\n";
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
        //std::cout << "[Audio] Calling RM with filepath: " << filepath << "\n\n";
        LOAD_RESULT result = m_resourceMgr.lock()->LoadSound(filepath, soundID, m_fmodSystem.lock().get());

        if (result == LOAD_SUCCESS || result == LOAD_SUCCESS_STOPPED_UNLOAD)
        {
			// map key to ID
            m_keyToID[key] = soundID;
			// get sound pointer from resource manager
            FMOD::Sound* sound = m_resourceMgr.lock()->GetSound(soundID);

            if (isBGM) {
				// store bg pointer and set to loop
                m_loadedBGM[key] = sound;
                if (sound) {
                    sound->setMode(FMOD_LOOP_NORMAL);
                }
            }
            else {
				// store sfx pointer
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

    std::vector<std::string> AudioSystem::GetBGM() const 
    {
        std::vector<std::string> playing;
        for (const auto& [key, channel] : m_bgmChannels)
        {
            if (!channel) continue;
            bool isPlaying = false;
            channel->isPlaying(&isPlaying);

            if (isPlaying && !m_bgmPaused)
                playing.push_back(key);
        }
        return playing;
    }

    // plays background music by string key
    void AudioSystem::PlayBGM(const std::string& bgmKey)
    {
        if (!m_initialized || m_resourceMgr.expired() || m_fmodSystem.expired())
            return;

        auto it = m_loadedBGM.find(bgmKey);
        if (it == m_loadedBGM.end())
        {
            std::cerr << "[AudioSystem] BGM not found: " << bgmKey << "\n";
            return;
        }

        // Check if this BGM is already playing
        auto channelIt = m_bgmChannels.find(bgmKey);
        if (channelIt != m_bgmChannels.end())
        {
            bool isPlaying = false;
            channelIt->second->isPlaying(&isPlaying);
            if (isPlaying)
            {
                //std::cout << "[AudioSystem] Already playing " << bgmKey << ", continuing...\n";
                return;
            }
        }

        // Play the new BGM
        FMOD::Sound* bgm = it->second;
        FMOD::Channel* newChannel = nullptr;

        m_fmodSystem.lock()->playSound(bgm, nullptr, false, &newChannel);

        if (newChannel)
        {
            newChannel->setVolume(m_bgmVolume * m_masterVolume);
            m_bgmChannels[bgmKey] = newChannel;  // Store in map
        }

        m_bgmPaused = false;
        m_currentKeyBGM = bgmKey;  // Track most recent for backwards compatibility
        //std::cout << "[AudioSystem] Playing BGM: " << bgmKey << "\n";
    }

    // Stop specific BGM
    void AudioSystem::StopBGM(const std::string& bgmKey)
    {
        auto it = m_bgmChannels.find(bgmKey);
        if (it != m_bgmChannels.end())
        {
            if (it->second)
                it->second->stop();
            m_bgmChannels.erase(it);

            if (m_currentKeyBGM == bgmKey)
                m_currentKeyBGM = "";
        }
    }

    // Stop all BGM
    void AudioSystem::StopBGM()
    {
        for (auto& [key, channel] : m_bgmChannels)
        {
            if (channel)
                channel->stop();
        }
        m_bgmChannels.clear();
        m_currentKeyBGM = "";
        m_bgmPaused = false;
    }

    // pauses the currently playing background music
    void AudioSystem::PauseBGM()
    {
        for (auto& [key, channel] : m_bgmChannels)
        {
            if (channel && !m_bgmPaused)
            {
                bool isPlaying = false;
                channel->isPlaying(&isPlaying);
                if (isPlaying)
                {
                    channel->setPaused(true);
                }
            }
        }
        m_bgmPaused = true;
    }

    void AudioSystem::ResumeBGM()
    {
        for (auto& [key, channel] : m_bgmChannels)
        {
            if (channel && m_bgmPaused)
            {
                channel->setPaused(false);
            }
        }
        m_bgmPaused = false;
    }

    // checks if background music is currently playing
    bool AudioSystem::IsBGMPlaying() const
    {
        for (const auto& [key, channel] : m_bgmChannels)
        {
            if (channel)
            {
                bool isPlaying = false;
                channel->isPlaying(&isPlaying);
                if (isPlaying && !m_bgmPaused)
                    return true;
            }
        }
        return false;
    }

    // sets the master volume for all audio
    void AudioSystem::SetMasterVolume(float volume)
    {
        m_masterVolume = std::clamp(volume, 0.0f, 1.0f);

        // Update all BGM channels
        for (auto& [key, channel] : m_bgmChannels)
        {
            if (channel)
                channel->setVolume(m_bgmVolume * m_masterVolume);
        }
    }

    // sets the bgm volume
    void AudioSystem::SetBGMVolume(float volume)
    {
        m_bgmVolume = std::clamp(volume, 0.0f, 1.0f);
        m_userBGMVolume = m_bgmVolume;

        // Update all BGM channels
        for (auto& [key, channel] : m_bgmChannels)
        {
            if (channel)
                channel->setVolume(m_bgmVolume * m_masterVolume);
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
        return m_userBGMVolume;
    }

    // gets the sound effects volume
    float AudioSystem::GetSFXVolume() const
    {
        return m_sfxVolume;
    }

    void AudioSystem::PlayEntityAudio(Entity e) 
    {
        if (!m_coord.HasComponent<Audio>(e)) return;
        Audio const& a = m_coord.GetComponent<Audio>(e);
        PlaySFX(a.audioKey);
    }

    // for audio component
    void AudioSystem::PlayAudio(Audio const& audio, Entity entity)
    {
        FMOD::Channel* channel = nullptr;

        // Determine which sound to play
        std::string keyToPlay = audio.audioKey;

        // If using variants, pick randomly
        if (audio.useVariants && audio.numVariants > 1) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(1, audio.numVariants);
            int variant = dist(gen);
            keyToPlay = audio.audioKey + "_" + std::to_string(variant);
        }

        size_t id = m_keyToID[keyToPlay];
        FMOD::Sound* sound = m_resourceMgr.lock()->GetSound(id);
        if (!sound) return;

        // stop previous channel for this entity if exists
        auto existingIt = m_entityChannels.find(entity);
        if (existingIt != m_entityChannels.end() && existingIt->second) {
            existingIt->second->stop();
            m_entityChannels.erase(existingIt);
        }

        // play the sound to get the channel
        FMOD_RESULT result = m_fmodSystem.lock()->playSound(sound, nullptr, false, &channel);

        if (result != FMOD_OK || !channel) {
            std::cerr << "[AudioSystem] Failed to play sound: " << keyToPlay << std::endl;
            return;
        }

        // set loop properties on the channel
        if (audio.loop) {
            channel->setMode(FMOD_LOOP_NORMAL);
            channel->setLoopCount(-1);
        }
        else {
            channel->setMode(FMOD_LOOP_OFF);
            channel->setLoopCount(0);
        }

        // set volume and store channel
        channel->setVolume(audio.volume * m_masterVolume);
        m_entityChannels[entity] = channel;
    }

    // getter for audio keys to edit in prefab panel
    std::vector<std::string> AudioSystem::GetAvailableAudioKeys() const 
    {
        std::vector<std::string> keys;
        for (const auto& [key, sound] : m_loadedSFX) 
        {
            keys.push_back(key);
        }
        for (const auto& [key, sound] : m_loadedBGM) 
        {
            keys.push_back(key);
        }
        return keys;
    }

    void AudioSystem::FadeToVolume(float targetVolume, float duration)
    {
        m_isFading = true;
        m_fadeTimer = 0.0f;
        m_fadeDuration = duration;
        m_fadeTargetVolume = targetVolume;
    }

    void AudioSystem::ControlFade(float deltaTime)
    {
        if (!m_isFading) return;

        m_fadeTimer += deltaTime;
        float t = std::min(m_fadeTimer / m_fadeDuration, 1.0f);

        // Calculate current volume
        float startVolume = m_bgmVolume - (m_fadeTargetVolume - m_bgmVolume) * (m_fadeTimer - deltaTime) / m_fadeDuration;
        float currentVolume = startVolume + (m_fadeTargetVolume - startVolume) * t;

        // Apply to ALL BGM channels
        for (auto& [key, channel] : m_bgmChannels)
        {
            if (channel)
                channel->setVolume(currentVolume * m_masterVolume);
        }

        if (t >= 1.0f) {
            m_isFading = false;
            m_bgmVolume = m_fadeTargetVolume;
            if (m_fadeTargetVolume > 0.0f) 
            {
                m_userBGMVolume = m_fadeTargetVolume;
            }
        }
    }

    void AudioSystem::PlayRandomVariant(const std::string& baseKey, int numVariants, float volume)
    {
        if (numVariants <= 0) {
            PlaySFXWithVolume(baseKey, volume);
            return;
        }

        // generate random number between 1 and numVariants
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, numVariants);

        int variant = dist(gen);
        std::string soundKey = baseKey + "_" + std::to_string(variant);
        PlaySFXWithVolume(soundKey, volume);
    }

    void AudioSystem::UpdateSpatialAudio(Vec2 const& playerPos, float maxDistance)
    {
        for (Entity e : m_entities) {
            if (!m_coord.HasComponent<Audio>(e)) continue;
            if (!m_coord.HasComponent<Transform>(e)) continue;

            Audio const& a = m_coord.GetComponent<Audio>(e);
            if (!a.spatial) continue; // only process spatial audio entities

            Transform const& t = m_coord.GetComponent<Transform>(e);
            Vec2 entityPos = { t.pos.x, t.pos.y };

            // Calculate distance between entity and player
            float dx = entityPos.x - playerPos.x;
            float dy = entityPos.y - playerPos.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            // If out of range, stop/mute the audio
            auto channelIt = m_entityChannels.find(e);
            if (channelIt == m_entityChannels.end()) continue;

            if (distance >= maxDistance) {
                channelIt->second->setVolume(0.0f);
            }
            else {
                // Linear falloff: 1.0 at distance 0, 0.0 at maxDistance
                float fallOff = 1.0f - (distance / maxDistance);
                // Smooth the falloff with a squared curve
                float spatialVolume = fallOff * fallOff * a.volume * m_masterVolume;
                channelIt->second->setVolume(spatialVolume);
            }
        }
    }

    Signature AudioSystem::GetSignature() const
    {
        return m_systemSignature;
    }

    std::string AudioSystem::GetName() const
    {
        return "AudioSystem";
    }

} // namespace NightSnack