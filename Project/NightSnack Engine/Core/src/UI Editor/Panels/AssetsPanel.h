/******************************************************************************
 * File:        AssetsPanel.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Unified assets panel
 *
 * Description:
 * Unified assets panel displaying all assets in a hierarchical folder structure.
 * Integrates Images, Files, Scripts, and Prefabs in one view.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "AllPanels.h"
#include "Graphics/Texture.h"
#include "Systems/AnimationSystem.h"
#include "Factory/EntityFactory.h"
#include "PrefabsPanel.h"
#include "FilesPanel.h"
#include "ScriptsPanel.h"
#include <stb_image.h>

namespace fs = std::filesystem;

namespace NightSnack
{
	enum class AssetType
	{
		Folder,
		Image,
		Script,
		Prefab,
		Audio,
		Other
	};

	struct AssetItem
	{
		std::string name;
		fs::path fullPath;
		AssetType type{ AssetType::Other };
		bool isDirectory{};
	};

	/**
	* @brief assets panel for managing all game assets
	*/
	class AssetsPanel {
	public:
		/**
		* @brief constructor
		* @param coord reference to the ECS coordinator
		*/
		AssetsPanel(Coordinator& coord) :
			m_coord(coord),
			m_assetsRoot(RESOURCE_PATH),
			m_scriptsRoot(SCRIPTS_PATH),
			m_currentDirectory(m_assetsRoot),
			m_thumbnailSize(96.0f),
			m_selectedAssetPath(""),
			m_showAssetProperties(false),
			m_showImportDialog(false),
			m_prefabsPanel(coord, RESOURCE_PATH + std::string{ "GameData/Prefabs/" }),
			m_filesPanel(coord, RESOURCE_PATH),
			m_scriptsPanel(coord, SCRIPTS_PATH)
		{
			m_prefabNames.reserve(coord.GetSystem<EntityFactory>()->GetPrefabNames().size());
			for (std::string const& s : coord.GetSystem<EntityFactory>()->GetPrefabNames())
			{
				m_prefabNames.emplace_back(s);
			}
			m_prefabMats.reserve(coord.GetSystem<EntityFactory>()->GetPrefabMaterials().size());
			for (auto const& [n, m] : coord.GetSystem<EntityFactory>()->GetPrefabMaterials())
			{
				m_prefabMats.insert({ n, m });
			}
		}

		/**
		* @brief draw the assets panel
		*/
		void DrawAssetsPanel() {
			DrawToolbar();
			ImGui::Separator();

			float totalWidth = ImGui::GetContentRegionAvail().x;

			// two-column layout
			ImGui::Columns(2, "AssetColumns", true);

			ImGui::SetColumnWidth(0, totalWidth * 0.3f);

			// Left column
			ImGui::BeginChild("FolderTree", ImVec2(0, 0), true);
			ImGui::Text("Project");
			ImGui::Separator();
			m_assetsRoot = std::filesystem::canonical(m_assetsRoot);
			DrawFolderTree(m_assetsRoot);

			// add scripts folder if it exists
			m_scriptsRoot = std::filesystem::canonical(m_scriptsRoot);
			if (fs::exists(m_scriptsRoot))
			{
				DrawFolderTree(m_scriptsRoot);
			}
			ImGui::EndChild();

			ImGui::NextColumn();

			// right column
			ImGui::BeginChild("ContentView", ImVec2(0, 0), true);
			DrawContentView();
			ImGui::EndChild();

			ImGui::Columns(1);
		}

		/**
		* @brief draw the assets and animation panel
		*/
		void Draw()
		{
			static bool firstFrame = true;
			if (firstFrame)
			{
				auto appSystem = m_coord.GetSystem<ApplicationSystem>();
				if (appSystem) appSystem->ClearDroppedFiles();
				firstFrame = false;
			} else
			{
				HandleExternalFileDrop();
			}

			ImGui::Begin("Assets");
			DrawAssetsPanel();
			ImGui::End();

			// preview/Properties panel
			if (m_showAssetProperties && !m_selectedAssetPath.empty())
			{
				DrawAssetProperties();
			}

			if (m_showImportDialog)
			{
				DrawImportDialog();
			}

		}

	private:
		Coordinator& m_coord;
		fs::path m_assetsRoot;
		fs::path m_scriptsRoot;
		fs::path m_currentDirectory;

		std::unordered_map<std::string, GLuint> m_loadedTextures;
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureObjects;

		float m_thumbnailSize;
		std::string m_searchFilter;
		std::string m_selectedAssetPath;
		AssetItem m_selectedAsset;
		bool m_showAssetProperties;

		bool m_showImportDialog;
		std::vector<std::string> m_pendingImports;
		std::string m_importDestination;

		std::vector<std::string> m_prefabNames;
		std::unordered_map<std::string, std::string> m_prefabMats;

		PrefabsPanel m_prefabsPanel;
		FilesPanel m_filesPanel;
		ScriptsPanel m_scriptsPanel;

		/**
		* @brief draws the toolbar with navigation and controls
		*/
		void DrawToolbar()
		{
			// back button
			if (m_currentDirectory != m_assetsRoot && m_currentDirectory != m_scriptsRoot)
			{
				if (ImGui::Button("<- Back"))
				{
					m_currentDirectory = m_currentDirectory.parent_path();
				}
				ImGui::SameLine();
			}

			// Current path breadcrumb
			ImGui::Text("Path: %s", m_currentDirectory.string().c_str());

			ImGui::SameLine(ImGui::GetWindowWidth() - 100);

			if (ImGui::Button("Refresh"))
			{
				m_loadedTextures.clear();
				m_textureObjects.clear();

				//std::cout << "Refreshed" << std::endl;
			}
		}

		/**
		* @brief draws the folder tree recursively
		*/
		void DrawFolderTree(const fs::path& path)
		{
			if (!fs::exists(path) || !fs::is_directory(path))
				return;

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

			if (path == m_currentDirectory)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			bool nodeOpen = ImGui::TreeNodeEx(path.filename().string().c_str(), flags);

			if (ImGui::IsItemClicked())
			{
				m_currentDirectory = path;
			}

			if (nodeOpen)
			{
				for (const auto& entry : fs::directory_iterator(path))
				{
					if (entry.is_directory())
					{
						DrawFolderTree(entry.path());
					}
				}
				ImGui::TreePop();
			}
		}

		/**
		* @brief draws the content of the current directory
		*/
		void DrawContentView()
		{
			if (!fs::exists(m_currentDirectory))
			{
				ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "Directory does not exist");
				return;
			}

			std::vector<AssetItem> items;

			// Collect all items in current directory
			for (const auto& entry : fs::directory_iterator(m_currentDirectory))
			{
				AssetItem item;
				item.name = entry.path().filename().string();
				item.fullPath = entry.path();
				item.isDirectory = entry.is_directory();
				item.type = DetermineAssetType(entry.path());

				// Apply search filter
				if (!m_searchFilter.empty())
				{
					std::string lowerName = item.name;
					std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
					std::string lowerFilter = m_searchFilter;
					std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), ::tolower);

					if (lowerName.find(lowerFilter) == std::string::npos)
						continue;
				}

				items.push_back(item);
			}

			// Sort: folders first, then files
			std::sort(items.begin(), items.end(), [](const AssetItem& a, const AssetItem& b) {
				if (a.isDirectory != b.isDirectory)
					return a.isDirectory;
				return a.name < b.name;
				});

			DrawGridView(items);
		}

		/**
		* @brief draws items in grid view
		*/
		void DrawGridView(const std::vector<AssetItem>& items)
		{
			float padding = 12.0f;
			float cellSize = m_thumbnailSize + padding;
			float panelWidth = ImGui::GetContentRegionAvail().x;
			int columnCount = std::max(1, (int)(panelWidth / cellSize));

			ImGui::Columns(columnCount, nullptr, false);

			for (const auto& item : items)
			{
				ImGui::PushID(item.fullPath.string().c_str());

				bool isSelected = (item.fullPath.string() == m_selectedAssetPath);
				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
				}

				if (item.isDirectory)
				{
					// Folder button
					if (ImGui::Button("Folder", ImVec2(m_thumbnailSize, m_thumbnailSize)))
					{
						if (ImGui::IsMouseDoubleClicked(0))
						{
							m_currentDirectory = item.fullPath;
						} else
						{
							m_selectedAsset = item;
							m_selectedAssetPath = item.fullPath.string();
							m_showAssetProperties = false;
						}
					}
				} else
				{
					DrawItemThumbnail(item);
				}

				if (isSelected)
				{
					ImGui::PopStyleColor();
				}

				// Handle selection
				if (ImGui::IsItemClicked() && !ImGui::IsMouseDoubleClicked(0))
				{
					m_selectedAsset = item;
					m_selectedAssetPath = item.fullPath.string();
					m_showAssetProperties = false;
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					if (item.isDirectory)
					{
						m_currentDirectory = item.fullPath;
					}
					else
					{
						m_selectedAsset = item;
						m_selectedAssetPath = item.fullPath.string();
						m_showAssetProperties = true;

						// Load appropriate preview based on type
						if (item.type == AssetType::Script)
						{
							m_scriptsPanel.SelectScript(item.fullPath.string());
						}
						else if (item.type == AssetType::Prefab)
						{
							std::string prefabName = item.name;
							if (prefabName.ends_with(".json"))
							{
								prefabName = prefabName.substr(0, prefabName.size() - 5);
							}
							m_prefabsPanel.SelectPrefab(prefabName);
						}
					}
				}

				// Double-click to open folders
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && item.isDirectory)
				{
					m_currentDirectory = item.fullPath;
				}

				// Drag and drop source for assets
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					std::string pathStr = item.fullPath.string();

					if (item.type == AssetType::Image)
					{
						// Make the file path of the image to be the relative path from the RESOURCE_PATH
						std::filesystem::path relPath = std::filesystem::relative(item.fullPath, m_assetsRoot);
						pathStr = relPath.string();

						ImGui::SetDragDropPayload("ASSET_IMAGE", pathStr.c_str(), pathStr.size() + 1);
						ImTextureID texID = GetOrLoadTexture(pathStr);
						if (texID)
						{
							ImGui::Image(texID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
						}
					} else if (item.type == AssetType::Audio)
					{
						ImGui::SetDragDropPayload("ASSET_AUDIO", pathStr.c_str(), pathStr.size() + 1);
					} else if (item.type == AssetType::Script)
					{
						ImGui::SetDragDropPayload("ASSET_SCRIPT", pathStr.c_str(), pathStr.size() + 1);
					}

					else
					{
						std::string ext = item.fullPath.extension().string();

						if (ext == ".json")
						{
							ImGui::SetDragDropPayload("ASSET_PREFAB", pathStr.c_str(), pathStr.size() + 1);
						} else
						{
							//ImGui::SetDragDropPayload("ASSET_ITEM", pathStr.c_str(), pathStr.size() + 1);
						}
					}

					ImGui::Text("%s", item.name.c_str());
					ImGui::EndDragDropSource();
				}

				// Show tooltip on hover
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("File: %s", item.name.c_str());
					ImGui::Text("Path: %s", item.fullPath.string().c_str());
					ImGui::EndTooltip();
				}

				ImGui::TextWrapped("%s", item.name.c_str());
				ImGui::NextColumn();
				ImGui::PopID();
			}

			ImGui::Columns(1);
		}

		/**
		* @brief draws thumbnail for an item
		*/
		void DrawItemThumbnail(const AssetItem& item)
		{
			//std::cout << "[AssetPanel] item fullpath: " << item.fullPath.filename().string() << '\n';


			switch (item.type)
			{
			case AssetType::Image:
			{
				// Use the relative file path instead
				std::filesystem::path relPath = std::filesystem::relative(item.fullPath, m_assetsRoot);
				ImTextureID texID = GetOrLoadTexture(relPath.string());
				if (texID)
				{
					ImGui::ImageButton(
						//item.fullPath.string().c_str(),
						relPath.string().c_str(),
						texID,
						ImVec2(m_thumbnailSize, m_thumbnailSize),
						ImVec2(0, 1), ImVec2(1, 0));
				} else
				{
					ImGui::Button("Image", ImVec2(m_thumbnailSize, m_thumbnailSize));
				}
				break;
			}
			case AssetType::Script:
				ImGui::Button("Script", ImVec2(m_thumbnailSize, m_thumbnailSize));
				break;
			case AssetType::Prefab:
				ImGui::Button("Prefab", ImVec2(m_thumbnailSize, m_thumbnailSize));
				break;
			case AssetType::Audio:
				ImGui::Button("Audio", ImVec2(m_thumbnailSize, m_thumbnailSize));
				break;
			default:
				ImGui::Button("File", ImVec2(m_thumbnailSize, m_thumbnailSize));
				break;
			}
		}

		/**
		* @brief draws the asset properties window
		*/
		void DrawAssetProperties()
		{
			ImGui::Begin("Asset Properties", &m_showAssetProperties);

			if (!m_selectedAssetPath.empty())
			{
				fs::path selectedPath(m_selectedAssetPath);

				ImGui::Text("Asset Inspector");
				ImGui::Separator();

				ImGui::Text("Name: %s", selectedPath.filename().string().c_str());
				ImGui::Text("Type: %s", selectedPath.extension().string().c_str());
				ImGui::Text("Path: %s", m_selectedAssetPath.c_str());

				if (fs::exists(selectedPath) && !m_selectedAsset.isDirectory)
				{
					auto fileSize = fs::file_size(selectedPath);
					ImGui::Text("Size: %.2f KB", fileSize / 1024.0f);
				}

				ImGui::Separator();

				// Show appropriate preview based on asset type
				switch (m_selectedAsset.type)
				{
				case AssetType::Image:
				{
					ImGui::Text("Preview:");
					// Use the relative file path instead
					std::filesystem::path relPath = std::filesystem::canonical(selectedPath);
					ImTextureID texID = GetOrLoadTexture(relPath.string());

					//ImTextureID texID = GetOrLoadTexture(selectedPath.filename().string());
					if (texID)
					{
						ImGui::Image(texID, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));
					}
					break;
				}
				case AssetType::Script:
				{
					ImGui::Text("Script Contents:");
					std::string contents = m_scriptsPanel.GetScriptContents(m_selectedAssetPath);
					ImGui::BeginChild("ScriptContent", ImVec2(0, 300), true);
					ImGui::TextWrapped("%s", contents.c_str());
					ImGui::EndChild();
					break;
				}
				case AssetType::Prefab:
				{
					std::string prefabName = selectedPath.stem().string();
					m_prefabsPanel.DrawPrefabProperties(prefabName);
					break;
				}
				case AssetType::Audio:
				{
					ImGui::Text("Audio File");
					ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Supported format");

					std::string ext = selectedPath.extension().string();
					if (ext == ".mp3")
						ImGui::Text("Format: MP3");
					else if (ext == ".wav")
						ImGui::Text("Format: WAV");
					break;
				}
				default:
					ImGui::TextDisabled("No preview available");
					break;
				}

				ImGui::Separator();

				// Close button
				if (ImGui::Button("Close"))
				{
					m_showAssetProperties = false;
					m_selectedAssetPath = "";
				}

				ImGui::SameLine();

				// Delete button (only for non-directories and images)
				if (!m_selectedAsset.isDirectory)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));

					if (ImGui::Button("Delete", ImVec2(100, 0)))
					{
						ImGui::OpenPopup("Delete Asset?");
					}

					ImGui::PopStyleColor(3);

					// Delete confirmation popup
					if (ImGui::BeginPopupModal("Delete Asset?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Are you sure you want to delete this asset?");
						ImGui::Separator();

						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
							"File: %s", selectedPath.filename().string().c_str());

						ImGui::Separator();

						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
						if (ImGui::Button("Delete", ImVec2(120, 0)))
						{
							DeleteAsset(selectedPath.string());
							ImGui::CloseCurrentPopup();
						}
						ImGui::PopStyleColor();

						ImGui::SameLine();

						if (ImGui::Button("Cancel", ImVec2(120, 0)))
						{
							ImGui::CloseCurrentPopup();
						}

						ImGui::EndPopup();
					}
				}
			}
			ImGui::End();
		}

		/**
		* @brief handles external file drop events
		*/
		void HandleExternalFileDrop()
		{
			auto appSystem = m_coord.GetSystem<ApplicationSystem>();
			if (!appSystem) return;

			const auto& droppedFiles = appSystem->GetDroppedFiles();

			if (!droppedFiles.empty())
			{
				m_pendingImports = droppedFiles;
				m_showImportDialog = true;
				m_importDestination = m_currentDirectory.string();
				appSystem->ClearDroppedFiles();
			}
		}

		/**
		* @brief draws the import dialog
		*/
		void DrawImportDialog()
		{
			if (!ImGui::IsPopupOpen("Import Files"))
			{
				ImGui::OpenPopup("Import Files");
			}

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Appearing);

			if (ImGui::BeginPopupModal("Import Files", &m_showImportDialog))
			{
				ImGui::Text("Import %zu file(s) to Assets folder", m_pendingImports.size());
				ImGui::Separator();

				ImGui::Text("Files:");
				ImGui::BeginChild("FileList", ImVec2(0, 150), true);

				bool hasUnsupportedAudio = false;
				bool hasSupportedFiles = false;

				for (size_t i = 0; i < m_pendingImports.size(); i++)
				{
					fs::path filePath(m_pendingImports[i]);
					std::string filename = filePath.filename().string();
					std::string ext = filePath.extension().string();
					std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

					if (IsImageFile(ext))
					{
						ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f),
							"[Image] %s", filename.c_str());
						hasSupportedFiles = true;
					} else if (ext == ".mp3" || ext == ".wav")
					{
						ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f),
							"[Audio] %s", filename.c_str());
						hasSupportedFiles = true;
					} else if (ext == ".ogg" || ext == ".flac" || ext == ".aac")
					{
						ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
							"[Audio - Unsupported] %s", filename.c_str());
						hasUnsupportedAudio = true;
					} else
					{
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f),
							"[Other] %s", filename.c_str());
						hasSupportedFiles = true; // allow other file types
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text("Source: %s", m_pendingImports[i].c_str());
						ImGui::EndTooltip();
					}
				}

				ImGui::EndChild();
				ImGui::Separator();

				ImGui::Text("Import to: %s", m_importDestination.c_str());
				ImGui::Separator();

				static bool copyFiles = true;
				static bool overwriteExisting = false;

				ImGui::Checkbox("Copy files to project", &copyFiles);
				ImGui::Checkbox("Overwrite existing files", &overwriteExisting);
				ImGui::Separator();

				if (hasUnsupportedAudio)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
						"Audio file is not supported (only .mp3 and .wav");
					ImGui::Separator();
				}

				float buttonWidth = 100.0f;

				// disable Import button if there are unsupported audio files
				if (hasUnsupportedAudio)
				{
					ImGui::BeginDisabled();
					ImGui::Button("Import", ImVec2(buttonWidth, 0));
					ImGui::EndDisabled();

					if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
					{
						ImGui::BeginTooltip();
						ImGui::Text("Cannot import: unsupported audio files detected");
						ImGui::EndTooltip();
					}
				} else
				{
					if (ImGui::Button("Import", ImVec2(buttonWidth, 0)))
					{
						ImportFiles(copyFiles, overwriteExisting);
						m_showImportDialog = false;
						m_pendingImports.clear();
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
				{
					m_showImportDialog = false;
					m_pendingImports.clear();
				}

				ImGui::EndPopup();
			}
		}

		/**
		* @brief imports files to the assets folder
		*/
		void ImportFiles(bool copyFiles, bool overwriteExisting)
		{
			//m_unsupportedAudioFiles.clear();
			int successCount = 0;
			int failCount = 0;

			for (const auto& sourcePath : m_pendingImports)
			{
				try
				{
					fs::path source(sourcePath);
					fs::path destination = fs::path(m_importDestination) / source.filename();

					if (DetermineAssetType(source) == AssetType::Audio)
					{
						std::string ext = source.extension().string();
						std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

						// validate audio files - only allow .mp3 and .wav
						if (ext == ".mp3" || ext == ".wav" || DetermineAssetType(source) == AssetType::Audio)
						{
							if (ext != ".mp3" && ext != ".wav")
							{
								// m_unsupportedAudioFiles.push_back(source.filename().string());
								failCount++;
								continue;
							}
						}
					}

					if (fs::exists(destination) && !overwriteExisting)
					{
						failCount++;
						continue;
					}

					if (copyFiles)
					{
						fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
					}

					if (IsImageFile(source.extension().string()))
					{
						m_loadedTextures.erase(destination.string());
						m_textureObjects.erase(destination.string());
					}

					// dynamically load and create the newly dragged-in texture
					m_coord.GetSystem<GraphicsSystem>()->CreateTexture(destination.string());

					successCount++;
				}
				catch (const std::exception& e)
				{
					std::cerr << "Failed to import " << sourcePath << ": " << e.what() << "\n";
					failCount++;
				}

				//// show warning if there were unsupported audio files
				//if (!m_unsupportedAudioFiles.empty())
				//{
				//    m_showUnsupportedAudioWarning = true;
				//}
			}
		}

		/**
		* @brief deletes an asset from the filesystem and cache
		*/
		void DeleteAsset(const std::string& assetPath)
		{
			try
			{
				fs::path filePath(assetPath);

				if (!fs::exists(filePath))
				{
					m_showAssetProperties = false;
					m_selectedAssetPath = "";
					std::cout << "Asset path does not exist, unable to delete [" << assetPath << "]\n";
					std::cout << "FilePath -> " << filePath.string() << '\n';
					return;
				}

				std::string ext = filePath.extension().string();
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

				if (IsImageFile(ext))
				{
					auto texIt = m_loadedTextures.find(assetPath);
					if (texIt != m_loadedTextures.end())
					{
						GLuint texID = texIt->second;
						glDeleteTextures(1, &texID);
						m_loadedTextures.erase(texIt);
					}

					//fs::remove(filePath);
					m_textureObjects.erase(assetPath);
				}

				if (ext == ".mp3" || ext == ".wav")
				{
					// Add any audio-specific cleanup here if needed

				}

				if (!fs::remove(filePath)) {
					std::cout << "Failed to delete from folder: " << filePath.string() << '\n';
				}

				m_showAssetProperties = false;
				m_selectedAssetPath = "";
			}
			catch (const std::exception& e)
			{
				std::cerr << "[Assets] Error deleting asset: " << e.what() << "\n";
				m_showAssetProperties = false;
				m_selectedAssetPath = "";
			}
		}

		/**
		* @brief determines the type of an asset
		*/
		AssetType DetermineAssetType(const fs::path& path)
		{
			if (fs::is_directory(path))
				return AssetType::Folder;

			std::string ext = path.extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp")
				return AssetType::Image;

			if (ext == ".cs" || ext == ".txt")
				return AssetType::Script;

			if (ext == ".json" || path.filename().string().find("Archetype") != std::string::npos)
				return AssetType::Prefab;

			if (ext == ".wav" || ext == ".mp3")
				return AssetType::Audio;

			return AssetType::Other;
		}

		/**
		* @brief checks if a file has a supported image extension
		*/
		bool IsImageFile(const std::string& ext)
		{
			std::string lower = ext;
			std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
			return (lower == ".png" || lower == ".jpg" || lower == ".jpeg" || lower == ".bmp");
		}

		/**
		* @brief gets or loads a texture from cache
		*/
		ImTextureID GetOrLoadTexture(const std::string& path)
		{
			if (m_loadedTextures.find(path) != m_loadedTextures.end())
			{
				return m_loadedTextures[path];
			}

			std::shared_ptr<Texture> tex = std::make_shared<Texture>(path, m_coord.GetSystem<ResourceManager>());
			m_textureObjects[path] = tex;
			m_loadedTextures[path] = (ImTextureID)(intptr_t)tex->GetID();

			return m_loadedTextures[path];
		}

	};
}