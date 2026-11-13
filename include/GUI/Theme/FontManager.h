#pragma once
#include "imgui.h"

class FontManager {
private:
    static ImFont* fontRegular; // 16.5px - Chữ nội dung chuẩn
    static ImFont* fontMedium;  // 18.5px - Tiêu đề thẻ Section / Nút lớn
    static ImFont* fontHeading; // 24.0px - Tiêu đề trang / Header

public:
    static void loadFonts();

    static ImFont* regular() { return fontRegular; }
    static ImFont* medium()  { return fontMedium; }
    static ImFont* heading() { return fontHeading; }

    static void pushMedium()  { if (fontMedium) ImGui::PushFont(fontMedium); }
    static void popMedium()   { if (fontMedium) ImGui::PopFont(); }

    static void pushHeading() { if (fontHeading) ImGui::PushFont(fontHeading); }
    static void popHeading()  { if (fontHeading) ImGui::PopFont(); }
};