/******************************************************************************
 * File:        AssetsPanel.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Drawing of assets and files tab with thumbnails
 *              
 * Description:
 * Assets panel displaying and handling all game assets.
 * Displays thumbnails for images and allows importing files from external sources.
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
    /**
    * @brief Assets Panel for managing game assets
    */
    class AssetsPanel {
    public:
        /**
        * @brief Constructor
        * @param coord reference to the ECS coordinator
        */
        AssetsPanel(Coordinator& coord) :
            m_coord(coord),
            m_assetsRoot("../../../NightSnack Engine/assets"),
            m_thumbnailSize(96.0f),
            //m_searchFilter(""),
            m_selectedAssetPath(""),
            m_showAssetProperties(false),
            m_showImportDialog(false),
            m_prefabsPanel(coord, "../../../NightSnack Engine/assets/GameData/Archetypes/Archetypes.json")
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
        void Draw()
        {
            // Set initial window size and style
            //ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_Once);
            static bool firstFrame = true;
            if (firstFrame)
            {
                // clear any leftover dropped files on first frame
                auto appSystem = m_coord.GetSystem<ApplicationSystem>();
                if (appSystem) appSystem->ClearDroppedFiles();
                firstFrame = false;
            }
            else
            {
                // check for dropped files from file explorer
                HandleExternalFileDrop();
            }

            ImGui::Begin("Assets");
            // not in use for now
            // DrawToolbar();

            if (ImGui::BeginTabBar("AssetsTabs"))
            {
                if (ImGui::BeginTabItem("Images"))
                {
                    // render all image thumbnails
                    DrawImagesTab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Files"))
                {
                    // list out files
                    DrawAllFilesTab();
                    ImGui::EndTabItem();
                }

                // handle the prefab tab item creation
                m_prefabsPanel.Draw();

                ImGui::EndTabBar();
            }
            ImGui::End();

            if (m_showImportDialog)
            {
                // import dialog to show what file is going to be imported
                DrawImportDialog();
            }

           // draw asset properties tab if asset is selected
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

        PrefabsPanel m_prefabsPanel;  // prefabs panel instance

        /**
        * @brief handles external file drop events
        */
        void HandleExternalFileDrop()
        {
            // get the application system
            auto appSystem = m_coord.GetSystem<ApplicationSystem>();
            if (!appSystem) return;

            // check if any files were dropped
            const auto& droppedFiles = appSystem->GetDroppedFiles();

            if (!droppedFiles.empty())
            {
                // store them and show import dialog
                m_pendingImports = droppedFiles;
                m_showImportDialog = true;
                m_importDestination = m_assetsRoot.string();

                /*std::cout << "[Assets] Received " << droppedFiles.size()
                    << " dropped file(s)\n";*/

                // clear the application's dropped files list
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

            // center the dialog
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Appearing);

            if (ImGui::BeginPopupModal("Import Files", &m_showImportDialog))
            {
                ImGui::Text("Import %zu file(s) to Assets folder", m_pendingImports.size());
                ImGui::Separator();

                // show list of files to import
                ImGui::Text("Files:");
                ImGui::BeginChild("FileList", ImVec2(0, 150), true);

                for (size_t i = 0; i < m_pendingImports.size(); i++)
                {
                    fs::path filePath(m_pendingImports[i]);
                    std::string filename = filePath.filename().string();
                    std::string ext = filePath.extension().string();

                    // color code by file type
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

				// the folder that files will be imported to
                ImGui::Text("Import to: %s", m_importDestination.c_str());
                ImGui::Separator();

                // import options
                static bool copyFiles = true;
                static bool overwriteExisting = false;

                ImGui::Checkbox("Copy files to project", &copyFiles);
                ImGui::Checkbox("Overwrite existing files", &overwriteExisting);
                ImGui::Separator();

                // action buttons
                float buttonWidth = 100.0f;

                if (ImGui::Button("Import", ImVec2(buttonWidth, 0)))
                {
                    ImportFiles(copyFiles, overwriteExisting);
                    m_showImportDialog = false;
                    m_pendingImports.clear();
                }
                
                // create cancel button in the same line
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
        * @param copyFiles copy files to project
        * @param overwriteExisting whether to overwrite existing files
        */
        void ImportFiles(bool copyFiles, bool overwriteExisting)
        {
            //std::cout << "[Assets] Importing " << m_pendingImports.size() << " files...\n";

            int successCount = 0;
            int failCount = 0;

            for (const auto& sourcePath : m_pendingImports)
            {
                try
                {
                    fs::path source(sourcePath);
                    fs::path destination = fs::path(m_importDestination) / source.filename();

                    // check if file exists
                    if (fs::exists(destination) && !overwriteExisting)
                    {
                       /* std::cout << "[Assets] Skipped (already exists): "
                            << destination.string() << "\n";*/
                        failCount++;
                        continue;
                    }

                    // copy file
                    if (copyFiles)
                    {
                        fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
                        /*std::cout << "[Assets] Copied: " << source.filename().string() << " -> " << destination.string() << "\n";*/
                    }

                    // clear texture cache to reload
                    if (IsImageFile(source.extension().string()))
                    {
                        m_loadedTextures.erase(destination.string());
                        m_textureObjects.erase(destination.string());
                    }

                    successCount++;
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Failed to import " << sourcePath << ": " << e.what() << "\n";
                    failCount++;
                }
            }
            //std::cout << "[Assets] Import complete: " << successCount << " success, " << failCount << " failed\n";
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

                if (fs::exists(selectedPath))
                {
                    auto fileSize = fs::file_size(selectedPath);
                    ImGui::Text("Size: %.2f KB", fileSize / 1024.0f);
                }

                ImGui::Separator();

                // preview for images
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

                // close buttoon
                if (ImGui::Button("Close"))
                {
                    m_showAssetProperties = false;
                    m_selectedAssetPath = "";
                }

                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));

                // delete button
                if (ImGui::Button("Delete", ImVec2(100, 0)))
                {
                    // Show confirmation popup
                    ImGui::OpenPopup("Delete Asset?");
                }

                ImGui::PopStyleColor(3);

                // delete confirmation popup
                // safety measure to prevent accidental deletion
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

                    // cancel button
                    if (ImGui::Button("Cancel", ImVec2(120, 0)))
                    {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
            }
            ImGui::End();
        }

        // not used for now
        //void DrawToolbar()
        //{
        //    // search bar to filter assets by name

        //    /*static char searchBuffer[256] = "";
        //    ImGui::SetNextItemWidth(200.0f);
        //    if (ImGui::InputTextWithHint("##Search", "Search assets...", searchBuffer, 256))
        //    {
        //        m_searchFilter = std::string(searchBuffer);
        //    }

        //    ImGui::SameLine();*/

        //    // refresh button to Rescan assets folder
        //     
        //    if (ImGui::Button("Refresh"))
        //    {
        //        // Clear texture cache to force reload
        //        m_loadedTextures.clear();
        //        m_textureObjects.clear();
        //        std::cout << "[Assets] Refreshed asset cache\n";
        //    }

        //    if (ImGui::Button("Import Files..."))
        //    {
        //        // import window
        //    }

        //    ImGui::SameLine();
        //    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        //        "");
        //}

        /**
        * @brief draws the images in the assets tab
        */
        void DrawImagesTab()
        {
            // calculate grid layout
           // determine how many thumbnails fit per row
            static float padding = 12.0f;
            float cellSize = m_thumbnailSize + padding;
            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = std::max(1, (int)(panelWidth / cellSize));

            // create column layout for grid
            ImGui::Columns(columnCount, 0, false);

            // iterate through all image files
            // scan assets folder recursively for images
            for (auto const& entry : fs::recursive_directory_iterator(m_assetsRoot))
            {
                // skip directories and only process files
                if (!entry.is_regular_file())
                    continue;

                // check if file is an image (png, jpg, etc.)
                std::string ext = entry.path().extension().string();
                if (!IsImageFile(ext)) continue;

                // get full path as string
                std::string pathStr = entry.path().string();

                // search filter not in use for now
                /*
                // SEARCH FILTER - Skip if doesn't match search
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

                // load texture from cache
                ImTextureID texID = GetOrLoadTexture(pathStr);
                if (!texID) continue;

                // push unique ID for the asset
                ImGui::PushID(pathStr.c_str());

                // highlight if selected
                // change colour values accordingly
                if (m_selectedAssetPath == pathStr)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                }

				// clickable image button with thumbnail
                ImGui::ImageButton(pathStr.c_str(), texID, 
                    ImVec2(m_thumbnailSize, m_thumbnailSize), ImVec2(0, 1), ImVec2(1, 0));


                // pop highlight color if was selected
                if (m_selectedAssetPath == pathStr)
                {
                    ImGui::PopStyleColor();
                }

                // click to select
                if (ImGui::IsItemClicked())
                {
                    m_selectedAssetPath = pathStr;
                    m_showAssetProperties = true;
                    //std::cout << "[Assets] Selected: " << pathStr << "\n";
                }

                // not in use for now
                //// enable dragging
                //// This allows dragging the image to Scene panel
                //if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                //{
                //    // SET PAYLOAD - Store the asset path as drag data
                //    ImGui::SetDragDropPayload("ASSET_IMAGE", pathStr.c_str(), pathStr.size() + 1);

                //    // DRAG PREVIEW - Show thumbnail while dragging
                //    ImGui::Image(texID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
                //    ImGui::Text("%s", entry.path().filename().string().c_str());

                //    ImGui::EndDragDropSource();
                //}

                // show info on hover
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    //ImGui::Text("Drag to Scene to add");
                    ImGui::Text("File: %s", entry.path().filename().string().c_str());
                    ImGui::Text("Path: %s", pathStr.c_str());
                    ImGui::EndTooltip();
                }

                // display filename text below thumbnail
                ImGui::TextWrapped("%s", entry.path().filename().string().c_str());

                // move to next column in grid
                ImGui::NextColumn();

                ImGui::PopID();
            }

            // end grid layout
            ImGui::Columns(1);
        }

        /**
        * @brief displays all files inside the assets folder
        */
        void DrawAllFilesTab()
        {
            // use a tree view to show folders and files
            DrawDirectoryRecursive(m_assetsRoot);
        }

        /**
        * @brief recursively displays folders and their files as a tree
        * @param dir the directory to display
        */
        void DrawDirectoryRecursive(const fs::path& dir)
        {
            for (const auto& entry : fs::directory_iterator(dir))
            {
                const auto& path = entry.path();
                std::string name = path.filename().string();

                if (entry.is_directory())
                {
                    // create a collapsible tree node for folders
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

        /**
        * @brief checks if a file has a supported image extension
        * @param ext file extension to check
        * @return true if the file is an image, else false
        */
        bool IsImageFile(const std::string& ext)
        {
            std::string lower = ext;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            return (lower == ".png" || lower == ".jpg" || lower == ".jpeg" || lower == ".bmp");
        }

        /**
        * @brief gets or loads a texture from cache
        * @param path path to the texture file
        * @return ImTextureID for the texture
        */

        ImTextureID GetOrLoadTexture(const std::string& path)
        {
            // If already cached, return it
            if (m_loadedTextures.find(path) != m_loadedTextures.end())
            {
                return m_loadedTextures[path];
            }

            //stbi_set_flip_vertically_on_load(false);

            std::shared_ptr<Texture> tex = std::make_shared<Texture>(path, m_coord.GetSystem<ResourceManager>());

            // cache the texture (store both Texture and ImGui ID)
            m_textureObjects[path] = tex;
            m_loadedTextures[path] = (ImTextureID)(intptr_t)tex->GetID();

            return m_loadedTextures[path];
        }

        /**
        * @brief deletes an asset from the filesystem and cache
        * @param assetPath path to the asset to delete
        */
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
                    // remove from cache
                    auto texIt = m_loadedTextures.find(assetPath);
                    if (texIt != m_loadedTextures.end())
                    {
                        // delete OpenGL texture
                        GLuint texID = texIt->second;
                        glDeleteTextures(1, &texID);
                        m_loadedTextures.erase(texIt);
                        //std::cout << "[Assets] Removed texture from cache\n";
                    }

                    /*if (fs::remove(filePath))
                    {
                        std::cout << "[Assets] Successfully deleted file: " << assetPath << "\n";
                    }
                    else
                    {
                        std::cerr << "[Assets] Failed to delete file: " << assetPath << "\n";
                    }*/

                    // remove texture object
                    m_textureObjects.erase(assetPath);
                }

                // close the asset inspector window
                m_showAssetProperties = false;
                m_selectedAssetPath = "";

            }
            catch (const std::exception& e)
            {
                std::cerr << "[Assets] Error deleting asset: " << e.what() << "\n";

                // close the window
                m_showAssetProperties = false;
                m_selectedAssetPath = "";
            }
        }
    };
}
