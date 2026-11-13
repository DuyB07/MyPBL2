#include "GUI/Components/UIComponents.h"
#include "GUI/Theme/OrangeLightTheme.h"
#include "GUI/Theme/FontManager.h"

namespace UI {

    void BeginSectionCard(const char* id, const char* title, float height) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 18.0f));
        ImGui::BeginChild(id, ImVec2(0.0f, height), true);

        if (title && title[0] != '\0') {
            FontManager::pushMedium();
            ImGui::TextColored(Theme::PrimaryOrange, "%s", title);
            FontManager::popMedium();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }
    }

    void EndSectionCard() {
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    bool PageHeader(const char* pageTitle, const char* subtitle, const char* rightActionBtnLabel) {
        bool clicked = false;
        float startY = ImGui::GetCursorPosY();

        ImGui::BeginGroup();
        FontManager::pushHeading();
        ImGui::TextColored(Theme::TextPrimary, "%s", pageTitle);
        FontManager::popHeading();
        if (subtitle && subtitle[0] != '\0') {
            ImGui::TextColored(Theme::TextMuted, "%s", subtitle);
        }
        ImGui::EndGroup();

        if (rightActionBtnLabel && rightActionBtnLabel[0] != '\0') {
            float btnWidth = ImGui::CalcTextSize(rightActionBtnLabel).x + 36.0f;
            float rightX = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - btnWidth;
            ImGui::SameLine(rightX);
            ImGui::SetCursorPosY(startY + 4.0f);
            clicked = PrimaryButton(rightActionBtnLabel, ImVec2(btnWidth, 38.0f));
        }

        ImGui::Spacing();
        return clicked;
    }

    void CenterNextModal(float width, float height) {
        const ImGuiViewport* vp = ImGui::GetMainViewport();
        ImVec2 center = vp->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Appearing);
    }

    bool BeginFormTable(const char* id, float labelColWidth) {
        if (ImGui::BeginTable(id, 2, ImGuiTableFlags_SizingFixedFit)) {
            ImGui::TableSetupColumn("LabelCol", ImGuiTableColumnFlags_WidthFixed, labelColWidth);
            ImGui::TableSetupColumn("InputCol", ImGuiTableColumnFlags_WidthStretch);
            return true;
        }
        return false;
    }

    void FormRowLabel(const char* label) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(Theme::TextPrimary, "%s", label);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1.0f);
    }

    void EndFormTable() {
        ImGui::EndTable();
    }

    bool PrimaryButton(const char* label, const ImVec2& size) {
        ImGui::PushStyleColor(ImGuiCol_Button, Theme::PrimaryOrange);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::OrangeHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Theme::OrangeActive);
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextOnOrange);
        bool res = ImGui::Button(label, size);
        ImGui::PopStyleColor(4);
        return res;
    }

    bool SecondaryButton(const char* label, const ImVec2& size) {
        ImGui::PushStyleColor(ImGuiCol_Button, Theme::BgInput);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::SoftOrangeTint);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Theme::BorderLight);
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextPrimary);
        bool res = ImGui::Button(label, size);
        ImGui::PopStyleColor(4);
        return res;
    }

    bool DangerButton(const char* label, const ImVec2& size) {
        ImGui::PushStyleColor(ImGuiCol_Button, Theme::DangerRed);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.32f, 0.30f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.78f, 0.16f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextOnOrange);
        bool res = ImGui::Button(label, size);
        ImGui::PopStyleColor(4);
        return res;
    }

    bool SuccessButton(const char* label, const ImVec2& size) {
        ImGui::PushStyleColor(ImGuiCol_Button, Theme::SuccessGreen);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.10f, 0.72f, 0.44f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.52f, 0.30f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextOnOrange);
        bool res = ImGui::Button(label, size);
        ImGui::PopStyleColor(4);
        return res;
    }

    void StatusBadge(const std::string& statusText) {
        ImGui::AlignTextToFramePadding();
        ImVec4 fg = Theme::TextPrimary;
        if (statusText == "AVAILABLE" || statusText == "COMPLETED" ||
            statusText == "APPROVED" || statusText == "HOAT DONG" || statusText == "CON TRONG") {
            fg = Theme::SuccessGreen;
        } else if (statusText == "CANCELED" || statusText == "REJECTED" ||
                   statusText == "BLACKLIST" || statusText == "MAINTENANCE" || statusText == "DA DAT") {
            fg = Theme::DangerRed;
        } else if (statusText == "CONFIRMED" || statusText == "PENDING") {
            fg = Theme::PrimaryOrange;
        }
        ImGui::TextColored(fg, "[%s]", statusText.c_str());
    }

    void MetricCard(const char* id, const char* title, const std::string& value, const char* caption, bool highlightOrange, float width) {
        ImVec4 bg = highlightOrange ? Theme::PrimaryOrange : Theme::BgCard;
        ImVec4 titleCol = highlightOrange ? ImVec4(1.0f, 0.92f, 0.85f, 1.0f) : Theme::TextMuted;
        ImVec4 valCol   = highlightOrange ? Theme::TextOnOrange : Theme::PrimaryOrange;
        ImVec4 capCol   = highlightOrange ? ImVec4(1.0f, 0.90f, 0.80f, 1.0f) : Theme::TextMuted;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18.0f, 14.0f));
        ImGui::BeginChild(id, ImVec2(width, 108.0f), true, ImGuiWindowFlags_NoScrollbar);

        ImGui::TextColored(titleCol, "%s", title);
        FontManager::pushHeading();
        ImGui::TextColored(valCol, "%s", value.c_str());
        FontManager::popHeading();
        if (caption && caption[0] != '\0') {
            ImGui::TextColored(capCol, "%s", caption);
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
}