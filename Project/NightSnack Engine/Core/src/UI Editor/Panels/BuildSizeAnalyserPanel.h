/******************************************************************************
 * File:        BuildSizeAnalyserPanel.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created BuildSizeAnalyser Panel
 *
 * Description:
 * BuildSizeAnalyser Panel functionalities include:
 * - Displaying all files inside the assets folder.
 * - For every asset item listed, its name, asset type, size and status of being loaded in the program, will be displayed. 
 * - Assets that are already loaded in the program will be selected by default.
 * - Export selections are persistent when user close export panel and open it again.
 * - User is able to select/deselect all assets to export out.
 * - User is able to see the total size of all selected assets that will be exported out. 
 * - User can browse his local window file explorer to choose export destination folder.
 * - Exported Assets can be exported while preserving their internal folder structure too. 
 * - After exporting, popup panel will display how many files are successfully exported or failed to export. 
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "AllPanels.h"
#include "WindowsFolderDialog.h"

#ifdef _WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>
#endif

#include <filesystem>
#include <algorithm>
#include <unordered_set>

namespace fs = std::filesystem;

namespace NightSnack 
{
	struct BuildAssetEntry {
		std::string displayName;	// Filename for display
		std::string fullPath;		// Absolute path on disk
		std::string relativePath;	// Relative path from assets root (for export structure)
		RESOURCE_TYPE resourceType; // Type of resource
		std::size_t mapKey;			// Key in ResourceManager map
		uintmax_t fileSize;			// File size in bytes
		bool isSelected;			// Whether this asset is selected for export
		bool isLoadedInEngine;		// Whether this asset is currently loaded in ResourceManager
	};

	class BuildSizeAnalyserPanel {

	public:

		/**
		 * @brief Constructor
		 * @param coord Reference to the ECS coordinator
		 */
		BuildSizeAnalyserPanel(Coordinator& coord) :
			m_coord(coord),
			m_assetsRoot(RESOURCE_PATH),
			m_needsRefresh(true),
			m_exportInProgress(false),
			m_showExportResult(false),
			m_exportSuccessCount(0),
			m_exportFailCount(0),
			m_exportPathErrorMessage("")
		{
			memset(m_exportPath, 0, sizeof(m_exportPath));

			// Default Export Destination Folder
			strcpy_s(m_exportPath, sizeof(m_exportPath), "C:\\ExportedAssets");
		}

		/**
		 * @brief Main draw function
		 * @param isOpen - pointer to the visibility bool
		 */
		void Draw(bool* isOpen) {
			if (!*isOpen) {
				return;
			}

			ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);

			if (!ImGui::Begin("Export Assets", isOpen)) {
				ImGui::End();
				return;
			}

			// Refresh the assets list on first open or when requested
			if (m_needsRefresh) {
				RefreshAssetList();
				m_needsRefresh = false;
			}

			DrawToolbar();
			ImGui::Separator();
			DrawAssetTable();
			ImGui::Separator();
			DrawFooter();

			// Export result popup
			if (m_showExportResult) {
				ImGui::OpenPopup("Export Result");
				m_showExportResult = false;
			}
			DrawExportResultPopup();

			ImGui::End();
		}

		/**
		 * @brief Request a refresh of the assets list
		 */
		void RequestRefresh() {
			m_needsRefresh = true;
		}

	private:

		Coordinator& m_coord;
		fs::path m_assetsRoot;

		// Asset data
		std::vector<BuildAssetEntry> m_assets = {};
		bool m_needsRefresh{ true };

		// Selection persistence: store map keys of previously selected assets
		// Only persists within the same program session
		std::unordered_set<std::size_t> m_persistedSelections = {};
		bool m_hasPersistedSelections = false;  // True after first refresh, used to restore selections

		// Export state
		char m_exportPath[512] = {};
		bool m_exportInProgress{ false };
		bool m_showExportResult{ false };
		int m_exportSuccessCount{};
		int m_exportFailCount{};
		std::string m_exportPathErrorMessage{};


		/**
		 * @brief Computes a map key from a file path string, matching
		 *        ResourceManager::ConstructMapKey logic (std::hash)
		 */
		std::size_t ComputeMapKey(const std::string& filePath)
		{
			return std::hash<std::string>{}(filePath);
		}

		/**
		 * @brief Determines RESOURCE_TYPE from a file extension
		 */
		RESOURCE_TYPE DetermineResourceType(const std::string& ext)
		{
			std::string lower = ext;
			std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

			if (lower == ".png" || lower == ".jpg" || lower == ".jpeg" || lower == ".bmp") {
				return RESOURCE_IMAGE;
			} else if (lower == ".txt" || lower == ".cs") {
				return RESOURCE_TXTFILE;
			} else if (lower == ".json") {
				return RESOURCE_JSON;
			} else if (lower == ".wav" || lower == ".mp3") {
				return RESOURCE_SOUND;
			} else if (lower == ".ttf" || lower == ".otf") {
				return RESOURCE_FONT;
			}

			return RESOURCE_NONE;
		}

		/**
		 * @brief Rebuilds the assets list by scanning ALL files in the assets folder.
		 *		  Loaded assets from ResourceManager are auto selected by default. 
		 */
		void RefreshAssetList() {
			// Save current selections before refreshing first
			SaveSelections();

			m_assets.clear();

			auto rm = m_coord.GetSystem<ResourceManager>();
			
			// Build a set of loaded resource keys for quick lookup
			std::unordered_set<std::size_t> loadedKeys;
			// Also map key to type for loaded resources
			std::unordered_map<std::size_t, RESOURCE_TYPE> loadedTypeMap;

			if (rm) {
				// Get all currently loaded resources from ResourceManager
				auto loadedResources = rm->GetAllLoadedResources();

				for (auto& [path, type, key] : loadedResources) {
					loadedKeys.insert(key);
					loadedTypeMap[key] = type;
				}
			}

			// Canonical assets root for computing relative paths
			fs::path canonicalRoot;
			try {
				canonicalRoot = fs::canonical(m_assetsRoot);
			}
			catch (...) {
				canonicalRoot = m_assetsRoot;
			}

			// Scan all files in the assets directory tree
			try
			{
				for (const auto& dirEntry : fs::recursive_directory_iterator(canonicalRoot, fs::directory_options::skip_permission_denied)) {
					if (!dirEntry.is_regular_file()) {
						continue;
					}

					fs::path filePath = dirEntry.path();

					BuildAssetEntry entry;

					// Canonical path for consistent key computation
					fs::path canonicalFile;
					try {
						canonicalFile = fs::canonical(filePath);
					}
					catch (...) {
						canonicalFile = filePath;
					}

					entry.fullPath = canonicalFile.string();
					entry.displayName = filePath.filename().string();

					// Compute relative path from assets root
					try {
						entry.relativePath = fs::relative(canonicalFile, canonicalRoot).string();
					}
					catch (...) {
						entry.relativePath = filePath.filename().string();
					}

					// Compute map key (same hash logic as ResourceManager::ConstructMapKey)
					entry.mapKey = ComputeMapKey(canonicalFile.string());

					// Check if this file is loaded in the ResourceManager
					bool isLoaded = (loadedKeys.find(entry.mapKey) != loadedKeys.end());
					entry.isLoadedInEngine = isLoaded;

					// Assign the resource type accordingly
					if (isLoaded && loadedTypeMap.find(entry.mapKey) != loadedTypeMap.end()) {
						entry.resourceType = loadedTypeMap[entry.mapKey];
					} else {
						entry.resourceType = DetermineResourceType(filePath.extension().string());
					}

					// Get file size
					try {
						entry.fileSize = dirEntry.file_size();
					}
					catch (...) {
						entry.fileSize = 0;
					}

					// Restore persisted selections if there are
					// If first time, auto select loaded assets only
					if (m_hasPersistedSelections) {
						entry.isSelected = (m_persistedSelections.find(entry.mapKey) != m_persistedSelections.end());
					} else {
						entry.isSelected = isLoaded;
					}

					m_assets.push_back(entry);
				}
			}
			catch (const std::exception& e) {
				std::cerr << "[BuildSizeAnalyser] Error scanning assets folder: " << e.what() << std::endl;
			}

			// Sort - loaded assets first, then by type, then by name
			std::sort(m_assets.begin(), m_assets.end(),
				[](const BuildAssetEntry& a, const BuildAssetEntry& b)
				{
					// Loaded assets come first
					if (a.isLoadedInEngine != b.isLoadedInEngine) {
						return a.isLoadedInEngine > b.isLoadedInEngine;
					}

					if (a.resourceType != b.resourceType) {
						return a.resourceType < b.resourceType;
					}

					return a.displayName < b.displayName;
				});
		}

		/**
		 * @brief Saves current selection state for persistence
		 */
		void SaveSelections() {
			m_persistedSelections.clear();

			for (auto& asset : m_assets) {
				if (asset.isSelected) {
					m_persistedSelections.insert(asset.mapKey);
				}
			}

			if (!m_assets.empty()) {
				m_hasPersistedSelections = true;
			}
		}
		
		/**
		 * @brief Draws the toolbar
		 */
		void DrawToolbar() {

			if (ImGui::Button("Select All")) {
				for (auto& asset : m_assets) {
					asset.isSelected = true;
				}
			}
			ImGui::SameLine();

			if (ImGui::Button("Deselect All")) {
				for (auto& asset : m_assets) {
					asset.isSelected = false;
				}
			}
			ImGui::SameLine();

			// Push some spacing before Refresh
			ImGui::SameLine(ImGui::GetWindowWidth() - 80);
			if (ImGui::Button("Refresh")) {
				m_needsRefresh = true;
			}

			// Show asset count
			int selectedCount = 0;
			int loadedCount = 0;
			for (auto& asset : m_assets) {
				if (asset.isSelected) {
					++selectedCount;
				}
				if (asset.isLoadedInEngine) {
					++loadedCount;
				}
			}
			ImGui::Text("%d / %zu assets selected  |  %d loaded in engine", selectedCount, m_assets.size(), loadedCount);
		}

		/**
		 * @brief Draws the main asset table with checkboxes
		 */
		void DrawAssetTable() {
			// Reserve space at the bottom for the footer
			float footerHeight = 150.f;
			float availableHeight = ImGui::GetContentRegionAvail().y - footerHeight;
			if (availableHeight < footerHeight) {
				availableHeight = footerHeight;
			}

			ImGui::BeginChild("AssetTableRegion", ImVec2(0, availableHeight), true);

			ImGuiTableFlags tableFlags =
				ImGuiTableFlags_Borders |
				ImGuiTableFlags_RowBg |
				ImGuiTableFlags_Resizable |
				ImGuiTableFlags_ScrollY |
				ImGuiTableFlags_SizingStretchProp;

			if (ImGui::BeginTable("AssetsTable", 5, tableFlags)) {
				// Column headers
				ImGui::TableSetupColumn("##Select", ImGuiTableColumnFlags_WidthFixed, 30.0f);
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
				ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 70.f);
				ImGui::TableHeadersRow();

				for (size_t i{ 0 }; i < m_assets.size(); ++i) {
					auto& asset = m_assets[i];

					ImGui::TableNextRow();
					ImGui::PushID(static_cast<int>(i));

					// Dim unloaded assets slightly 
					if (!asset.isLoadedInEngine) {
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.6f);
					}

					// Column 0: Checkbox
					ImGui::TableSetColumnIndex(0);
					ImGui::Checkbox("##sel", &asset.isSelected);

					// Column 1: Name (with tooltip showing the full path)
					ImGui::TableSetColumnIndex(1);
					ImGui::TextUnformatted(asset.displayName.c_str());
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("Path: %s", asset.fullPath.c_str());
						ImGui::Text("Relative: %s", asset.relativePath.c_str());
						ImGui::EndTooltip();
					}

					// Column 2: Type
					ImGui::TableSetColumnIndex(2);
					ImGui::TextUnformatted(ResourceTypeToString(asset.resourceType));

					// Column 3: Size
					ImGui::TableSetColumnIndex(3);
					ImGui::Text("%s", FormatFileSize(asset.fileSize).c_str());

					// Column 4: Status (loaded or not in the ResourceManager)
					ImGui::TableSetColumnIndex(4);
					if (asset.isLoadedInEngine) {
						ImGui::TextColored(ImVec4(0.4f, 1.f, 0.4f, 1.f), "Loaded");
					} else {
						ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Unused");
						ImGui::PopStyleVar();
					}

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::EndChild();
		}

		/**
		 * @brief Draws the footer with total size and export controls
		 */
		void DrawFooter() {
			// Calculate total selected size
			uintmax_t totalSize = 0;
			int selectedCount = 0;
			for (auto& asset : m_assets) {
				if (asset.isSelected) {
					totalSize += asset.fileSize;
					++selectedCount;
				}
			}

			// Total size display
			ImGui::Text("Total Export Size: %s (%d files)", FormatFileSize(totalSize).c_str(), selectedCount);

			ImGui::Separator();

			// Export path input and Export button
			ImGui::Text("Export to:");
			ImGui::SameLine();

			// Buttons widths
			float browseButtonWidth = 75.0f;
			float exportButtonWidth = 70.0f;
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			float buttonsWidth = browseButtonWidth + exportButtonWidth + spacing * 2;

			// Export path is read-only, user has to press Browse button to select Export Destination folder
			// Export path is greyed-out slightly
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonsWidth);
			ImGui::InputText("##ExportPath", m_exportPath, sizeof(m_exportPath), ImGuiInputTextFlags_ReadOnly);
			ImGui::PopStyleColor();
			ImGui::SameLine();

			// Browse button - opens native folder picker
			if (ImGui::Button("Browse", ImVec2(browseButtonWidth, 0))) {
				// Get the native Win32 window handle from Application System
				GLFWwindow* window = m_coord.GetSystem<ApplicationSystem>()->GetNativeHandle();
				void* nativeHandle = nullptr;
#ifdef _WIN32
				nativeHandle = static_cast<void*>(glfwGetWin32Window(window));
#endif
				// Pass in the native handle so that the user cannot click anywhere else in the level editor 
				// unless they close the windows file explorer first
				std::string selectedPath;
				if (NightSnack::OpenFolderDialog(std::string(m_exportPath), selectedPath, nativeHandle)) {
					// Append "ExportedAssets" folder to the chosen directory
					// Just for a slightly better quality of life for the user
					fs::path exportDir = fs::path(selectedPath) / "ExportedAssets";
					std::string finalPath = exportDir.string();

					strncpy_s(m_exportPath, finalPath.c_str(), sizeof(m_exportPath) - 1);
					m_exportPath[sizeof(m_exportPath) - 1] = '\0';
				}
			}

			ImGui::SameLine();

			// Export button
			bool canExport = (selectedCount > 0 && strlen(m_exportPath) > 0);
			if (!canExport) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Export", ImVec2(exportButtonWidth, 0))) {
				ExportSelectedAssets();
			}

			if (!canExport) {
				ImGui::EndDisabled();
			}

			if (!canExport && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
				ImGui::BeginTooltip();
				if (selectedCount == 0) {
					ImGui::Text("No assets selected");
				} else {
					ImGui::Text("Please enter an export path");
				}
				ImGui::EndTooltip();
			}
		}

		/**
		 * @brief Draws the export result popup
		 */
		void DrawExportResultPopup() {
			// Popup will be at the center of the screen
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // only when popup first opens

			if (ImGui::BeginPopupModal("Export Result", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				if (m_exportFailCount == 0) {
					ImGui::TextColored(ImVec4(0.4f, 1.f, 0.4f, 1.f), "Export completed successfully!");
				} else {
					ImGui::TextColored(ImVec4(1.f, 1.f, 0.4f, 1.f), "Export partially completed.");
				}

				ImGui::Separator();
				ImGui::Text("Exported: %d files", m_exportSuccessCount);
				if (m_exportFailCount > 0) {
					ImGui::TextColored(ImVec4(1.f, 0.4f, 0.4f, 1.f), "Failed: %d files", m_exportFailCount);
				}

				ImGui::Text("Destination: %s", m_exportPath);
				ImGui::Separator();

				float buttonWidth = 80.0f;
				float windowWidth = ImGui::GetWindowSize().x;
				ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
				if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		/**
		 * @brief Exports all selected assets to the chosen destination folder,
		 *        preserving relative folder structure
		 */
		void ExportSelectedAssets() {

			m_exportSuccessCount = 0;
			m_exportFailCount = 0;

			fs::path destRoot(m_exportPath);

			// Create destination root if it does not exist yet
			try {
				if (!fs::exists(destRoot)) {
					fs::create_directories(destRoot);
				}
			}
			catch (const std::exception& e) {
				std::cerr << "[BuildSizeAnalyser] Failed to create export directory: " << e.what() << std::endl;
				m_exportPathErrorMessage = std::string("Failed to create directory: ") + e.what();
				return;
			}

			for (auto& asset : m_assets) {
				if (!asset.isSelected) {
					continue;
				}

				try {
					fs::path sourcePath(asset.fullPath);

					// Source file path does not exist
					if (!fs::exists(sourcePath)) {
						std::cerr << "[BuildSizeAnalyser] Source file not found: " << asset.fullPath << std::endl;
						++m_exportFailCount;
						continue;
					}

					// Build destination path preserving relative structure
					fs::path destPath = destRoot / fs::path(asset.relativePath);

					// Create parent directories if needed first
					fs::path destDir = destPath.parent_path();
					if (!fs::exists(destDir)) {
						fs::create_directories(destDir);
					}

					// Copy the file (overwrite if it exists already)
					fs::copy_file(sourcePath, destPath, fs::copy_options::overwrite_existing);
					++m_exportSuccessCount;
				}
				catch (const std::exception& e) {
					std::cerr << "[BuildSizeAnalyser] Failed to export " << asset.displayName << ": " << e.what() << std::endl;
					++m_exportFailCount;
				}
			}

			m_showExportResult = true;
		}

		/**
		 * @brief Helper function to convert a RESOURCE_TYPE enum to a display string
		 */
		const char* ResourceTypeToString(RESOURCE_TYPE type) {
			switch (type) {
			case RESOURCE_IMAGE:	return "Image";
			case RESOURCE_TXTFILE:	return "Text";
			case RESOURCE_JSON:		return "JSON";
			case RESOURCE_SOUND:	return "Sound";
			case RESOURCE_FONT:		return "Font";
			case RESOURCE_NONE:		return "Other";
			default:				return "Unknown";
			}
		}

		/**
		 * @brief Helper function to format a file size in bytes to a string
		 */
		std::string FormatFileSize(uintmax_t bytes) {
			char buffer[64];

			if (bytes < 1024) { // Bytes
				snprintf(buffer, sizeof(buffer), "%llu B", static_cast<unsigned long long>(bytes));

			} else if (bytes < 1024 * 1024) { // KB
				snprintf(buffer, sizeof(buffer), "%.2f KB", static_cast<double>(bytes) / 1024.0);

			} else if (bytes < 1024ULL * 10024 * 1024) { // MB
				snprintf(buffer, sizeof(buffer), "%.2f MB", static_cast<double>(bytes) / (1024.0 * 1024.0));

			} else { // GB
				snprintf(buffer, sizeof(buffer), "%.2f GB", static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0));
			}

			return std::string(buffer);
		}
	};

}