#include "DialogueSystem.h"
#include <fstream>
#include <filesystem>

namespace NightSnack {

	void DialogueSystem::Init() {
        m_dialogueState = DialogueState::INACTIVE;
        LoadAllSequences();
	}

    void DialogueSystem::LoadAllSequences() {
        std::filesystem::path dialoguePath = std::filesystem::canonical(
            RESOURCE_PATH + std::string{ "Dialogue" }
        );

        if (!std::filesystem::exists(dialoguePath)) {
            std::cerr << "[DialogueSystem] Dialogue directory not found!\n";
            return;
        }

        for (auto const& entry : std::filesystem::directory_iterator(dialoguePath)) {
            if (entry.is_directory()) continue;
            if (entry.path().extension().string() != ".json") continue;

            std::string filePath = std::string{ "Dialogue/" } + entry.path().filename().string();

            // Derive the sequence ID from the filename stem
            // e.g. "guard_intro.json" -> "guard_intro"
            std::string sequenceID = entry.path().stem().string();

            DialogueSequence sequence = LoadDialogueSequence(filePath);

            if (sequence.isValid()) {
                // Inject the sequence ID (derived from filename)
                sequence.sequenceID = sequenceID;
                m_dialogueLibrary[sequenceID] = std::move(sequence);
                //std::cout << "[DialogueSystem] Loaded sequence: " << sequenceID << '\n';
            } else {
                //std::cerr << "[DialogueSystem] Failed to load: " << filePath << '\n';
            }
        }

        //std::cout << "[DialogueSystem] Total sequences loaded: " << m_dialogueLibrary.size() << '\n';
    }

    void DialogueSystem::SetupUIEntities() {
        for (Entity e : m_entities) {
            auto& t = m_coord.GetComponent<Transform>(e);
            if (t.layer != LayerType::GAMEUI) continue;

            // Allocate the UI entities by name
            if (t.name == "DialogueBox") m_dialogueBoxEntity = e;
            else if (t.name == "DialogueSpeakerName") m_speakerNameEntity = e;
            else if (t.name == "DialogueText") m_dialogueTextEntity = e;
            else if (t.name == "DialoguePortrait") m_portraitEntity = e;
        }

        if (m_dialogueBoxEntity == INVALID_ENTITY || m_speakerNameEntity == INVALID_ENTITY ||
            m_dialogueTextEntity == INVALID_ENTITY || m_portraitEntity == INVALID_ENTITY) {
            //std::cerr << "[DialogueSystem] Missing UI entities in scene\n";
            return;
        }

        m_foundUIEntities = true;
        ToggleUIEntities(false);
    }

	void DialogueSystem::Update() {
        if (!m_foundUIEntities) {
            SetupUIEntities();
            return;
        }

        // TESTING CODE TO START DIALOGUE 
        if (m_testDialogue && m_dialogueBoxEntity != INVALID_ENTITY) {
            StartDialogue("guard_intro");
            m_testDialogue = false;
        }

        if (m_dialogueState == DialogueState::INACTIVE) return;

        if (m_dialogueState == DialogueState::TYPING) {
            // Advance the typewriter timer each frame
            m_typingTimer += FixedDT;

            // Reveal characters one at a time based on the timer
            // Uses a while loop instead of an if statement so that it will reveal multiple characters 
            // to catch up if needed
            while (m_typingTimer >= m_typingSpeed && m_visibleCharCount < static_cast<int>(m_fullText.size())) {
                ++m_visibleCharCount;
                m_typingTimer -= m_typingSpeed;
            }

            m_visibleText = m_fullText.substr(0, m_visibleCharCount);

            // Check if the full line has been revealed
            if (m_visibleCharCount >= static_cast<int>(m_fullText.size())) {
                m_dialogueState = DialogueState::WAITING_INPUT;
            }
        }

        // Push the current visible text to the font component
        UpdateUIText();
	}

    // Search and start playing a dialogue sequence
    void DialogueSystem::StartDialogue(const std::string& sequenceID) {
        auto it = m_dialogueLibrary.find(sequenceID);
        if (it == m_dialogueLibrary.end()) {
            std::cerr << "[DialogueSystem] Sequence not found: " << sequenceID << std::endl;
            return;
        }

        m_currentSequence = it->second;
        ToggleUIEntities(true);
        LoadLine(m_currentSequence.startID);
    }

    // Call this when the player presses the "advance" button
    void DialogueSystem::Advance() {
        // When the dialogue is typing, skip and show the full line immediately
        if (m_dialogueState == DialogueState::TYPING) {
            FinishTyping();
            return;
        }

        // When the dialogue is waiting for input, move on to the next line
        if (m_dialogueState == DialogueState::WAITING_INPUT) {
            auto it = m_currentSequence.lines.find(m_currentLineID);
            if (it == m_currentSequence.lines.end()) {
                StopDialogue();
                return;
            }

            const std::string& nextID = it->second.nextID;
            if (nextID.empty()) {
                // There is no next line, whole dialogue sequence is over
                StopDialogue();
            } else {
                LoadLine(nextID);
            }
        }
    }

