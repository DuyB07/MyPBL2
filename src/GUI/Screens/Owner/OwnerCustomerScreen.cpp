#include "GUI/Screens/Owner/OwnerCustomerScreen.h"
#include "GUI/Components/UIComponents.h"
#include "GUI/Theme/OrangeLightTheme.h"
#include <GLFW/glfw3.h>
#include <cstring>

OwnerCustomerScreen::OwnerCustomerScreen(AppContext& context)
    : ctx(context), foundSuccess(false), openBlacklistModal(false) {
    std::memset(phoneSearchBuf, 0, sizeof(phoneSearchBuf));
}

void OwnerCustomerScreen::render() {
    if (UI::PageHeader("QUAN LY KHACH HANG & BLACKLIST",
                       "Tra cuu danh ba tuc thi O(1) bang HashTable va kiem soat khach vi pham huy san",
                       "Xem Lich su Blacklist")) {
        openBlacklistModal = true;
    }

    UI::BeginSectionCard("CustomerDirCard", "DANH BA KHACH HANG THANH VIEN");

    // Thanh tra cứu HashTable O(1) căn thẳng hàng Baseline
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Tra cuu nhanh theo SDT (HashTable O(1)):");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(220.0f);
    bool enterSearch = ImGui::InputText("##ph_search", phoneSearchBuf, sizeof(phoneSearchBuf), ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (UI::PrimaryButton("Tim kiem O(1)", ImVec2(130.0f, 0.0f)) || enterSearch) {
        std::string ph = AppContext::sanitize(phoneSearchBuf);
        Customer* found = ctx.custService.getCustomerByPhone(ph);
        if (found) {
            foundSuccess = true;
            foundCustomerBanner = "KET QUA O(1): [" + found->getUserID() + "] " + found->getFullName() +
                                  " | Username: " + found->getUsername() +
                                  " | Da dat: " + std::to_string(found->getBookingCount()) + " lan" +
                                  " | Uu dai: " + std::to_string(found->getDiscountPercent()) + "%";
        } else {
            foundSuccess = false;
            foundCustomerBanner = "Khong tim thay khach hang nao co So dien thoai: " + ph;
        }
    }
    ImGui::SameLine();
    if (UI::SecondaryButton("Lam moi", ImVec2(90.0f, 0.0f))) {
        std::memset(phoneSearchBuf, 0, sizeof(phoneSearchBuf));
        foundCustomerBanner.clear();
    }

    if (!foundCustomerBanner.empty()) {
        ImGui::Spacing();
        ImGui::TextColored(foundSuccess ? Theme::SuccessGreen : Theme::DangerRed, "%s", foundCustomerBanner.c_str());
    }

    ImGui::Spacing();

    if (ImGui::BeginTable("CustomersGridTable", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Ma KH", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Ten Dang Nhap", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Ho va Ten Khach Hang", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("So Dien Thoai", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Luot Dat", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("Luot Huy", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("Muc Uu Dai", ImGuiTableColumnFlags_WidthFixed, 95.0f);
        ImGui::TableSetupColumn("Trang Thai / Thao Tac", ImGuiTableColumnFlags_WidthFixed, 195.0f);
        ImGui::TableHeadersRow();

        for (const auto& c : ctx.custRepo.getAllCustomers()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", c.getUserID().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", c.getUsername().c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", c.getFullName().c_str());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", c.getPhone().c_str());
            ImGui::TableSetColumnIndex(4); ImGui::AlignTextToFramePadding(); ImGui::Text("%d lan", c.getBookingCount());
            ImGui::TableSetColumnIndex(5); ImGui::AlignTextToFramePadding(); ImGui::Text("%d lan", c.getCancelCount());
            ImGui::TableSetColumnIndex(6); ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::PrimaryOrange, "%d%%", c.getDiscountPercent());

            ImGui::TableSetColumnIndex(7);
            if (c.isBlacklisted()) {
                UI::StatusBadge("BLACKLIST");
                ImGui::SameLine(0.0f, 8.0f);
                std::string unbanLbl = "Go cam##ub_" + c.getUserID();
                if (UI::SuccessButton(unbanLbl.c_str(), ImVec2(80.0f, 28.0f))) {
                    if (ctx.custService.unbanCustomer(c.getUserID())) {
                        ctx.saveAllData();
                        ctx.notify("Da go Blacklist va reset bo dem huy cho KH " + c.getFullName(), false, glfwGetTime());
                    }
                }
            } else {
                UI::StatusBadge("HOAT DONG");
            }
        }
        ImGui::EndTable();
    }

    UI::EndSectionCard();

    renderBlacklistHistoryModal();
}

void OwnerCustomerScreen::renderBlacklistHistoryModal() {
    if (openBlacklistModal) {
        ImGui::OpenPopup("Lich Su Ban Ghi Blacklist");
        openBlacklistModal = false;
    }

    UI::CenterNextModal(640.0f, 360.0f);
    if (ImGui::BeginPopupModal("Lich Su Ban Ghi Blacklist", nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::TextColored(Theme::TextMuted, "Danh sach cac ban ghi vi pham huy san >= 3 lan luu tai data/blacklist.txt:");
        ImGui::Spacing();

        if (ImGui::BeginTable("ModalBlacklistTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 230.0f))) {
            ImGui::TableSetupColumn("Ma Ban Ghi", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableSetupColumn("Ma KH", ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableSetupColumn("So Dien Thoai", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableSetupColumn("Trang Thai Hieu Luc", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (const auto& bl : ctx.custRepo.getAllBlacklists()) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", bl.getBlacklistId().c_str());
                ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", bl.getCustomerId().c_str());
                ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", bl.getPhone().c_str());
                ImGui::TableSetColumnIndex(3);
                UI::StatusBadge(bl.isActive() ? "BLACKLIST" : "HOAT DONG");
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 130.0f);
        if (UI::SecondaryButton("Dong cua so", ImVec2(130.0f, 36.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}