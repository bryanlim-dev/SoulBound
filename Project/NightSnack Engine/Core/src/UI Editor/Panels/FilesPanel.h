/******************************************************************************
 * File:        FilesPanel.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Drawing of files tab
 *             
 *
 * Description:
 * Files tab displaying all the files in the assets folder.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once

namespace fs = std::filesystem;
namespace NightSnack
{
    /**
     * @brief Prefabs panel for displaying and managing archetypes from JSON files
     */
    class FilesPanel {
    public:
        /**
        * @brief constructor
        * @param coord reference to the ECS coordinator
        * @param archetypesPath path to the archetypes JSON file
        */
        FilesPanel(Coordinator& coord, const std::string& assetsRoot)
            : m_coord(coord),
            m_assetsRoot(assetsRoot)
        {}

        /**
        * @brief draws the files panel
        */
        void Draw()
        {
            if (ImGui::BeginTabItem("Files"))
            {
                ImGui::Text("Assets Directory: %s", m_assetsRoot.c_str());
                ImGui::Separator();

                try
                {
                    DrawDirectoryRecursive(m_assetsRoot);
                }
                catch (const std::exception& e)
                {
                    ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Error: %s", e.what());
                }

                ImGui::EndTabItem();
            }
        }

    private:
        Coordinator& m_coord;
        std::string m_assetsRoot;

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
                        ImGui::Indent();
                        DrawDirectoryRecursive(path); // recurse into subfolder
						ImGui::Unindent();
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

                    ImGui::Indent();
                    ImGui::BulletText("%s", name.c_str());
                    ImGui::Unindent();
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

    };
}