    // Stop dialogue immediately
    void DialogueSystem::StopDialogue() {
        m_dialogueState = DialogueState::INACTIVE;
        m_fullText = "";
        m_visibleText = "";
        m_currentSpeaker = "";
        m_currentSpeakerMaterial = "";
        m_currentLineID = "";
        m_visibleCharCount = 0;
        m_typingTimer = 0.f;
        m_currentSequence = DialogueSequence{};

        // Reset the text in the Font components
        if (m_dialogueTextEntity != INVALID_ENTITY && m_coord.HasComponent<Font>(m_dialogueTextEntity)) {
            m_coord.ModifyComponent<Font>(m_dialogueTextEntity).text = "";
        }
        if (m_speakerNameEntity != INVALID_ENTITY && m_coord.HasComponent<Font>(m_speakerNameEntity)) {
            m_coord.ModifyComponent<Font>(m_speakerNameEntity).text = "";
        }

        ToggleUIEntities(false);
    }

    // Switch to a new line by ID and reset typewriter state
    void DialogueSystem::LoadLine(const std::string& lineID) {
        auto it = m_currentSequence.lines.find(lineID);
        if (it == m_currentSequence.lines.end()) {
            std::cerr << "[DialogueSystem] Line ID not found: " << lineID << std::endl;
            StopDialogue();
            return;
        }

        DialogueLine const& line = it->second;

        m_currentLineID = lineID;
        m_fullText = line.text;
        m_currentSpeaker = line.speaker;
        m_currentSpeakerMaterial = line.speakerMaterial;
        m_typingSpeed = line.typingSpeed;

        // Reset typewriter
        m_visibleCharCount = 0;
        m_visibleText = "";
        m_typingTimer = 0.f;
        m_dialogueState = DialogueState::TYPING;

        // Update the portrait to match the new speaker
        UpdateSpeakerPortrait();
    }

    DialogueSystem::DialogueSequence DialogueSystem::LoadDialogueSequence(const std::string& filePath) {
        DialogueSequence result{};
        std::ifstream file(RESOURCE_PATH + filePath);

        if (!file.is_open()) {
            std::cerr << "[DialogueSystem] Could not open file: " << filePath << '\n';
            return result;
        }

        try {
            json j;
            file >> j;
            result = j.get<DialogueSequence>();
        }
        catch (std::exception const& e) {
            std::cerr << "[DialogueSystem] Failed to parse: " << filePath << " — " << e.what() << '\n';
        }

        return result;
    }

    void DialogueSystem::FinishTyping() {
        m_visibleCharCount = static_cast<int>(m_fullText.size());
        m_visibleText = m_fullText;
        m_typingTimer = 0.f;
        m_dialogueState = DialogueState::WAITING_INPUT;
    }

    void DialogueSystem::ToggleUIEntities(bool active) {
        if (m_dialogueBoxEntity != INVALID_ENTITY)
            m_coord.ModifyComponent<Transform>(m_dialogueBoxEntity).isActive = active;
        if (m_speakerNameEntity != INVALID_ENTITY)
            m_coord.ModifyComponent<Transform>(m_speakerNameEntity).isActive = active;
        if (m_dialogueTextEntity != INVALID_ENTITY)
            m_coord.ModifyComponent<Transform>(m_dialogueTextEntity).isActive = active;
        if (m_portraitEntity != INVALID_ENTITY)
            m_coord.ModifyComponent<Transform>(m_portraitEntity).isActive = active;
    }

    void DialogueSystem::UpdateUIText() {
        if (m_dialogueTextEntity != INVALID_ENTITY && m_coord.HasComponent<Font>(m_dialogueTextEntity)) {
            m_coord.ModifyComponent<Font>(m_dialogueTextEntity).text = m_visibleText;
        } else {
            std::cerr << "[DialogueSystem] Dialogue Text Entity does not have Font component.\n";
        }

        if (m_speakerNameEntity != INVALID_ENTITY && m_coord.HasComponent<Font>(m_speakerNameEntity)) {
            m_coord.ModifyComponent<Font>(m_speakerNameEntity).text = m_currentSpeaker;
        } else {
            std::cerr << "[DialogueSystem] Speaker Name Entity does not have Font component.\n";
        }
    }

    void DialogueSystem::UpdateSpeakerPortrait() {
        if (m_portraitEntity == INVALID_ENTITY) return;

        auto& r = m_coord.ModifyComponent<Renderable>(m_portraitEntity);

        // Hide portrait entirely if no speaker is set (narration lines)
        if (m_currentSpeakerMaterial.empty()) {
            m_coord.ModifyComponent<Transform>(m_portraitEntity).isActive = false;
            return;
        }

        m_coord.ModifyComponent<Transform>(m_portraitEntity).isActive = true;
        r.materialName = m_currentSpeakerMaterial;
    }
}