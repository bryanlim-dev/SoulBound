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
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <unordered_map>
#include <string>

namespace NightSnack {

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

        std::shared_ptr<FMOD::System> fmodSystem; //Shared with resource manager
        std::shared_ptr<ResourceManager> resourceManager;

        // Track active channels separately
        std::unordered_map<std::size_t, FMOD::Channel*> sfxChannels;
        std::unordered_map<std::size_t, FMOD::Channel*> musicChannels;

        bool initialized;

        // Private constructor for singleton
        AudioSystem();

    public:
        ~AudioSystem();

        /************************************************************************/
        /*!
        \brief
        Gets instance of AudioSystem

        \return
        Pointer to AudioSystem
        */
        /************************************************************************/
        static AudioSystem* GetInstance();

        /************************************************************************/
        /*!
        \brief
        Destroys instance of AudioSystem

        */
        /************************************************************************/
        static void DestroyInstance();

        /************************************************************************/
        /*!
        \brief
        Init AudioSystem

        \return
        True if succeed, else false
        */
        /************************************************************************/
        bool Initialize();

        /************************************************************************/
        /*!
        \brief
        Update FMOD system and clean finished sounds
        */
        /************************************************************************/
        void Update();

        /************************************************************************/
        /*!
        \brief
        Sets resource manager

        \return
        Pointer to resource manager
        */
        /************************************************************************/
        void SetResourceManager(std::shared_ptr<ResourceManager> rm);

        /************************************************************************/
        /*!
        \brief
        Load sound effect to memory

        \param path
        File path of the sound effect

        \return
        Unique sound key
        */
        /************************************************************************/
        std::size_t LoadSFX(const std::string& path);

        /************************************************************************/
        /*!
        \brief
        Load music to memory

        \param path
        File path of the music

        \param loop
        Loop or not; default true

        \return
        Unique music key
        */
        /************************************************************************/
        std::size_t LoadMusic(const std::string& path, bool loop = true);


        /************************************************************************/
        /*!
        \brief
        Play sound by the key

        \param soundKey
        Sound key to play

        \return
        True if success, else false
        */
        /************************************************************************/
        bool PlaySound(std::size_t soundKey);

        /************************************************************************/
        /*!
        \brief
        Stop sound by the key

        \param soundKey
        Sound key to stop

        \return
        True if success, else false
        */
        /************************************************************************/
        bool StopSound(std::size_t soundKey);

        /************************************************************************/
        /*!
        \brief
        Stop all audio (sound and music)
        */
        /************************************************************************/
        void StopAllSounds();

        /************************************************************************/
        /*!
        \brief
        Pause or resume sound

        \param soundKey
        Sound key to pause or play

        \param pause
        True to pause, false to resume.

        \return
        True if success, else false
        */
        /************************************************************************/
        bool PauseSound(std::size_t soundKey, bool pause);

        /************************************************************************/
        /*!
        \brief
        Get FMOD system shared_ptr

        \return
        Shared Pointer to FMOD system
        */
        /************************************************************************/
        std::shared_ptr<FMOD::System> GetFMODSystem() { return  fmodSystem; }


        /************************************************************************/
        /*!
        \brief
        Check if Audio is init

        \return
        True if init, else false
        */
        /************************************************************************/
        bool IsInitialized() { return initialized; }
    };

}   // NightSnack
