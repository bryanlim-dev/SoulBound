/******************************************************************************
 * File:        AssetsPanel.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 95%)
 *              Lim Wei Jie (Secondary, 5%)
 *
 * Description:
 * Assets panel displaying and handling all game assets.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "AllPanels.h"
#include "Graphics/Texture.h"
#include "Factory/EntityFactory.h"
#include "PrefabsPanel.h"
#include <stb_image.h>

namespace fs = std::filesystem;

namespace NightSnack
{
    class AssetsPanel {
    public:
        AssetsPanel(Coordinator& coord) :
            m_coord(coord),
            m_assetsRoot("../../NightSnack Engine/assets"),
            m_thumbnailSize(96.0f),
            //m_searchFilter(""),
            m_selectedAssetPath(""),
            m_showAssetProperties(false),
            m_showImportDialog(false)
        {
            m_prefabNames.reserve(coord.GetSystem<EntityFactory>()->GetPrefabNames().size());
            for (std::string const& s : coord.GetSystem<EntityFactory>()->GetPrefabNames()) {
                m_prefabNames.emplace_back(s);
            }
            m_prefabMats.reserve(coord.GetSystem<EntityFactory>()->GetPrefabMaterials().size());
            for (auto const& [n, m] : coord.GetSystem<EntityFactory>()->GetPrefabMaterials()) {
                m_prefabMats.insert({ n, m });
            }
        }

        void Draw()
        {
            // Set initial window size and style
            //ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_Once);

            static bool firstFrame = true;
            if (firstFrame)
            {
                // Clear any leftover dropped files on first frame
                auto appSystem = m_coord.GetSystem<ApplicationSystem>();
                if (appSystem) appSystem->ClearDroppedFiles();
                firstFrame = false;
            }
            else
            {
                // Normal operation: check for dropped files
                HandleExternalFileDrop();
            }

            // Options Window
            ImGui::Begin("Assets");
            DrawToolbar();

            if (ImGui::BeginTabBar("AssetsTabs"))
            {
                if (ImGui::BeginTabItem("Images"))
                {
                    DrawImagesTab(); // Render all image thumbnails
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Files"))
                {
                    DrawAllFilesTab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Prefabs"))
                {
                    //DrawPrefabsPanel(m_prefabs);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();

            if (m_showImportDialog)
            {
                DrawImportDialog();
            }

            // ================================================================
            // STEP 4: DRAW ASSET PROPERTIES (if asset selected)
            // ================================================================
            if (m_showAssetProperties && !m_selectedAssetPath.empty())
            {
                DrawAssetProperties();
            }
        }

    private:
        Coordinator& m_coord;
        fs::path m_assetsRoot;// Root directory of the assets folder

        std::unordered_map<std::string, GLuint> m_loadedTextures; // Cache of loaded textures
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureObjects;

        float m_thumbnailSize;                   // Size of thumbnail images
        std::string m_searchFilter;              // Current search text
        std::string m_selectedAssetPath;         // Currently selected asset
        bool m_showAssetProperties;              // Show properties window

        bool m_showImportDialog;
        std::vector<std::string> m_pendingImports;
        std::vector<std::string> m_prefabNames;
        std::unordered_map<std::string, std::string> m_prefabMats;
        std::string m_importDestination;

        void HandleExternalFileDrop()
        {
            // Get the application system
            auto appSystem = m_coord.GetSystem<ApplicationSystem>();
            if (!appSystem) return;

            // Check if any files were dropped
            const auto& droppedFiles = appSystem->GetDroppedFiles();

            if (!droppedFiles.empty())
            {
                // Files were dropped! Store them and show import dialog
                m_pendingImports = droppedFiles;
                m_showImportDialog = true;
                m_importDestination = m_assetsRoot.string();

                std::cout << "[Assets] Received " << droppedFiles.size()
                    << " dropped file(s)\n";

                // Clear the application's dropped files list
                appSystem->ClearDroppedFiles();
            }
        }

        void DrawImportDialog()
        {
            if (!ImGui::IsPopupOpen("Import Files"))
            {
                ImGui::OpenPopup("Import Files");
            }

            // Center the dialog
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Appearing);

            if (ImGui::BeginPopupModal("Import Files", &m_showImportDialog))
            {
                ImGui::Text("Import %zu file(s) to Assets folder", m_pendingImports.size());
                ImGui::Separator();

                // ----------------------------------------------------------------
                // Show list of files to import
                // ----------------------------------------------------------------
                ImGui::Text("Files:");
                ImGui::BeginChild("FileList", ImVec2(0, 150), true);

                for (size_t i = 0; i < m_pendingImports.size(); i++)
                {
                    fs::path filePath(m_pendingImports[i]);
                    std::string filename = filePath.filename().string();
                    std::string ext = filePath.extension().string();

                    // Color code by file type
                    if (IsImageFile(ext))
                    {
                        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f),
                            "[Image] %s", filename.c_str());
                    }
                    else
                    {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f),
                            "[Other] %s", filename.c_str());
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

                // ----------------------------------------------------------------
                // Destination folder
                // ----------------------------------------------------------------
                ImGui::Text("Import to: %s", m_importDestination.c_str());
                ImGui::Separator();

                // ----------------------------------------------------------------
                // Import options
                // ----------------------------------------------------------------
                static bool copyFiles = true;
                static bool overwriteExisting = false;

                ImGui::Checkbox("Copy files to project", &copyFiles);
                ImGui::Checkbox("Overwrite existing files", &overwriteExisting);
                ImGui::Separator();

                // ----------------------------------------------------------------
                // Action buttons
                // ----------------------------------------------------------------
                float buttonWidth = 100.0f;

                if (ImGui::Button("Import", ImVec2(buttonWidth, 0)))
                {
                    ImportFiles(copyFiles, overwriteExisting);
                    m_showImportDialog = false;
                    m_pendingImports.clear();
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

        void ImportFiles(bool copyFiles, bool overwriteExisting)
        {
            std::cout << "[Assets] Importing " << m_pendingImports.size() << " files...\n";

            int successCount = 0;
            int failCount = 0;

            for (const auto& sourcePath : m_pendingImports)
            {
                try
                {
                    fs::path source(sourcePath);
                    fs::path destination = fs::path(m_importDestination) / source.filename();

                    // Check if file exists
                    if (fs::exists(destination) && !overwriteExisting)
                    {
                        std::cout << "[Assets] Skipped (already exists): "
                            << destination.string() << "\n";
                        failCount++;
                        continue;
                    }

                    // Copy file
                    if (copyFiles)
                    {
                        fs::copy_file(source, destination,
                            fs::copy_options::overwrite_existing);
                        std::cout << "[Assets] Copied: " << source.filename().string()
                            << " -> " << destination.string() << "\n";
                    }

                    // Clear texture cache to reload
                    if (IsImageFile(source.extension().string()))
                    {
                        m_loadedTextures.erase(destination.string());
                        m_textureObjects.erase(destination.string());
                    }

                    successCount++;
                }
                catch (const std::exception& e)
                {
                    std::cerr << "[Assets] Failed to import " << sourcePath
                        << ": " << e.what() << "\n";
                    failCount++;
                }
            }

            std::cout << "[Assets] Import complete: " << successCount
                << " success, " << failCount << " failed\n";
        }

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

                if (fs::exists(selectedPath))
                {
                    auto fileSize = fs::file_size(selectedPath);
                    ImGui::Text("Size: %.2f KB", fileSize / 1024.0f);
                }

                ImGui::Separator();

                // Preview for images
                if (IsImageFile(selectedPath.extension().string()))
                {
                    ImGui::Text("Preview:");
                    ImTextureID texID = GetOrLoadTexture(m_selectedAssetPath);
                    if (texID)
                    {
                        ImGui::Image(texID, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));
                    }
                }

                ImGui::Separator();

                // buttons 

                if (ImGui::Button("Close"))
                {
                    m_showAssetProperties = false;
                    m_selectedAssetPath = "";
                }

                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));

                if (ImGui::Button("Delete", ImVec2(100, 0)))
                {
                    // Show confirmation popup
                    ImGui::OpenPopup("Delete Asset?");
                }

                ImGui::PopStyleColor(3);

                // delete confirmation popup
                // Safety measure to prevent accidental deletion
                if (ImGui::BeginPopupModal("Delete Asset?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Are you sure you want to delete this asset?");
                    ImGui::Separator();

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
                        "File: %s", selectedPath.filename().string().c_str());

                    ImGui::Separator();

                    // Confirm delete button
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                    if (ImGui::Button("Delete", ImVec2(120, 0)))
                    {
                        DeleteAsset(m_selectedAssetPath);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::PopStyleColor();

                    ImGui::SameLine();

                    // Cancel button
                    if (ImGui::Button("Cancel", ImVec2(120, 0)))
                    {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
            }

            ImGui::End();
        }

        void DrawToolbar()
        {
            // ----------------------------------------------------------------
            // SEARCH BAR - Filter assets by name
            // ----------------------------------------------------------------
            /*static char searchBuffer[256] = "";
            ImGui::SetNextItemWidth(200.0f);
            if (ImGui::InputTextWithHint("##Search", "Search assets...", searchBuffer, 256))
            {
                m_searchFilter = std::string(searchBuffer);
            }

            ImGui::SameLine();*/

            //// ----------------------------------------------------------------
            //// REFRESH BUTTON - Rescan assets folder
            //// ----------------------------------------------------------------
            //if (ImGui::Button("Refresh"))
            //{
            //    // Clear texture cache to force reload
            //    m_loadedTextures.clear();
            //    m_textureObjects.clear();
            //    std::cout << "[Assets] Refreshed asset cache\n";
            //}

            //if (ImGui::Button("Import Files..."))
            //{
            //    // import window
            //}

            //ImGui::SameLine();
            //ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
            //    "");
        }

        void DrawImagesTab()
        {
            // ----------------------------------------------------------------
            // CALCULATE GRID LAYOUT
            // Determine how many thumbnails fit per row
            // ----------------------------------------------------------------
            static float padding = 12.0f;
            float cellSize = m_thumbnailSize + padding;
            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = std::max(1, (int)(panelWidth / cellSize));

            // Create column layout for grid
            ImGui::Columns(columnCount, 0, false);

            // ----------------------------------------------------------------
            // ITERATE THROUGH ALL IMAGE FILES
            // Scan assets folder recursively for images
            // ----------------------------------------------------------------
            for (auto const& entry : fs::recursive_directory_iterator(m_assetsRoot))
            {
                // Skip directories - only process files
                if (!entry.is_regular_file())
                    continue;

                // Check if file is an image (png, jpg, etc.)
                std::string ext = entry.path().extension().string();
                if (!IsImageFile(ext))
                    continue;

                // Get full path as string
                std::string pathStr = entry.path().string();

                /*
                // ----------------------------------------------------------------
                // SEARCH FILTER - Skip if doesn't match search
                // ----------------------------------------------------------------
                if (!m_searchFilter.empty())
                {
                    std::string filename = entry.path().filename().string();
                    std::string lowerFilename = filename;
                    std::string lowerFilter = m_searchFilter;

                    // Convert to lowercase for case-insensitive search
                    std::transform(lowerFilename.begin(), lowerFilename.end(),
                        lowerFilename.begin(), ::tolower);
                    std::transform(lowerFilter.begin(), lowerFilter.end(),
                        lowerFilter.begin(), ::tolower);

                    // Skip if filename doesn't contain search term
                    if (lowerFilename.find(lowerFilter) == std::string::npos)
                        continue;
                }
                */

                // ----------------------------------------------------------------
                // LOAD TEXTURE - Get or load thumbnail from cache
                // ----------------------------------------------------------------
                ImTextureID texID = GetOrLoadTexture(pathStr);
                if (!texID)
                    continue; // Skip if failed to load

                // Push unique ID for this asset (prevents ImGui conflicts)
                ImGui::PushID(pathStr.c_str());

                // ----------------------------------------------------------------
                // HIGHLIGHT IF SELECTED (Unity blue highlight)
                // ----------------------------------------------------------------
                if (m_selectedAssetPath == pathStr)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                }

                // ----------------------------------------------------------------
                // RENDER THUMBNAIL - Clickable image button
                // ----------------------------------------------------------------
                ImGui::ImageButton(pathStr.c_str(), texID, 
                    ImVec2(m_thumbnailSize, m_thumbnailSize), ImVec2(0, 1), ImVec2(1, 0));


                // Pop highlight color if was selected
                if (m_selectedAssetPath == pathStr)
                {
                    ImGui::PopStyleColor();
                }

                // ----------------------------------------------------------------
                // CLICK TO SELECT - Single click selects the asset
                // ----------------------------------------------------------------
                if (ImGui::IsItemClicked())
                {
                    m_selectedAssetPath = pathStr;
                    m_showAssetProperties = true;
                    //std::cout << "[Assets] Selected: " << pathStr << "\n";
                }

                // DRAG SOURCE - ENABLE DRAGGING 
                // This allows dragging the image to Scene panel
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    // SET PAYLOAD - Store the asset path as drag data
                    ImGui::SetDragDropPayload("ASSET_IMAGE", pathStr.c_str(), pathStr.size() + 1);

                    // DRAG PREVIEW - Show thumbnail while dragging
                    ImGui::Image(texID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::Text("%s", entry.path().filename().string().c_str());

                    ImGui::EndDragDropSource();
                }

                // TOOLTIP - Show info on hover
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Drag to Scene to add");
                    ImGui::Text("File: %s", entry.path().filename().string().c_str());
                    ImGui::Text("Path: %s", pathStr.c_str());
                    ImGui::EndTooltip();
                }

                // Display filename text below thumbnail
                ImGui::TextWrapped("%s", entry.path().filename().string().c_str());

                // Move to next column in grid
                ImGui::NextColumn();

                ImGui::PopID();
            }

            // End grid layout
            ImGui::Columns(1);
        }

        // Displays all files (any type) inside the assets folder
        void DrawAllFilesTab()
        {
            // Use a tree view to show folders & files
            DrawDirectoryRecursive(m_assetsRoot);
        }

        // Recursively displays folders and their files as a tree.
        void DrawDirectoryRecursive(const fs::path& dir)
        {
            for (const auto& entry : fs::directory_iterator(dir))
            {
                const auto& path = entry.path();
                std::string name = path.filename().string();

                if (entry.is_directory())
                {
                    // Create a collapsible tree node for folders
                    if (ImGui::TreeNode(name.c_str()))
                    {
                        DrawDirectoryRecursive(path); // recurse into subfolder
                        ImGui::TreePop();
                    }
                }
                else if (entry.is_regular_file())
                {
                    std::string ext = path.extension().string();
                    if (IsImageFile(ext))
                    {
                        continue;
                    }
                    ImGui::BulletText("%s", name.c_str());
                }
            }
        }

        // Checks if a file has a supported image extension
        bool IsImageFile(const std::string& ext)
        {
            std::string lower = ext;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            return (lower == ".png" || lower == ".jpg" || lower == ".jpeg" || lower == ".bmp");
        }

        ImTextureID GetOrLoadTexture(const std::string& path)
        {
            // If already cached, return it
            if (m_loadedTextures.find(path) != m_loadedTextures.end())
            {
                return m_loadedTextures[path];
            }

            //stbi_set_flip_vertically_on_load(false);

            // --- Use your Texture class to load the image --- //
            std::shared_ptr<Texture> tex = std::make_shared<Texture>(path, m_coord.GetSystem<ResourceManager>());

            // Cache the texture (store both Texture and ImGui ID)
            m_textureObjects[path] = tex;
            m_loadedTextures[path] = (ImTextureID)(intptr_t)tex->GetID();

            return m_loadedTextures[path];
        }

        void DeleteAsset(const std::string& assetPath)
        {
            try
            {
                fs::path filePath(assetPath);

                // check if the file exists
                if (!fs::exists(filePath))
                {
                    m_showAssetProperties = false;
                    m_selectedAssetPath = "";
                    return;
                }

                if (IsImageFile(filePath.extension().string()))
                {
                    // Remove from cache
                    auto texIt = m_loadedTextures.find(assetPath);
                    if (texIt != m_loadedTextures.end())
                    {
                        // Delete OpenGL texture
                        GLuint texID = texIt->second;
                        glDeleteTextures(1, &texID);
                        m_loadedTextures.erase(texIt);
                        //std::cout << "[Assets] Removed texture from cache\n";
                    }

                    if (fs::remove(filePath))
                    {
                        std::cout << "[Assets] Successfully deleted file: " << assetPath << "\n";
                    }
                    else
                    {
                        std::cerr << "[Assets] Failed to delete file: " << assetPath << "\n";
                    }

                    // Remove texture object
                    m_textureObjects.erase(assetPath);
                }

                // close the asset inspector window
                m_showAssetProperties = false;
                m_selectedAssetPath = "";

            }
            catch (const std::exception& e)
            {
                //std::cerr << "[Assets] Error deleting asset: " << e.what() << "\n";

                // close the window
                m_showAssetProperties = false;
                m_selectedAssetPath = "";
            }
        }
    };
}
