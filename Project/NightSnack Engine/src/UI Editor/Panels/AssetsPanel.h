/******************************************************************************
 * File:        AssetsPanel.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 90%)
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
        AssetsPanel(Coordinator& coord) : m_coord(coord) , m_assetsRoot("../../NightSnack Engine/assets") {}

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


        void DrawImagesTab()
        {
            // Define thumbnail appearance
            static float thumbnailSize = 96.0f;   // Size of each image preview square
            static float padding = 12.0f;         // Space between thumbnails
            float cellSize = thumbnailSize + padding;

            // Compute how many thumbnails fit per row based on available panel width
            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = (int)(panelWidth / cellSize);
            if (columnCount < 1)
            {
                columnCount = 1;
            }

            // Create columns to layout thumbnails in a grid
            ImGui::Columns(columnCount, 0, false);

            // Iterate through all files inside the assets folder (recursively)
            for (auto const& entry : fs::recursive_directory_iterator(m_assetsRoot))
            {
                // Skip folders; we only want image files
                if (!entry.is_regular_file())
                {
                    continue;
                }

                // Get file extension (e.g., ".png", ".jpg")
                std::string ext = entry.path().extension().string();
                if (!IsImageFile(ext))
                {
                    continue; // Skip non-image files
                }

                // Convert full path to string
                std::string pathStr = entry.path().string();

                // Load the texture (cached if already loaded)
                ImTextureID texID = GetOrLoadTexture(pathStr);
                if (!texID)
                {
                    continue; // Skip if texture loading failed or not implemented yet
                }

                // Each image has its own ImGui ID to avoid conflicts
                ImGui::PushID(pathStr.c_str());

                // Render the image as a clickable button (thumbnail)
                ImGui::ImageButton(pathStr.c_str(), texID, ImVec2(thumbnailSize, thumbnailSize));

                // If hovered, show a tooltip with the file name
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", entry.path().filename().string().c_str());
                    ImGui::EndTooltip();
                }

                // Display filename text below the image
                ImGui::TextWrapped("%s", entry.path().filename().string().c_str());

                // Move to the next column in the grid layout
                ImGui::NextColumn();

                ImGui::PopID();
            }

            // End the columns layout
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
