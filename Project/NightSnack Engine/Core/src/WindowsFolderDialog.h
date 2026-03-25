/******************************************************************************
 * File:        WindowsFolderDialog.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created WindowsFolderDialog
 *
 * Description:
 * Windows platform-native folder picker dialog. 
 * The implementation of this is in a separate .h file so that <Windows.h> never leaks into engine headers.
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifdef _WIN32 // This will only compile on Windows

// Tell <Windows.h> to exclude rarely-used APIs, to avoid macro conflicts in our engine
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Prevents <Windows.h> from defining min and max macros, which can conflict with std::min and std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>  // HRESULT, HWND etc
#include <ShlObj.h>   // Shell functions: SHCreateItemFromParsingName
#include <ShObjIdl.h> // COM interfaces: IFileDialog, IShellItem
#include <vector>
#include <string>

#pragma comment(lib, "Ole32.lib")   // Links the COM library (CoInitializeEx, CoCreateInstance etc)
#pragma comment(lib, "Shell32.lib") // Links the Shell library (SHCreateItemFromParsingName)

namespace NightSnack {

    /**
     * @brief Opens a native folder picker dialog.
     * @param initialPath The path to start the dialog in (can be empty)
     * @param outPath     On success, receives the selected folder path
     * @param ownerWindow Native Win32 window handle
     * @return true if the user selected a folder, false if cancelled
     */
    inline bool OpenFolderDialog(const std::string& initialPath, std::string& outPath, void* ownerWindow = nullptr) {
        bool result = false;

        // Initialize Component Object Model (COM) before using any COM interfaces like IFileDialog
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
            return false;
        }

        // Create an instance of the Windows file dialog
        // 'CLSID_FileOpenDialog' is the class ID for the "Open" dialog
        // 'IID_IFileDialog' is the interface we want
        IFileDialog* pFileDialog = nullptr;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileDialog, reinterpret_cast<void**>(&pFileDialog));

        if (SUCCEEDED(hr)) {
            // Set the dialog to pick folders only instead of files
            // This is because we want to export a whole folder of assets into a destination folder
            DWORD dwOptions;
            pFileDialog->GetOptions(&dwOptions);
            pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

            // Set dialog title
            // L prefix makes it a wide string (wchar_t*), which is what Windows APIs expect
            pFileDialog->SetTitle(L"Select Export Destination");

            // Try to set initial folder
            if (!initialPath.empty()) {
                // Converts std::string path to a wide string, to know how much memory to allocate
                int wideLen = MultiByteToWideChar(CP_UTF8, 0, initialPath.c_str(), -1, NULL, 0);
                if (wideLen > 0) {

                    std::vector<wchar_t> widePath(wideLen);
                    MultiByteToWideChar(CP_UTF8, 0, initialPath.c_str(), -1, widePath.data(), wideLen);

                    // Converts the path string into an IShellItem (how the Shell represents a folder)
                    IShellItem* pFolder = nullptr;
                    hr = SHCreateItemFromParsingName(widePath.data(), NULL,
                        IID_IShellItem,
                        reinterpret_cast<void**>(&pFolder));

                    if (SUCCEEDED(hr)) {
                        pFileDialog->SetFolder(pFolder);
                        pFolder->Release();
                    }
                }
            }

            // Actually displays the dialog
            // Pass in the GLFW window handle so user cannot click back to the level editor
            // unless they close this dialog first
            hr = pFileDialog->Show(static_cast<HWND>(ownerWindow));

            // If the user clicked 'OK', get the selected item
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem = nullptr;
                hr = pFileDialog->GetResult(&pItem);

                if (SUCCEEDED(hr))
                {
                    // Get the full filesystem path as a string
                    PWSTR pszFilePath = nullptr;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr))
                    {
                        // Convert wide string back to UTF-8 std::string
                        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1,
                            NULL, 0, NULL, NULL);
                        if (utf8Len > 0)
                        {
                            std::vector<char> utf8Path(utf8Len);
                            WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1,
                                utf8Path.data(), utf8Len,
                                NULL, NULL);
                            outPath = std::string(utf8Path.data());
                            result = true;
                        }

                        // The Shell allocated this string for us, must free it
                        CoTaskMemFree(pszFilePath);
                    }
                    // Release COM objects when done
                    pItem->Release();
                }
            }
            // Release COM objects when done
            pFileDialog->Release();
        }

        // Uninitialize COM and return whether a folder was successfully selected
        CoUninitialize();
        return result;
    }

} // NightSnack

#endif // _WIN32