

#pragma once
#include "AllPanels.h"
#include "Systems/DialogueSystem.h"
#include "Systems/Graphics.h"

namespace fs = std::filesystem;

namespace NightSnack
{

	/**
	* @brief Dialogue panel 
	*/
	class DialoguePanel {
	public:
		/**
		* @brief constructor
		* @param coord reference to the ECS coordinator
		*/
		DialoguePanel(Coordinator& coord) :	m_coord(coord)
		{

		}

		/**
		* @brief draw the dialogue panel
		*/
		void Draw()
		{
			ImGui::Begin("Dialogue");

            // When user presses 'New' button to create a new sequence
			DrawNewSequenceModal();
            // Draw the current dialogue sequence and its sequence ID
			DrawSequenceSelector();

            // Only draw the lines of the currently selected sequence
			if (!m_selectedSequenceID.empty()) {
				ImGui::Separator();
				DrawLineList();
				ImGui::Separator();
				DrawLineProperties();
			}

			ImGui::End();
		}

	private:

		void DrawSequenceSelector()
		{
            // Draw the current dialogue sequence and its sequence ID

            auto dialogueSystem = m_coord.GetSystem<DialogueSystem>();
            auto& library = dialogueSystem->GetDialogueLibrary();

            // Build a sorted list of sequence IDs from the library in the combo
            std::vector<std::string> sequenceIDs;
            for (auto const& [ID, seq] : library) {
                sequenceIDs.push_back(ID);
            }
            // Sorted alphabetically in ascending order
            std::sort(sequenceIDs.begin(), sequenceIDs.end());

            ImGui::Text("Dialogue Sequences");
            ImGui::SameLine();

            // Find the current index in the combo
            int currentIndex = -1;
            if (!m_selectedSequenceID.empty()) {
                for (int i = 0; i < static_cast<int>(sequenceIDs.size()); ++i) {
                    if (sequenceIDs[i] == m_selectedSequenceID) {
                        currentIndex = i;
                        break;
                    }
                }
            }

            // If the current selection is no longer valid (eg after a delete)
            // fall back to the first valid sequence
            if (currentIndex < 0 && !sequenceIDs.empty()) {
                currentIndex = 0;
                m_selectedSequenceID = sequenceIDs[0];
                m_selectedLineIndex = -1;
            }

            // Sequence dropdown combo box
            ImGui::SetNextItemWidth(200.f);
            if (!sequenceIDs.empty()) {
                if (ImGui::BeginCombo("##SequenceCombo",
                    currentIndex >= 0 ? sequenceIDs[currentIndex].c_str() : "")) {
                    for (int i = 0; i < static_cast<int>(sequenceIDs.size()); ++i) {
                        bool selected = (i == currentIndex);
                        if (ImGui::Selectable(sequenceIDs[i].c_str(), selected)) {
                            // When use selects a different sequence, reset line selection
                            if (m_selectedSequenceID != sequenceIDs[i]) {
                                m_selectedSequenceID = sequenceIDs[i];
                                m_selectedLineIndex = -1;
                                m_lineEditorDirty = false;
                            }
                        }
                        if (selected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            } else {
                ImGui::TextDisabled("No sequences loaded");
            }

            // Button to open the modal to create a new dialogue sequence
            ImGui::SameLine();
            if (ImGui::Button("New")) {
                m_openNewSequenceModal = true;
                std::memset(m_newSequenceNameBuffer, 0, sizeof(m_newSequenceNameBuffer));
            }

            // Button to delete the currently selected sequence from disk
            ImGui::SameLine();
            if (ImGui::Button("Delete") && !m_selectedSequenceID.empty()) {
                dialogueSystem->DeleteSequence(m_selectedSequenceID);
                m_selectedSequenceID = "";
                m_selectedLineIndex = -1;
                m_lineEditorDirty = false;
            }

            // Rename row
            if (!m_selectedSequenceID.empty()) {
                static char renameBuffer[128] = {};
                static std::string lastRenameID = "";

                // Update the rename buffer whenever the selected sequence changes
                if (lastRenameID != m_selectedSequenceID) {
                    std::snprintf(renameBuffer, sizeof(renameBuffer),
                        "%s", m_selectedSequenceID.c_str());
                    lastRenameID = m_selectedSequenceID;
                }

                ImGui::Text("Sequence ID");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(200.f);
                ImGui::InputText("##RenameSequence", renameBuffer, sizeof(renameBuffer));
                ImGui::SameLine();

                // Rename button
                // Renames in the disk too
                if (ImGui::Button("Rename")) {
                    std::string newID = renameBuffer;
                    // Only rename if the new name is not empty and is different
                    if (!newID.empty() && newID != m_selectedSequenceID) {
                        if (dialogueSystem->RenameSequence(m_selectedSequenceID, newID)) {
                            // Update the currently selected sequence too to the new ID
                            m_selectedSequenceID = newID;
                            lastRenameID = newID;
                        }
                    }
                }
            }
		}

		void DrawLineList()
		{
            auto dialogueSystem = m_coord.GetSystem<DialogueSystem>();
            auto& library = dialogueSystem->GetDialogueLibrary();

            auto seqIt = library.find(m_selectedSequenceID);
            if (seqIt == library.end()) {
                return;
            }

            DialogueSystem::DialogueSequence& seq = seqIt->second;

            ImGui::Text("Lines");

            // Scrollable child region containing the list of dialogue lines
            ImGui::BeginChild("LineList", ImVec2(0, 180), true);

            for (int i = 0; i < static_cast<int>(seq.lines.size()); ++i) {
                auto& [lineID, line] = seq.lines[i];

                // Show: [lineID]  Speaker: "text preview..."
                // Show the first 30 chars of the text only
                std::string preview = "[" + lineID + "]  "
                    + line.speaker + ":  \""
                    + (line.text.size() > 30 ? line.text.substr(0, 30) + "..." : line.text)
                    + "\"";

                bool selected = (m_selectedLineIndex == i);

                // PushID ensures each Selectable has a unique ImGui ID
                // even if the text is the same
                ImGui::PushID(i);

                if (ImGui::Selectable(preview.c_str(), selected)) {
                    // Only update state if a different line was selected
                    if (m_selectedLineIndex != i) {
                        m_selectedLineIndex = i;
                        m_lineEditorDirty = false;

                        // Populate edit buffers from selected line
                        // These buffers are read and displayed by DrawLineProperties()
                        std::snprintf(m_lineIDBuffer, sizeof(m_lineIDBuffer), "%s", lineID.c_str());
                        std::snprintf(m_speakerBuffer, sizeof(m_speakerBuffer), "%s", line.speaker.c_str());
                        std::snprintf(m_materialBuffer, sizeof(m_materialBuffer), "%s", line.speakerMaterial.c_str());
                        std::snprintf(m_textBuffer, sizeof(m_textBuffer), "%s", line.text.c_str());
                        m_charDelay = line.charDelay;

                        // Find nextID index in the line list for the dropdown
                        // Must skip its own ID to stay consistent with how nextIDOptions is built
                        m_nextIDComboIndex = 0; // 0 is default to (End of sequence)
                        int comboOffset = 1; // start at 1 because index 0 is (End of sequence)
                        for (int j = 0; j < static_cast<int>(seq.lines.size()); ++j) {
                            // Skip its own ID to prevent having infinite loop when pointing to itself
                            if (seq.lines[j].first == lineID) {
                                continue;
                            }
                            if (seq.lines[j].first == line.nextID) {
                                m_nextIDComboIndex = comboOffset;
                                break;
                            }
                            ++comboOffset; // only increment when we dont skip its own ID
                        }
                    }
                }

                ImGui::PopID();
            }

            ImGui::EndChild();

            // Add a new line with default placeholder values
            if (ImGui::Button("Add Line")) {
                DialogueSystem::DialogueLine newLine;
                // Generate a unique default ID
                newLine.ID = "line_" + std::to_string(seq.lines.size());
                newLine.speaker = "Speaker";
                newLine.speakerMaterial = "";
                newLine.text = "New dialogue line.";
                newLine.nextID = "";
                newLine.charDelay = 0.05f;

                dialogueSystem->AddLine(m_selectedSequenceID, newLine);

                // Auto-select the new line and populate the edit buffers immediately
                m_selectedLineIndex = static_cast<int>(seq.lines.size()) - 1;
                m_lineEditorDirty = false;

                std::snprintf(m_lineIDBuffer, sizeof(m_lineIDBuffer), "%s", newLine.ID.c_str());
                std::snprintf(m_speakerBuffer, sizeof(m_speakerBuffer), "%s", newLine.speaker.c_str());
                std::snprintf(m_materialBuffer, sizeof(m_materialBuffer), "%s", newLine.speakerMaterial.c_str());
                std::snprintf(m_textBuffer, sizeof(m_textBuffer), "%s", newLine.text.c_str());
                m_charDelay = newLine.charDelay;
                m_nextIDComboIndex = 0; // new lines always default to (End of sequence)
            }

            ImGui::SameLine();

            // Delete the selected line and save to disk immediately
            if (ImGui::Button("Delete Line") && m_selectedLineIndex >= 0) {
                auto& [lineID, _] = seq.lines[m_selectedLineIndex];
                dialogueSystem->DeleteLine(m_selectedSequenceID, lineID);
                dialogueSystem->SaveSequence(m_selectedSequenceID);
                m_selectedLineIndex = -1;
                m_lineEditorDirty = false;
            }
		}

		void DrawLineProperties()
		{
            // No line currently selected
            if (m_selectedLineIndex < 0) {
                ImGui::TextDisabled("Select a line to edit its properties.");
                return;
            }

            auto dialogueSystem = m_coord.GetSystem<DialogueSystem>();
            auto& library = dialogueSystem->GetDialogueLibrary();

            auto seqIt = library.find(m_selectedSequenceID);
            if (seqIt == library.end()) {
                return;
            }

            DialogueSystem::DialogueSequence& seq = seqIt->second;

            // In case of stale index after a line deletion
            if (m_selectedLineIndex >= static_cast<int>(seq.lines.size())) {
                m_selectedLineIndex = -1;
                return;
            }

            ImGui::Text("Line Properties");

            float labelWidth = 130.f;

            // Line ID
            ImGui::Text("Line ID");
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(220.f);
            if (ImGui::InputText("##LineID", m_lineIDBuffer, sizeof(m_lineIDBuffer))) {
                m_lineEditorDirty = true;
            }

            // Speaker name
            ImGui::Text("Speaker");
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(220.f);
            if (ImGui::InputText("##Speaker", m_speakerBuffer, sizeof(m_speakerBuffer))) {
                m_lineEditorDirty = true;
            }

            // Speaker Material - dropdown of all loaded materials from graphics system
            ImGui::Text("Material");
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(220.f);
            const std::vector<std::string>& materials = m_coord.GetSystem<GraphicsSystem>()->GetMaterialMapStrings();

            // Find the current selection index
            // Index 0 is always (none), real materials will start at index 1
            int materialIndex = 0; // default to (none)
            for (int i = 0; i < static_cast<int>(materials.size()); ++i) {
                if (materials[i] == std::string(m_materialBuffer)) {
                    materialIndex = i + 1; // +1 to account for (none) entry
                    break;
                }
            }

            // Display label: (none) if it is index 0, or the real material name
            const char* previewLabel = (materialIndex == 0) ? "(None)" : materials[materialIndex - 1].c_str();
            if (ImGui::BeginCombo("##Material", previewLabel)) {
                // (none) option
                if (ImGui::Selectable("(none)", materialIndex == 0)) {
                    std::memset(m_materialBuffer, 0, sizeof(m_materialBuffer)); // clear to empty string
                    m_lineEditorDirty = true;
                }
                if (materialIndex == 0) {
                    ImGui::SetItemDefaultFocus();
                }

                // Real materials
                for (int i = 0; i < static_cast<int>(materials.size()); ++i) {
                    bool selected = (materialIndex == i + 1);
                    if (ImGui::Selectable(materials[i].c_str(), selected)) {
                        std::snprintf(m_materialBuffer, sizeof(m_materialBuffer),
                            "%s", materials[i].c_str());
                        m_lineEditorDirty = true;
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            // Char Delay
            ImGui::Text("Char Delay");
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(220.f);
            if (ImGui::SliderFloat("##CharDelay", &m_charDelay, 0.01f, 0.5f, "%.3f")) {
                m_lineEditorDirty = true;
            }

            // Next Line ID — dropdown of all line IDs in this sequence
            ImGui::Text("Next Line");
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(220.f);

            // Build the options list: "(End of sequence)" + all line IDs
            // This options list is to be used for the selected line's nextID
            std::vector<const char*> nextIDOptions;
            nextIDOptions.push_back("(End of sequence)");
            for (auto const& [ID, _] : seq.lines) {
                // Skip its own ID
                if (ID == seq.lines[m_selectedLineIndex].first) {
                    continue;
                }
                nextIDOptions.push_back(ID.c_str());
            }

            if (ImGui::BeginCombo("##NextID", nextIDOptions[m_nextIDComboIndex])) {
                for (int i = 0; i < static_cast<int>(nextIDOptions.size()); ++i) {
                    bool selected = (m_nextIDComboIndex == i);
                    if (ImGui::Selectable(nextIDOptions[i], selected)) {
                        m_nextIDComboIndex = i;
                        m_lineEditorDirty = true;
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            // Text - supports multiline using \n for intentional new lines 
            ImGui::Text("Text");
            ImGui::SetNextItemWidth(-1.f); // full width
            if (ImGui::InputTextMultiline("##LineText", m_textBuffer, sizeof(m_textBuffer),
                ImVec2(-1.f, 80.f)))
                m_lineEditorDirty = true;

            ImGui::Spacing();

            // Dirty indicator
            if (m_lineEditorDirty)
                ImGui::TextColored(ImVec4(1.f, 0.8f, 0.f, 1.f), "* Unsaved changes");

            // Apply button — commits edits to the in-memory sequence
            if (ImGui::Button("Apply")) {
                DialogueSystem::DialogueLine updated;
                updated.ID = m_lineIDBuffer;
                updated.speaker = m_speakerBuffer;
                updated.speakerMaterial = m_materialBuffer;
                updated.text = m_textBuffer;
                updated.charDelay = m_charDelay;
                updated.nextID = (m_nextIDComboIndex == 0) ? "" : nextIDOptions[m_nextIDComboIndex];

                // If the line ID changed, old entry gets replaced under new key
                std::string oldLineID = seq.lines[m_selectedLineIndex].first;
                dialogueSystem->UpdateLine(m_selectedSequenceID, oldLineID, updated);
                m_lineEditorDirty = false;
            }

            ImGui::SameLine();

            // Save button — save the whole sequence to disk
            if (ImGui::Button("Save Sequence")) {
                DialogueSystem::DialogueLine updated;
                updated.ID = m_lineIDBuffer;
                updated.speaker = m_speakerBuffer;
                updated.speakerMaterial = m_materialBuffer;
                updated.text = m_textBuffer;
                updated.charDelay = m_charDelay;
                updated.nextID = (m_nextIDComboIndex == 0) ? "" : nextIDOptions[m_nextIDComboIndex];

                std::string oldLineID = seq.lines[m_selectedLineIndex].first;
                dialogueSystem->UpdateLine(m_selectedSequenceID, oldLineID, updated);
                dialogueSystem->SaveSequence(m_selectedSequenceID);
                m_lineEditorDirty = false;
            }
		}

		void DrawNewSequenceModal()
		{
            // If user pressed 'New' button to create a new dialogue sequence
            if (m_openNewSequenceModal) {
                ImGui::OpenPopup("New Dialogue Sequence");
                m_openNewSequenceModal = false;
            }

            // Show Popup Modal only when user wants tp create a new dialogue sequence
            if (ImGui::BeginPopupModal("New Dialogue Sequence", nullptr,
                ImGuiWindowFlags_AlwaysAutoResize)) {

                ImGui::Text("Sequence ID (this becomes the filename):");
                ImGui::SetNextItemWidth(300.f);
                ImGui::InputText("##NewSeqName", m_newSequenceNameBuffer, sizeof(m_newSequenceNameBuffer));

                ImGui::Spacing();

                // Create button
                if (ImGui::Button("Create", ImVec2(120, 0))) {
                    std::string newID = m_newSequenceNameBuffer;
                    if (!newID.empty()) {
                        auto dialogueSys = m_coord.GetSystem<DialogueSystem>();
                        // Successfully created a new sequence
                        if (dialogueSys->CreateSequence(newID)) {
                            m_selectedSequenceID = newID;
                            m_selectedLineIndex = -1;
                        }
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::SameLine();

                // Cancel button
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }
		}

		Coordinator& m_coord;
		std::shared_ptr<Entity> m_selectedEntity;

		// Editor state
		std::string m_selectedSequenceID = "";
		int         m_selectedLineIndex = -1;

		// Buffers for the "new sequence" modal
		char        m_newSequenceNameBuffer[128] = {};
		bool        m_openNewSequenceModal = false;

		// Working copy of the line being edited
		// We edit a local copy and only commit on Save
		char        m_lineIDBuffer[64] = {};
		char        m_speakerBuffer[64] = {};
		char        m_materialBuffer[64] = {};
		char        m_textBuffer[1024] = {};
		float       m_charDelay = 0.05f;
		int         m_nextIDComboIndex = 0;

		bool        m_lineEditorDirty = false; // tracks unsaved changes
	};
}