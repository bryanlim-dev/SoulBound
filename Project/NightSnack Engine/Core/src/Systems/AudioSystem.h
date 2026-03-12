/******************************************************************************
 * File:        AudioSystem.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Audio system with channels to play audio
 *
 * Description:
 * This file declares the AudioSystem class, which manages all game audio using FMOD.
 * Works with ResourceManager to load and cache sounds. Handles playing, pausing, stopping
 * and updating audio. Automatically scans audio folders and assigns keys to found files.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
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

    /**
     * @enum AUDIO_RESULT
     * @brief result of the audio operation
     */
    enum AUDIO_RESULT {
        AUDIO_SUCCESS,
        AUDIO_FAIL_INIT,
        AUDIO_FAIL_SOUND_NOT_FOUND,
        AUDIO_FAIL_ERROR
    };

    /**
     * @class AudioSystem
     * @brief manage all game audio using FMOD
     * controls audio playback, volume, loading and updating audio system
     * automatically scans folders and assigns keys to audio files
     */
    class AudioSystem : public ISystem {
    public:
        /**
        * @brief audio system constructor
        * @param coordinator reference to the ECS coordinator
        * @param dispatcher reference to the event dispatcher
        */
        AudioSystem(Coordinator& coordinator, EventDispatcher& dispatcher);

        /**
        * @brief destructor
        */
        ~AudioSystem();

        /**
        * @brief initialize the audio system
        * @return true if successful, else false
        */
        bool Init();

        /**
        * @brief update the audio system
        * @param steps number of simulation steps
        */
        void Update() override;

        /**
         * @brief loads all game audio files by scanning audio directories
         */
        void LoadAllGameAudio();

        ///**
        // * @brief retrieve audio keys to access sound
        // * @return reference to AudioKeys struct
        // */
        //const AudioKeys& GetAudioKeys() const { return m_audioKeys; }

        // SFX Functions

        /**
         * @brief plays a sound effect (SFX) by its string key
         * @param soundKey string key of the sound effect to play
         */
        void PlaySFX(const std::string& soundKey);

        /**
         * @brief plays a sound effect (SFX) with a specified volume
         * @param soundKey string key of the sound effect to play
         * @param volume volume level (0.0 to 1.0)
         */
        void PlaySFXWithVolume(const std::string& soundKey, float volume);

        // BGM Functions

        /**
         * @brief plays background music (BGM) by its string key
         * @param bgmKey string key of the background music to play
         */
        void PlayBGM(const std::string& bgmKey);

        /**
         * @brief stops the currently playing background music
         */
        void StopBGM();

        /**
        * @brief stops specific background music by its key
        */
        void StopBGM(const std::string& bgmKey);

        /**
         * @brief pauses the currently playing background music
         */
        void PauseBGM();

        /**
         * @brief resumes the paused background music
         */
        void ResumeBGM();

        /**
         * @brief checks if background music is currently playing
         * @return true if BGM is playing, else false
         */
        bool IsBGMPlaying() const;

        // Volume Control

        /**
         * @brief sets the master volume level
         * @param volume volume level (0.0 to 1.0)
         */
        void SetMasterVolume(float volume);

        /**
         * @brief sets the background music (BGM) volume level
         * @param volume volume level (0.0 to 1.0)
         */
        void SetBGMVolume(float volume);
        /**
         * @brief sets the sound effects (SFX) volume level
         * @param volume volume level (0.0 to 1.0)
         */
        void SetSFXVolume(float volume);

        /**
         * @brief gets the current master volume level
         * @return master volume level (0.0 to 1.0)
         */
        float GetMasterVolume() const;

        /**
         * @brief gets the current background music (BGM) volume level
         * @return BGM volume level (0.0 to 1.0)
         */
        float GetBGMVolume() const;

        /**
         * @brief gets the current sound effects (SFX) volume level
         * @return SFX volume level (0.0 to 1.0)
         */
        float GetSFXVolume() const;

        /**
         * @brief plays audio for a specific entity using audio component properties
		 * @param a reference to the Audio component
		 * @param entity the entity to play audio for
         */
        void PlayAudio(Audio const& audio, Entity entity);

        /**
         * @brief wrapper for PlaySFX(), plays audio for a specific entity
         * @param entity the entity to play audio for
         */
        void PlayEntityAudio(Entity e);

        //void StopAudio(Entity entity);

		/**
		 * @brief retrieves a list of all available audio keys
		 * @return sorted list of available audio keys
		 */
        std::vector<std::string> GetAvailableAudioKeys() const;

        /**
         * @brief retrieves the audio key of current BGM
         * @return std::string key of current BGM
         */
        std::vector<std::string> GetBGM() const;

        /**
         * @brief fades the current BGM to a target volume over a duration
         * @param targetVolume target volume level (0.0 to 1.0)
         * @param duration fade duration in seconds
         */
        void FadeToVolume(float targetVolume, float duration);

        /**
         * @brief updates the fade transition for BGM
         * @param deltaTime time elapsed since last frame in seconds
         */
        void ControlFade(float deltaTime);

        /**
         * @brief plays a random variant of a sound effect
         * @param baseKey base name of the sound (e.g., "playerHurt")
         * @param numVariants number of variants available
		 * @param volume volume level (0.0 to 1.0)
         */
        void PlayRandomVariant(const std::string& baseKey, int numVariants, float volume);

        /**
		 * @brief plays spatial audio based on player position and max distance
		 * @param playerPos position of the player in the game world
		 * @param maxDistance maximum distance at which audio can be heard
         */
        //void UpdateSpatialAudio(Vec2 const& playerPos, float maxDistance);

        // ISystem overrides

        /**
         * @brief retrieve the system signature
         * @return system signature
         */
        Signature GetSignature() const override;

        /**
         * @brief retrieve the system name
         * @return system name
         */
        std::string GetName() const override;

    private:
        // Resource management
        // use weak pointer to avoid circular dependency
        // aka allow safe access to shared resources without extending their lifetime
        std::weak_ptr<ResourceManager> m_resourceMgr;
        EventDispatcher* m_dispatcher;
        Coordinator& m_coord;

		Signature m_systemSignature{};

        // FMOD
        std::weak_ptr<FMOD::System> m_fmodSystem;
        FMOD::Channel* m_bgmChannel;

        std::unordered_map<Entity, FMOD::Channel*> m_entityChannels;

        std::unordered_set<Entity> m_triggeredFallAudio;
        std::unordered_set<Entity> m_triggeredSpikeOutAudio;
        std::unordered_set<Entity> m_triggeredSpikeInAudio;
        std::unordered_map<Entity, float> m_spikePrevPosY;
        std::unordered_set<Entity> m_triggeredSpikeOutAudioX;
        std::unordered_set<Entity> m_triggeredSpikeInAudioX;
        std::unordered_map<Entity, float> m_spikePrevPosX;

        // Sound caches - maps string keys to sound pointers
        std::unordered_map<std::string, FMOD::Sound*> m_loadedSFX;
        std::unordered_map<std::string, FMOD::Sound*> m_loadedBGM;
        // Maps string keys to numeric IDs for ResourceManager
        std::unordered_map<std::string, size_t> m_keyToID;

        // Audio keys for all game sounds
        //AudioKeys m_audioKeys;

        //std::unordered_map<Entity, AudioComponent> m_audioComponents;

        // State
        bool m_initialized;
        bool m_bgmPaused;

        // CurrentBGM
        std::string m_currentKeyBGM;
        std::unordered_map<std::string, FMOD::Channel*> m_bgmChannels;  // Map of BGM key to channel

        // Volume levels
        float m_masterVolume;
        float m_bgmVolume;
        float m_sfxVolume;

        // user-set bgmvolume
        float m_userBGMVolume = 0.2f;

        // for transition audio
        bool m_isFading;
        float m_fadeTimer;
        float m_fadeDuration;
        float m_fadeTargetVolume;

        /**
         * @brief calculates the distance of the player to the entity
		 * @param e the entity to calculate distance to
         * @return distance from player to entity
         */
        float GetDistanceToPlayer(Entity e);

        /**
         * @brief loads a single sound file and assigns it a string key based on filename
         * @param filepath path to the audio file
         * @param isBGM true if this is background music, false if SFX
         * @return true if loaded properly, else false
         */
        bool LoadSoundFile(const std::string& filepath, bool isBGM);

        /**
         * @brief scans a directory and loads all audio files
         * @param folderPath path to the folder to scan
         * @param isBGM true if loading BGM, false if loading SFX
         */
        void ScanAudioDirectory(const std::string& folderPath, bool isBGM);

        /**
         * @brief extracts filename without extension to use as key
         * @param filepath full file path
         * @return filename without extension
         */
        std::string ExtractKeyFromPath(const std::string& filepath) const;

        /**
         * @brief checks if a file has a valid audio extension
         * @param filepath path to check
         * @return true if file is a valid audio format
         */
        bool IsValidAudioFile(const std::string& filepath) const;

    };

} // namespace NightSnack

#endif // AUDIOSYSTEM_H