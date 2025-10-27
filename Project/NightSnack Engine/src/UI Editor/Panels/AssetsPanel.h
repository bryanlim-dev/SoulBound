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
            m_showAssetProperties(false)
        {}

        void Draw()
        {
			// Set initial window size and style
            //ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_Once);

			// Options Window
            ImGui::Begin("Assets");

            if (ImGui::BeginTabBar("AssetsTabs"))
            {
                // Code here
                // Currently, we only have one tab: "Images"
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
                ImGui::EndTabBar();
            }
            ImGui::End();
        }

    private:
        Coordinator& m_coord;
        fs::path m_assetsRoot;// Root directory of the assets folder

        std::unordered_map<std::string, GLuint> m_loadedTextures; // Cache of loaded textures
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureObjects;

        float m_thumbnailSize;                   // Size of thumbnail images
        std::string m_searchFilter;              // Current search text
        std::string m_selectedAssetPath;         // Currently selected asset
        bool m_showAssetProperties;              // Show properties window?

        void DrawToolbar()
        {
            // ----------------------------------------------------------------
            // SEARCH BAR - Filter assets by name (Unity has this)
            // ----------------------------------------------------------------
            static char searchBuffer[256] = "";
            ImGui::SetNextItemWidth(200.0f);
            if (ImGui::InputTextWithHint("##Search", "Search assets...", searchBuffer, 256))
            {
                m_searchFilter = std::string(searchBuffer);
            }

            ImGui::SameLine();

            // ----------------------------------------------------------------
            // THUMBNAIL SIZE SLIDER - Adjust icon size (Unity feature)
            // ----------------------------------------------------------------
            ImGui::SetNextItemWidth(100.0f);
            ImGui::SliderFloat("##Size", &m_thumbnailSize, 64.0f, 128.0f, "Size: %.0f");

            ImGui::SameLine();

            // ----------------------------------------------------------------
            // REFRESH BUTTON - Rescan assets folder
            // ----------------------------------------------------------------
            if (ImGui::Button("Refresh"))
            {
                // Clear texture cache to force reload
                m_loadedTextures.clear();
                m_textureObjects.clear();
                std::cout << "[Assets] Refreshed asset cache\n";
            }
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
                    ImVec2(m_thumbnailSize, m_thumbnailSize));

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
                    std::cout << "[Assets] Selected: " << pathStr << "\n";
                }

                // ================================================================
                // DRAG SOURCE - ENABLE DRAGGING (MOST IMPORTANT FEATURE!)
                // This allows dragging the image to Scene panel
                // ================================================================
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    // ----------------------------------------------------------------
                    // SET PAYLOAD - Store the asset path as drag data
                    // "ASSET_IMAGE" is the identifier for this drag type
                    // ----------------------------------------------------------------
                    ImGui::SetDragDropPayload("ASSET_IMAGE", pathStr.c_str(), pathStr.size() + 1);

                    // ----------------------------------------------------------------
                    // DRAG PREVIEW - Show thumbnail while dragging
                    // ----------------------------------------------------------------
                    ImGui::Image(texID, ImVec2(64, 64));
                    ImGui::Text("%s", entry.path().filename().string().c_str());

                    ImGui::EndDragDropSource();
                }

                // ----------------------------------------------------------------
                // TOOLTIP - Show info on hover
                // ----------------------------------------------------------------
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

        /**********************************************************************
         * Function: DrawAllFilesTab()
         * Purpose : Displays *all* files (any type) inside the assets folder.
         **********************************************************************/
        void DrawAllFilesTab()
        {
            // Use a tree view to show folders & files
            DrawDirectoryRecursive(m_assetsRoot);
        }

        /**********************************************************************
         * Function: DrawDirectoryRecursive()
         * Purpose : Recursively displays folders and their files as a tree.
         **********************************************************************/
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


        /**********************************************************************
        * Function: IsImageFile()
        * Purpose : Checks if a file has an image extension we support.
        **********************************************************************/
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

            stbi_set_flip_vertically_on_load(false);

            // --- Use your Texture class to load the image --- //
            std::shared_ptr<Texture> tex = std::make_shared<Texture>(path); 

            // Cache the texture (store both Texture and ImGui ID)
            m_textureObjects[path] = tex;
            m_loadedTextures[path] = (ImTextureID)(intptr_t)tex->GetID();

            return m_loadedTextures[path];
        }
    };
}
