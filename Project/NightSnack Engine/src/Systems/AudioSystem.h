/******************************************************************************
 * File:        AudioSystem.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Audio system with channels to play audio
 *
 * Description:
 * This file declares the AudioSystem class, which manages all game audio using FMOD.
 * Works with ResourceManager to load and cache sounds. Handles playing, pausing, stopping
 * and updating audio. Logs messages to cout for debug purposes.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include "Core/ECS/ISystem.h"
#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "ResourceManager/ResourceManager.h"
#include <fmod.hpp>
#include <unordered_map>
#include <string>
#include <iostream>

namespace NightSnack {

    // Audio keys for easy access throughout the game
    struct AudioKeys {
        // SFX
        size_t buttonClick = 0;

        // BGM
        size_t mainMenu = 0;

    };

    enum AUDIO_RESULT {
        AUDIO_SUCCESS,
        AUDIO_FAIL_INIT,
        AUDIO_FAIL_SOUND_NOT_FOUND,
        AUDIO_FAIL_ERROR
    };

    class AudioSystem : public ISystem {
    public:
        AudioSystem(Coordinator& coordinator, EventDispatcher& dispatcher);
        ~AudioSystem();

        // Initialization - loads all game audio
        bool Init();

        // System update
        void Update(float dt, Coordinator& coord) override;

        // Get audio keys for playing sounds
        const AudioKeys& GetAudioKeys() const { return m_audioKeys; }

        // Loading (called internally by Init, but can be called manually too)
        bool LoadSoundFile(const std::string& filepath, size_t& outKey);
        void LoadAllGameAudio(); // Loads all audio files at once

        // SFX Functions
        void PlaySFX(const size_t soundID);
        void PlaySFXWithVolume(const size_t soundID, float volume);

        // BGM Functions
        void PlayBGM(const size_t bgmID);
        void StopBGM();
        void PauseBGM();
        void ResumeBGM();
        bool IsBGMPlaying() const;

        // Volume Control (0.0 to 1.0)
        void SetMasterVolume(float volume);
        void SetBGMVolume(float volume);
        void SetSFXVolume(float volume);
        float GetMasterVolume() const;
        float GetBGMVolume() const;
        float GetSFXVolume() const;

        // ISystem overrides
        Signature GetSignature() const override;
        std::string GetName() const override;

    private:
        // Resource management
        std::weak_ptr<ResourceManager> m_resourceMgr;
        EventDispatcher* m_dispatcher;
        Coordinator& m_coord;

        // FMOD
        std::weak_ptr<FMOD::System> m_fmodSystem;
        FMOD::Channel* m_bgmChannel;

        // Sound caches
        std::unordered_map<size_t, FMOD::Sound*> m_loadedSFX;
        std::unordered_map<size_t, FMOD::Sound*> m_loadedBGM;

        // Audio keys for all game sounds
        AudioKeys m_audioKeys;

        // State
        bool m_initialized;
        bool m_bgmPaused;

        // Volume levels
        float m_masterVolume;
        float m_bgmVolume;
        float m_sfxVolume;
    };

} // namespace NightSnack

#endif // AUDIOSYSTEM_H