#include "GUI/Theme/FontManager.h"
#include <filesystem>

ImFont* FontManager::fontRegular = nullptr;
ImFont* FontManager::fontMedium  = nullptr;
ImFont* FontManager::fontHeading = nullptr;

void FontManager::loadFonts() {
    ImGuiIO& io = ImGui::GetIO();
    const char* candidates[] = {
        "assets/fonts/Roboto-Regular.ttf",
        "assets/fonts/Arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };

    for (const char* path : candidates) {
        if (std::filesystem::exists(path)) {
            fontRegular = io.Fonts->AddFontFromFileTTF(path, 16.5f, nullptr, io.Fonts->GetGlyphRangesVietnamese());
            fontMedium  = io.Fonts->AddFontFromFileTTF(path, 19.0f, nullptr, io.Fonts->GetGlyphRangesVietnamese());
            fontHeading = io.Fonts->AddFontFromFileTTF(path, 24.0f, nullptr, io.Fonts->GetGlyphRangesVietnamese());
            if (fontRegular) break;
        }
    }
}