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

    /**
     * @struct AudioKeys
	 * @brief stores keys for sound effects and background music
     */
    struct AudioKeys {
        // SFX
        size_t buttonClick = 0;

        // BGM
        size_t mainMenu = 0;
    };

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
        void Update(int steps) override;

        /**
        * @brief retrieve audio keys to access sound
		* @return reference to AudioKeys struct
        */
        const AudioKeys& GetAudioKeys() const { return m_audioKeys; }

        /**
         * @brief loads a single sound file
         * @param filepath path to the audio file
         * @param outKey output key for the loaded sound
         * @return true if loaded properly, else false
         */
        bool LoadSoundFile(const std::string& filepath, size_t& outKey);

        /**
         * @brief loads all game audio files
         */
        void LoadAllGameAudio();

        // SFX Functions

        /**
         * @brief plays a sound effect (SFX) by its key
         * @param soundID key of the sound effect to play
         */
        void PlaySFX(const size_t soundID);

        /**
         * @brief plays a sound effect (SFX) with a specified volume
         * @param soundID key of the sound effect to play
         * @param volume volume level (0.0 to 1.0)
         */
        void PlaySFXWithVolume(const size_t soundID, float volume);

        // BGM Functions

        /**
         * @brief plays background music (BGM) by its key
         * @param bgmID key of the background music to play
         */
        void PlayBGM(const size_t bgmID);

        /**
         * @brief stops the currently playing background music
         */
        void StopBGM();

        /**
         * @brief stops the currently playing background music
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
         * @return SFX volume level (0.0 to 1.0).
         */
        float GetSFXVolume() const;

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