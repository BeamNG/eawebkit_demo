/*
The MIT License

Copyright (c) 2017 BeamNG GmbH. https://github.com/BeamNG/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "bngUtils.h"

#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebkitAllocator.h>
#include <EAWebKit/EAWebKitFileSystem.h>
#include <EAWebKit/EAWebKitClient.h>
#include <EAWebKit/EAWebKitView.h>
#include <EAText/EAText.h>

#include <DirectXMath.h>

#include <stdio.h>

#include <windows.h> // LoadLibraryA
#include <assert.h>
#include <array>

#include <Shlwapi.h>
#include "EAWebkit/EAWebKitTextInterface.h"
#include <vector>
#pragma comment(lib, "shlwapi.lib")

std::string BeamNG::Utils::getExePath() {
    char path[MAX_PATH] = "";
    DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
    PathRemoveFileSpecA(path);
    return std::string(path);
}

std::string BeamNG::Utils::replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

int BeamNG::Utils::getSystemFonts(std::vector<std::string>& fonts) {
    static const LPWSTR fontRegistryPath = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
    HKEY hKey;
    LONG result;

    // Open Windows font registry key
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        return 1;
    }

    DWORD maxValueNameSize = 0, maxValueDataSize = 0;
    result = RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0);
    if (result != ERROR_SUCCESS) {
        return 1;
    }

    DWORD valueIndex = 0;
    LPSTR valueName = new CHAR[maxValueNameSize];
    LPBYTE valueData = new BYTE[maxValueDataSize];
    DWORD valueNameSize, valueDataSize, valueType;


    // Build full font file path
    char winDir_[MAX_PATH] = "";
    GetWindowsDirectoryA(winDir_, MAX_PATH);
    std::string winDir = std::string(winDir_);
    fonts.clear();

    do {
        valueDataSize = maxValueDataSize;
        valueNameSize = maxValueNameSize;

        result = RegEnumValueA(hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize);

        valueIndex++;

        if (result != ERROR_SUCCESS || valueType != REG_SZ) {
            continue;
        }

        std::string wsValueName(valueName, valueNameSize);
        std::string wsValueData((LPSTR)valueData, valueDataSize);

        std::string fontPath = winDir + "\\Fonts\\" + wsValueData;
        fonts.push_back(fontPath);

    } while (result != ERROR_NO_MORE_ITEMS);

    delete[] valueName;
    delete[] valueData;

    RegCloseKey(hKey);
    return 0;
}

int BeamNG::Utils::add_ttf_font(EA::WebKit::EAWebKitLib* wk, const char* ttfFile) {
    EA::WebKit::ITextSystem* ts = wk->GetTextSystem();
    FILE* f = 0;
    fopen_s(&f, ttfFile, "rb");
    if (!f) return 1;
    fseek(f, 0L, SEEK_END);
    size_t fileSize = ftell(f);
    fseek(f, 0L, SEEK_SET);
    char* buffer = (char*)calloc(fileSize + 6, 1);
    size_t read_bytes = fread(buffer, 1, fileSize, f);
    if (read_bytes != fileSize) {
        // error!
        free(buffer);
        return 0;
    }
    int res = ts->AddFace(buffer, fileSize);
    free(buffer);
    return res;
}

int BeamNG::Utils::init_system_fonts(EA::WebKit::EAWebKitLib* wk) {
    std::vector<std::string> fonts;
    if (getSystemFonts(fonts)) {
        return 1;
    }
    int fonts_installed = 0;
    for (int i = 0; i < fonts.size(); ++i) {
        add_ttf_font(wk, fonts[i].c_str());
    }
    return 0;
}
