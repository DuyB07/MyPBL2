#include "GUI/Theme/OrangeLightTheme.h"

namespace Theme {
    void applyOrangeLightTheme() {
        ImGuiStyle& style = ImGui::GetStyle();

        // Bo góc và khoảng đệm thoáng đãng chuẩn Web Dashboard
        style.WindowPadding     = ImVec2(16.0f, 16.0f);
        style.FramePadding      = ImVec2(12.0f, 8.0f);
        style.CellPadding       = ImVec2(10.0f, 8.0f);
        style.ItemSpacing       = ImVec2(12.0f, 10.0f);
        style.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
        style.ScrollbarSize     = 13.0f;

        style.WindowRounding    = 8.0f;
        style.ChildRounding     = 8.0f;
        style.FrameRounding     = 6.0f;
        style.PopupRounding     = 8.0f;
        style.GrabRounding      = 6.0f;
        style.TabRounding       = 6.0f;

        // Bật viền mảnh cho Khung thẻ và Ô nhập liệu để nổi bật trên nền sáng
        style.WindowBorderSize  = 1.0f;
        style.ChildBorderSize   = 1.0f;
        style.FrameBorderSize   = 1.0f;
        style.PopupBorderSize   = 1.0f;

        ImVec4* c = style.Colors;
        c[ImGuiCol_Text]                 = TextPrimary;
        c[ImGuiCol_TextDisabled]         = TextMuted;
        c[ImGuiCol_WindowBg]             = BgWorkspace;
        c[ImGuiCol_ChildBg]              = BgCard;
        c[ImGuiCol_PopupBg]              = BgCard;
        c[ImGuiCol_Border]               = BorderLight;
        c[ImGuiCol_BorderShadow]         = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        // Ô nhập liệu (InputText, Combo, Slider)
        c[ImGuiCol_FrameBg]              = BgInput;
        c[ImGuiCol_FrameBgHovered]       = SoftOrangeTint;
        c[ImGuiCol_FrameBgActive]        = ImVec4(1.0f, 0.92f, 0.84f, 1.0f);

        // Tiêu đề cửa sổ / Hộp thoại Modal
        c[ImGuiCol_TitleBg]              = PrimaryOrange;
        c[ImGuiCol_TitleBgActive]        = PrimaryOrange;
        c[ImGuiCol_TitleBgCollapsed]     = PrimaryOrange;
        c[ImGuiCol_MenuBarBg]            = BgCard;

        // Thanh cuộn
        c[ImGuiCol_ScrollbarBg]          = BgWorkspace;
        c[ImGuiCol_ScrollbarGrab]        = ImVec4(0.78f, 0.80f, 0.83f, 1.0f);
        c[ImGuiCol_ScrollbarGrabHovered] = PrimaryOrange;
        c[ImGuiCol_ScrollbarGrabActive]  = OrangeActive;

        // Nút bấm mặc định (Màu Cam)
        c[ImGuiCol_CheckMark]            = PrimaryOrange;
        c[ImGuiCol_SliderGrab]           = PrimaryOrange;
        c[ImGuiCol_SliderGrabActive]     = OrangeActive;
        c[ImGuiCol_Button]               = PrimaryOrange;
        c[ImGuiCol_ButtonHovered]        = OrangeHover;
        c[ImGuiCol_ButtonActive]         = OrangeActive;

        // Header bảng & mục chọn
        c[ImGuiCol_Header]               = SoftOrangeTint;
        c[ImGuiCol_HeaderHovered]        = ImVec4(1.0f, 0.88f, 0.75f, 1.0f);
        c[ImGuiCol_HeaderActive]         = ImVec4(1.0f, 0.82f, 0.65f, 1.0f);

        c[ImGuiCol_Separator]            = BorderLight;
        c[ImGuiCol_SeparatorHovered]     = PrimaryOrange;
        c[ImGuiCol_SeparatorActive]      = OrangeActive;

        // Bảng dữ liệu (Tables)
        c[ImGuiCol_TableHeaderBg]        = SoftOrangeTint;
        c[ImGuiCol_TableBorderStrong]    = BorderLight;
        c[ImGuiCol_TableBorderLight]     = ImVec4(0.92f, 0.93f, 0.95f, 1.0f);
        c[ImGuiCol_TableRowBg]           = BgCard;
        c[ImGuiCol_TableRowBgAlt]        = ImVec4(0.985f, 0.988f, 0.992f, 1.0f);

        c[ImGuiCol_TextSelectedBg]       = SoftOrangeTint;
        c[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.10f, 0.12f, 0.15f, 0.45f);
    }
}