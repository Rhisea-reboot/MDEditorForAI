#ifndef FOLDERSELECTOR_H
#define FOLDERSELECTOR_H

#include <string>
#include <windows.h>
#include <shobjidl.h> 
#include <sstream>

// CLSID_FileOpenDialog: {DC1C5A9C-E88A-4dde-A5A1-60F82A20AEF7}
static const CLSID CLSID_FileOpenDialog_MY = {0xdc1c5a9c, 0xe88a, 0x4dde, {0xa5, 0xa1, 0x60, 0xf8, 0x2a, 0x20, 0xae, 0xf7}};

// IID_IFileOpenDialog: {42f85136-db7e-439c-85f1-e4075d135fc8}
static const IID IID_IFileOpenDialog_MY = {0x42f85136, 0xdb7e, 0x439c, {0x85, 0xf1, 0xe4, 0x07, 0x5d, 0x13, 0x5f, 0xc8}};

namespace WindowsUtils {

    inline std::string wideToUtf8(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }

    inline std::string selectFolder() {
        std::string selectedPath = "";
        
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr)) return "";

        IFileOpenDialog* pFileOpen;
        
        // [修改处] 使用我们上面手动定义的变量 CLSID_FileOpenDialog_MY 和 IID_IFileOpenDialog_MY
        hr = CoCreateInstance(CLSID_FileOpenDialog_MY, NULL, CLSCTX_ALL, IID_IFileOpenDialog_MY, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr)) {
            DWORD dwOptions;
            if (SUCCEEDED(pFileOpen->GetOptions(&dwOptions))) {
                pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
            }

            hr = pFileOpen->Show(NULL);

            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr)) {
                        selectedPath = wideToUtf8(pszFilePath);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        
        CoUninitialize();
        return selectedPath;
    }
}

#endif