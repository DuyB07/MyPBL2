#pragma once
#include "imgui.h"

namespace Theme {
    // Bảng màu chủ đạo Trắng - Xám Sáng - Cam (Lấy cảm hứng từ mffms-web)
    constexpr ImVec4 BgWorkspace     = ImVec4(0.957f, 0.965f, 0.976f, 1.00f); // #F4F6F9
    constexpr ImVec4 BgCard          = ImVec4(1.000f, 1.000f, 1.000f, 1.00f); // #FFFFFF
    constexpr ImVec4 BgSidebar       = ImVec4(1.000f, 1.000f, 1.000f, 1.00f); // #FFFFFF
    constexpr ImVec4 BgInput         = ImVec4(0.973f, 0.976f, 0.980f, 1.00f); // #F8F9FA

    constexpr ImVec4 PrimaryOrange   = ImVec4(1.000f, 0.420f, 0.000f, 1.00f); // #FF6B00
    constexpr ImVec4 OrangeHover     = ImVec4(1.000f, 0.522f, 0.200f, 1.00f); // #FF8533
    constexpr ImVec4 OrangeActive    = ImVec4(0.878f, 0.353f, 0.000f, 1.00f); // #E05A00
    constexpr ImVec4 SoftOrangeTint  = ImVec4(1.000f, 0.949f, 0.898f, 1.00f); // #FFF2E5

    constexpr ImVec4 TextPrimary     = ImVec4(0.129f, 0.145f, 0.161f, 1.00f); // #212529
    constexpr ImVec4 TextMuted       = ImVec4(0.424f, 0.459f, 0.490f, 1.00f); // #6C757D
    constexpr ImVec4 TextOnOrange    = ImVec4(1.000f, 1.000f, 1.000f, 1.00f); // #FFFFFF
    constexpr ImVec4 BorderLight     = ImVec4(0.863f, 0.882f, 0.902f, 1.00f); // #DCE1E6

    // Màu ngữ nghĩa cho nhãn trạng thái (Badges)
    constexpr ImVec4 SuccessGreen    = ImVec4(0.063f, 0.620f, 0.373f, 1.00f); // #109E5F
    constexpr ImVec4 SoftGreenBg     = ImVec4(0.902f, 0.976f, 0.937f, 1.00f);
    constexpr ImVec4 DangerRed       = ImVec4(0.898f, 0.224f, 0.208f, 1.00f); // #E53935
    constexpr ImVec4 SoftRedBg       = ImVec4(0.992f, 0.922f, 0.922f, 1.00f);
    constexpr ImVec4 WarningAmber    = ImVec4(0.961f, 0.549f, 0.000f, 1.00f); // #F58C00
    constexpr ImVec4 SoftAmberBg     = ImVec4(1.000f, 0.961f, 0.898f, 1.00f);

    void applyOrangeLightTheme();
}