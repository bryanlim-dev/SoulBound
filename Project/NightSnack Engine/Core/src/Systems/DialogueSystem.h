/******************************************************************************
 * File:        DialogueSystem.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Dialogue System
 *
 * Description:
 * This file contains the declarations of the Dialogue System.
 * - Loading and storing all existing Dialogue Sequences from the disk using JSON files
 * - Render any Dialogue Sequence line by line, using a typewriter effect
 * - Ability to reveal the whole line or continue to the next line upon key press input
 * - When whole line is revealed, hint instruction text is shown
 * - Ability to update and render the corresponding material and speaker name for every dialogue line
 * 
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/AudioSystem.h"
#include <unordered_map>

namespace NightSnack {

	class DialogueSystem : public ISystem {
	public:

        enum class DialogueState {
            INACTIVE,       // No dialogues sequences currently being displayed
            TYPING,         // In the midst of revealing a Dialogue Line 
            WAITING_INPUT   // Successfully revealed a whole Dialogue Line
        };

        struct DialogueLine {
            std::string ID;
            std::string speaker;
            std::string speakerMaterial;
            std::string text;
            std::string nextID;
            float charDelay{ 0.5f };
        };

        struct DialogueSequence {
            std::string sequenceID; // injected from filename at load time
            std::string startID; // which line to start from
            std::vector<std::pair<std::string, DialogueLine>> lines; // ordered

            bool isValid() const { return !startID.empty() && !lines.empty(); }

            DialogueLine* FindLine(const std::string& id) {
                for (auto& [lineID, line] : lines) {
                    if (lineID == id) {
                        return &line;
                    }
                }
                return nullptr;
            }
        };

        /************************************************************************/
        /*!
        \brief
        Constructor for the DialogueSystem
        Sets up the system signatures so that this system only operates on entities
        that contain components 

        \param[in] coord
        Reference to the ECS Coordinator

        \param[in] dispatcher
        Reference to the event dispatcher used for Input Events
        */
        /************************************************************************/
        DialogueSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
            m_systemSignature.set(GetComponentTypeID<Transform>());
            Init();

            // Dispatcher lambda
            m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
                if (event.pressed == InputEvent::InputActionState::Pressed) {
                    switch (event.inputAction) {
                        
                    case InputEvent::InputAction::DefaultDialogue:
                        m_testDialogue = true;
                        break;

                    case InputEvent::InputAction::SkipDialogueOrCameraPan:
                    case InputEvent::InputAction::ClickLMB:
                        // Either complete the whole line or move on to the next line
                        Advance();
                        break;
                    }
                }
                });
        }

        /************************************************************************/
        /*!
        \brief
        Initializes the DialogueSystem.
        */
        /************************************************************************/
        void Init();

        /************************************************************************/
        /*!
        \brief
        Main update loop
        */
        /************************************************************************/
        void Update() override;

        /************************************************************************/
        /*!
        \brief
        Set up all Dialogue related UI entities in the level
        */
        /************************************************************************/
        void SetupUIEntities();

        /************************************************************************/
        /*!
        \brief
        Search and start playing a dialogue sequence
        */
        /************************************************************************/
        void StartDialogue(const std::string& sequenceID);

        /************************************************************************/
        /*!
        \brief
        Either reveal the whole line immediately or move on to the next line
        */
        /************************************************************************/
        void Advance();

        /************************************************************************/
        /*!
        \brief
        Stop the dialogue sequence immediatetly
        */
        /************************************************************************/
        void StopDialogue();

        /************************************************************************/
        /*!
        \brief
        Create a dialogue sequence
        */
        /************************************************************************/
        bool CreateSequence(const std::string& sequenceID); 

        /************************************************************************/
        /*!
        \brief
        Delete a dialogue sequence
        */
        /************************************************************************/
        bool DeleteSequence(const std::string& sequenceID);

        /************************************************************************/
        /*!
        \brief
        Rename a dialogue sequence
        */
        /************************************************************************/
        bool RenameSequence(const std::string& oldID, const std::string& newID);

        /************************************************************************/
        /*!
        \brief
        Save this dialogue sequence
        */
        /************************************************************************/
        void SaveSequence(const std::string& sequenceID);

        /************************************************************************/
        /*!
        \brief
        Create a dialogue line in this sequence
        */
        /************************************************************************/
        void AddLine(const std::string& sequenceID, const DialogueLine& line);

        /************************************************************************/
        /*!
        \brief
        Delete this dialogue line in this sequence
        */
        /************************************************************************/
        void DeleteLine(const std::string& sequenceID, const std::string& lineID);

        /************************************************************************/
        /*!
        \brief
        Update this dialogue line in this sequence
        */
        /************************************************************************/
        void UpdateLine(const std::string& sequenceID, const std::string& oldLineID, const DialogueLine& updated);

        std::unordered_map<std::string, DialogueSequence>& GetDialogueLibrary() { return m_dialogueLibrary; }

        // for dialogue played
        std::unordered_set<std::string> m_playedSequences;

        bool IsActive() const { return m_dialogueState != DialogueState::INACTIVE; }

        bool IsTypingDone() const { return m_dialogueState == DialogueState::WAITING_INPUT; }

        const std::string& GetVisibleText() const { return m_visibleText; }

        const std::string& GetCurrentSpeaker() const { return m_currentSpeaker; }

        void ResetUIEntities() { 
            m_foundUIEntities = false; 
            m_dialogueBoxEntity = INVALID_ENTITY;
            m_speakerNameEntity = INVALID_ENTITY;
            m_dialogueTextEntity = INVALID_ENTITY;
            m_portraitEntity = INVALID_ENTITY;
            m_hintTextEntity = INVALID_ENTITY;
        }

        // Required by ISystem
        std::string GetName() const override { return "DialogueSystem"; }
        Signature GetSignature() const override { return m_systemSignature; }

    private:

        /************************************************************************/
        /*!
        \brief
        Load all dialogue sequences from the disk
        */
        /************************************************************************/
        void LoadAllSequences();

        /************************************************************************/
        /*!
        \brief
        Switch to a new dialogue ID and reset typewriter state
        */
        /************************************************************************/
        void LoadLine(const std::string& lineID);

        /************************************************************************/
        /*!
        \brief
        Load this particular dialogue sequence from the disk
        */
        /************************************************************************/
        DialogueSequence LoadDialogueSequence(const std::string& filePath);

        /************************************************************************/
        /*!
        \brief
        Reveal the whole dialogue line immediately
        */
        /************************************************************************/
        void FinishTyping();

        /************************************************************************/
        /*!
        \brief
        Toggle visibility of dialogue related UI entities
        */
        /************************************************************************/
        void ToggleUIEntities(bool active);

        /************************************************************************/
        /*!
        \brief
        Update the necessary revealed text to the TextSystem to render
        */
        /************************************************************************/
        void UpdateUIText();

        /************************************************************************/
        /*!
        \brief
        Update the current renderable material of the speaker entity
        */
        /************************************************************************/
        void UpdateSpeakerPortrait();

        std::unordered_map<std::string, DialogueSequence> m_dialogueLibrary;

        // Active Sequence State
        DialogueState m_dialogueState{ DialogueState::INACTIVE };
        DialogueSequence m_currentSequence{};
        std::string m_currentLineID{};
        std::string m_currentSpeaker{};
        std::string m_currentSpeakerMaterial{};

        // Typewrite State
        std::string m_fullText{};
        std::string m_visibleText{};
        int m_visibleCharCount{};
        float m_typingTimer{};
        float m_charDelay{ 0.05f };

        // UI Entities
        Entity m_dialogueBoxEntity{ INVALID_ENTITY };
        Entity m_speakerNameEntity{ INVALID_ENTITY };
        Entity m_dialogueTextEntity{ INVALID_ENTITY };
        Entity m_portraitEntity{ INVALID_ENTITY };
        Entity m_hintTextEntity{ INVALID_ENTITY };
        bool m_foundUIEntities{ false };

        bool m_testDialogue{ false };

        Coordinator& m_coord;
        EventDispatcher* m_dispatcher = nullptr;
        Signature m_systemSignature{};
	};

    // DIALOGUE LINE
    inline void to_json(json& j, DialogueSystem::DialogueLine const& d) {
        j = {
            { "speaker", d.speaker },
            { "speakerMaterial", d.speakerMaterial },
            { "text", d.text },
            { "nextID", d.nextID },
            { "charDelay", d.charDelay }
        };
    }

    inline void from_json(json const& j, DialogueSystem::DialogueLine& d) {
        try {
            d.speaker = j.value("speaker", "");
            d.speakerMaterial = j.value("speakerMaterial", "");
            d.text = j.value("text", "");
            d.nextID = j.value("nextID", "");
            d.charDelay = j.value("charDelay", 0.05f);
        }
        catch (std::exception const& e) {
            std::cerr << "Invalid JSON format for DialogueLine: " << e.what() << std::endl;
        }
    }

    // DIALOGUE SEQUENCE
    inline void to_json(json& j, DialogueSystem::DialogueSequence const& ds) {
        j["startID"] = ds.startID;
        
        // Use an array to preserve insertion order
        j["lines"] = json::array();
        for (auto const& [ID, line] : ds.lines) {
            json entry = line; // calls to_json(DialogueLine)
            entry["ID"] = ID;  // embed the ID inside the object
            j["lines"].push_back(entry);
        }
    }

    inline void from_json(json const& j, DialogueSystem::DialogueSequence& ds) {
        try {
            ds.startID = j.value("startID", "");

            for (auto const& entry : j.at("lines")) {
                std::string ID = entry.value("ID", "");
                DialogueSystem::DialogueLine line = entry.get<DialogueSystem::DialogueLine>();
                line.ID = ID;
                ds.lines.push_back({ ID, line });
            }
        }
        catch (std::exception const& e) {
            std::cerr << "Invalid JSON format for DialogueSequence: " << e.what() << std::endl;
        }
    }
}