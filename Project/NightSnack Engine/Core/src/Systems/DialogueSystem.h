#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/AudioSystem.h"

namespace NightSnack {

	class DialogueSystem : public ISystem {
	public:

        enum class DialogueState {
            INACTIVE,
            TYPING,
            WAITING_INPUT
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
                        
                    case InputEvent::InputAction::DisplayFPS:
                        m_testDialogue = true;
                        break;

                    case InputEvent::InputAction::SkipDialogue:
                    case InputEvent::InputAction::ClickLMB:
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

        void SetupUIEntities();

        // Search and start playing a dialogue sequence
        void StartDialogue(const std::string& sequenceID);

        // Call this when the player presses the "advance" button
        void Advance();

        // Stop dialogue immediately
        void StopDialogue();

        bool CreateSequence(const std::string& sequenceID); 
        bool DeleteSequence(const std::string& sequenceID);
        bool RenameSequence(const std::string& oldID, const std::string& newID);
        void SaveSequence(const std::string& sequenceID);

        void AddLine(const std::string& sequenceID, const DialogueLine& line);
        void DeleteLine(const std::string& sequenceID, const std::string& lineID);
        void UpdateLine(const std::string& sequenceID, const std::string& oldLineID, const DialogueLine& updated);

        std::unordered_map<std::string, DialogueSequence>& GetDialogueLibrary() { return m_dialogueLibrary; }

        bool IsActive() const { return m_dialogueState != DialogueState::INACTIVE; }

        bool IsTypingDone() const { return m_dialogueState == DialogueState::WAITING_INPUT; }

        const std::string& GetVisibleText() const { return m_visibleText; }

        const std::string& GetCurrentSpeaker() const { return m_currentSpeaker; }

        void ResetUIEntities() { m_foundUIEntities = false; }

        // Required by ISystem
        std::string GetName() const override { return "DialogueSystem"; }
        Signature GetSignature() const override { return m_systemSignature; }

    private:

        void LoadAllSequences();
        void LoadLine(const std::string& lineID);
        DialogueSequence LoadDialogueSequence(const std::string& filePath);
        void FinishTyping();

        void ToggleUIEntities(bool active);
        void UpdateUIText();
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