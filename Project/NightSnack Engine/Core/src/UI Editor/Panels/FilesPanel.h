/******************************************************************************
 * File:        FilesPanel.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Drawing of files tab
 *
 *
 * Description:
 * Files tab displaying all the files in the assets folder.
 * Note: This is now mainly used as a helper class. The main unified view
 * is handled by AssetsPanel.h
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once

namespace fs = std::filesystem;

namespace NightSnack
{
    /**
     * @brief Files panel
     */
    class FilesPanel {
    public:
        /**
        * @brief constructor
        * @param coord reference to the ECS coordinator
        * @param assetsRoot path to the assets file
        */
        FilesPanel(Coordinator& coord, const std::string& assetsRoot)
            : m_coord(coord),
            m_assetsRoot(assetsRoot)
        {}

    private:
        Coordinator& m_coord;
        std::string m_assetsRoot;

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