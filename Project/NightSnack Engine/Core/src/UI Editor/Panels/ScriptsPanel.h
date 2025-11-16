/******************************************************************************
 * File:        ScriptsPanel.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Drawing of scripts tab
 *
 *
 * Description:
 * Scripts panel for viewing script file contents.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once

namespace fs = std::filesystem;
namespace NightSnack
{
    /**
     * @brief Scripts panel
     */
    class ScriptsPanel {
    public:
        /**
        * @brief constructor
        * @param coord reference to the ECS coordinator
        * @param scriptsRoot path to the scripts file
        */
        ScriptsPanel(Coordinator& coord, const std::string& scriptsRoot)
            : m_coord(coord),
            m_scriptsRoot(scriptsRoot)
        {
        }

        /**
        * @brief selects a script file for preview (called from main assets panel)
        * @param scriptPath full path to the script file
        */
        void SelectScript(const std::string& scriptPath)
        {
            m_selectedFile = fs::path(scriptPath).filename().string();
            LoadFileContents(scriptPath);
        }

        /**
        * @brief gets the contents of a script file
        * @param scriptPath full path to the script file
        * @return the file contents as a string
        */
        std::string GetScriptContents(const std::string& scriptPath)
        {
            LoadFileContents(scriptPath);
            return m_fileContents;
        }

    private:
        Coordinator& m_coord;
        std::string m_scriptsRoot;
        std::string m_selectedFile;
        std::string m_fileContents;

        /**
         * @brief reads txt or cs file and stores its contents
         */
        void LoadFileContents(const std::string& fullPath)
        {
            m_fileContents.clear();

            try
            {
                std::ifstream file(fullPath);
                if (file.is_open())
                {
                    std::string line;
                    while (std::getline(file, line))
                    {
                        m_fileContents += line + "\n";
                    }
                    file.close();
                }
                else
                {
                    m_fileContents = "Error opening file: " + fullPath;
                }
            }
            catch (const std::exception& e)
            {
                m_fileContents = "Error reading file: " + std::string(e.what());
            }
        }
    };
}