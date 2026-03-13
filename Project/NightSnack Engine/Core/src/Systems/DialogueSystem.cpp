/******************************************************************************
 * File:        DialogueSystem.cpp
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Dialogue System
 *
 * Description:
 * This file contains the definitions of the Dialogue System.
 * - Loading and storing all existing Dialogue Sequences from the disk using JSON files
 * - Render any Dialogue Sequence line by line, using a typewriter effect
 * - Ability to reveal the whole line or continue to the next line upon key press input
 * - When whole line is revealed, hint instruction text is shown
 * - Ability to update and render the corresponding material and speaker name for every dialogue line
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "DialogueSystem.h"
#include "Systems/PlayerController.h"
#include "Systems/Physics/Physics.h"
#include <fstream>
#include <filesystem>

namespace NightSnack {

	void DialogueSystem::Init() {
        m_dialogueState = DialogueState::INACTIVE;
        LoadAllSequences();

        /*for (Entity e : m_entities)
        {
            auto& t = m_coord.GetComponent<Transform>(e);

            if (t.name.find("DialogueCollider_") != std::string::npos)
            {
                m_dialogueTriggers[e] = false;
            }
        }*/
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
            else if (t.name == "DialogueHintText") m_hintTextEntity = e;
        }

        if (m_dialogueBoxEntity == INVALID_ENTITY || m_speakerNameEntity == INVALID_ENTITY ||
            m_dialogueTextEntity == INVALID_ENTITY || m_portraitEntity == INVALID_ENTITY || 
            m_hintTextEntity == INVALID_ENTITY) {
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

        for (Entity e : m_entities)
        {
            if (!m_coord.GetComponent<Transform>(e).isActive) continue;

            auto& transform = m_coord.GetComponent<Transform>(e);

            // any DialogueCollider_ entity
            if (!m_coord.HasComponent<Collider>(e)) continue;

            const std::string& name = transform.name;
            size_t prefixPos = name.find("DialogueCollider_");
            if (prefixPos == std::string::npos) continue;

            // Extract the sequence ID from the name after the prefix
            std::string sequenceID = name.substr(prefixPos + std::strlen("DialogueCollider_"));

            if (sequenceID.empty()) continue;

            // skip if this sequence has already been played
            if (m_playedSequences.count(sequenceID)) continue;

            // collider must be trigger type
            if (m_coord.GetComponent<Collider>(e).type != ColliderType::Trigger) continue;

            Entity player = m_coord.GetSystem<PlayerControllerSystem>()->FindPlayer(m_coord);

            if (m_coord.GetSystem<PhysicsSystem>()->EntitiesAreColliding(e, player))
            {
                m_playedSequences.insert(sequenceID); // mark as played before starting
                StartDialogue(sequenceID);
            }
        }
       

        // TESTING CODE TO START DIALOGUE 
        if (m_testDialogue && m_dialogueBoxEntity != INVALID_ENTITY) {
            StartDialogue("test_dialogue1");
            m_testDialogue = false;
        }

        if (m_dialogueState == DialogueState::INACTIVE) {
            return;
        }

        if (m_dialogueState == DialogueState::TYPING) {
            // Advance the typewriter timer each frame
            m_typingTimer += FixedDT;

            // Reveal characters one at a time based on the timer
            // Uses a while loop instead of an if statement so that it will reveal multiple characters 
            // to catch up if needed
            while (m_typingTimer >= m_charDelay && m_visibleCharCount < static_cast<int>(m_fullText.size())) {
                ++m_visibleCharCount;
                m_typingTimer -= m_charDelay;
                
                // play typewriter tick per character, skip spaces
                char c = m_fullText[m_visibleCharCount - 1];
                if (c != ' ' && c != '\n') 
                {
                    m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("typewriterClack", 0.15f);
                }   
            }

            m_visibleText = m_fullText.substr(0, m_visibleCharCount);

            // Check if the full line has been revealed
            if (m_visibleCharCount >= static_cast<int>(m_fullText.size())) {
                m_dialogueState = DialogueState::WAITING_INPUT;
            }
        }

        if (m_hintTextEntity != INVALID_ENTITY) {
            if (m_dialogueState == DialogueState::WAITING_INPUT) {
                // Dialogue System is currently waiting for user key press to advance to the next line
                // Show the Hint Text for player to advance
                m_coord.ModifyComponent<Transform>(m_hintTextEntity).isActive = true;
            } else {
                // Hide the Hint Text 
                m_coord.ModifyComponent<Transform>(m_hintTextEntity).isActive = false;
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

        if (m_dialogueState != DialogueState::INACTIVE)
        {
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
            DialogueLine* line = m_currentSequence.FindLine(m_currentLineID);
            if (!line) {
                StopDialogue();
                return;
            }

            //const std::string& nextID = it->second.nextID;
            const std::string& nextID = line->nextID;
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
        DialogueLine* line = m_currentSequence.FindLine(lineID);
        if (!line) {
            std::cerr << "[DialogueSystem] Line ID not found: " << lineID << std::endl;
            StopDialogue();
            return;
        }

        m_currentLineID = lineID;
        m_fullText = line->text;
        // Normalize line endings for all platforms, since it might store new lines as \r\n
        m_fullText.erase(std::remove(m_fullText.begin(), m_fullText.end(), '\r'), m_fullText.end());

        m_currentSpeaker = line->speaker;
        m_currentSpeakerMaterial = line->speakerMaterial;
        m_charDelay = line->charDelay;

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
            std::cerr << "[DialogueSystem] Failed to parse: " << filePath << " � " << e.what() << '\n';
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
        // This function is used to toggle the visibility of UI entities 

        if (m_dialogueBoxEntity != INVALID_ENTITY)
            m_coord.ModifyComponent<Transform>(m_dialogueBoxEntity).isActive = active;
        if (m_speakerNameEntity != INVALID_ENTITY)
            m_coord.ModifyComponent<Transform>(m_speakerNameEntity).isActive = active;
        if (m_dialogueTextEntity != INVALID_ENTITY)
            m_coord.ModifyComponent<Transform>(m_dialogueTextEntity).isActive = active;
        if (m_portraitEntity != INVALID_ENTITY)
            m_coord.ModifyComponent<Transform>(m_portraitEntity).isActive = active;

        // Only toggle the hint text entity off here since its visibility is determined by
        // whether the current dialogue line is complete or not
        if (!active && m_hintTextEntity != INVALID_ENTITY) {
            m_coord.ModifyComponent<Transform>(m_hintTextEntity).isActive = active;
        }
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

    bool DialogueSystem::CreateSequence(const std::string& sequenceID) {
        if (m_dialogueLibrary.count(sequenceID)) {
            std::cerr << "[DialogueSystem] Sequence already exists: " << sequenceID << std::endl;
            return false;
        }

        DialogueSequence seq;
        seq.sequenceID = sequenceID;
        seq.startID = "";
        m_dialogueLibrary[sequenceID] = seq;
        SaveSequence(sequenceID);  // create the json file immediately
        return true;
    }

    bool DialogueSystem::DeleteSequence(const std::string& sequenceID) {
        m_dialogueLibrary.erase(sequenceID);

        std::filesystem::path filePath = std::filesystem::path(RESOURCE_PATH) / "Dialogue" / (sequenceID + ".json");
        if (std::filesystem::exists(filePath)) {
            std::filesystem::remove(filePath);
        }

        return true;
    }

    bool DialogueSystem::RenameSequence(const std::string& oldID, const std::string& newID) {
        if (m_dialogueLibrary.count(newID)) {
            std::cerr << "[DialogueSystem] Sequence ID already taken: " << newID << std::endl;
            return false;
        }

        auto node = m_dialogueLibrary.extract(oldID);
        if (node.empty()) {
            return false;
        }

        node.key() = newID;
        node.mapped().sequenceID = newID;
        m_dialogueLibrary.insert(std::move(node));

        // Rename file on disk
        std::filesystem::path oldPath = std::filesystem::path(RESOURCE_PATH) / "Dialogue" / (oldID + ".json");
        std::filesystem::path newPath = std::filesystem::path(RESOURCE_PATH) / "Dialogue" / (newID + ".json");

        if (std::filesystem::exists(oldPath)) {
            std::filesystem::rename(oldPath, newPath);
        }

        SaveSequence(newID);
        return true;
    }

    void DialogueSystem::SaveSequence(const std::string& sequenceID) {
        auto it = m_dialogueLibrary.find(sequenceID);
        if (it == m_dialogueLibrary.end()) {
            return;
        }

        std::filesystem::path filePath = std::filesystem::path(RESOURCE_PATH) / "Dialogue" / (sequenceID + ".json");

        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "[DialogueSystem] Could not write: " << filePath << '\n';
            return;
        }

        json j = it->second;
        file << j.dump(4); // pretty print with 4 space indent
        std::cout << "[DialogueSystem] Saved: " << filePath << '\n';
    }

    void DialogueSystem::AddLine(const std::string& sequenceID, const DialogueLine& line) {
        auto it = m_dialogueLibrary.find(sequenceID);
        if (it == m_dialogueLibrary.end()) return;

        DialogueSequence& seq = it->second;
        seq.lines.push_back({ line.ID, line });

        // If this is the first line, make it the start
        if (seq.lines.size() == 1)
            seq.startID = line.ID;
    }

    void DialogueSystem::DeleteLine(const std::string& sequenceID, const std::string& lineID) {
        auto it = m_dialogueLibrary.find(sequenceID);
        if (it == m_dialogueLibrary.end()) return;

        auto& lines = it->second.lines;
        lines.erase(std::remove_if(lines.begin(), lines.end(),
            [&](auto const& pair) { return pair.first == lineID; }), lines.end());

        // Update startID if the deleted line was the start
        if (it->second.startID == lineID)
            it->second.startID = lines.empty() ? "" : lines.front().first;
    }

    void DialogueSystem::UpdateLine(const std::string& sequenceID, const std::string& oldLineID, const DialogueLine& updated) {
        auto it = m_dialogueLibrary.find(sequenceID);
        if (it == m_dialogueLibrary.end()) return;

        auto& lines = it->second.lines;
        for (auto& [ID, line] : lines) {
            if (ID == oldLineID) {
                ID = updated.ID;   // update the key too if ID changed
                line = updated;
                break;
            }
        }

        // Update startID if the first line's ID changed
        if (!lines.empty() && it->second.startID == oldLineID)
            it->second.startID = updated.ID;
    }

}