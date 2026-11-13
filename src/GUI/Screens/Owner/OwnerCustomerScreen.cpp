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
    if (UI::PageHeader("QUẢN LÝ KHÁCH HÀNG & DANH SÁCH ĐEN (BLACKLIST)",
                       "Tra cứu danh bạ tức thì O(1) bằng HashTable và kiểm soát khách vi phạm hủy sân",
                       "Xem Lịch sử Blacklist")) {
        openBlacklistModal = true;
    }

    UI::BeginSectionCard("CustomerDirCard", "DANH BẠ KHÁCH HÀNG THÀNH VIÊN");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Tra cứu nhanh theo SĐT (HashTable O(1)):");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(220.0f);
    bool enterSearch = ImGui::InputText("##ph_search", phoneSearchBuf, sizeof(phoneSearchBuf), ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (UI::PrimaryButton("Tìm kiếm O(1)", ImVec2(130.0f, 0.0f)) || enterSearch) {
        std::string ph = AppContext::sanitize(phoneSearchBuf);
        Customer* found = ctx.custService.getCustomerByPhone(ph);
        if (found) {
            foundSuccess = true;
            foundCustomerBanner = "KẾT QUẢ O(1): [" + found->getUserID() + "] " + found->getFullName() +
                                  " | Tài khoản: " + found->getUsername() +
                                  " | Đã đặt: " + std::to_string(found->getBookingCount()) + " lần" +
                                  " | Ưu đãi: " + std::to_string(found->getDiscountPercent()) + "%";
        } else {
            foundSuccess = false;
            foundCustomerBanner = "Không tìm thấy khách hàng nào có Số điện thoại: " + ph;
        }
    }
    ImGui::SameLine();
    if (UI::SecondaryButton("Làm mới", ImVec2(90.0f, 0.0f))) {
        std::memset(phoneSearchBuf, 0, sizeof(phoneSearchBuf));
        foundCustomerBanner.clear();
    }

    if (!foundCustomerBanner.empty()) {
        ImGui::Spacing();
        ImGui::TextColored(foundSuccess ? Theme::SuccessGreen : Theme::DangerRed, "%s", foundCustomerBanner.c_str());
    }

    ImGui::Spacing();

    if (ctx.custRepo.getAllCustomers().empty()) {
        UI::RenderEmptyState("Chưa có tài khoản khách hàng nào.");
    } else if (ImGui::BeginTable("CustomersGridTable", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Mã KH", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Tên Đăng Nhập", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Họ và Tên Khách Hàng", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Số Điện Thoại", ImGuiTableColumnFlags_WidthFixed, 125.0f);
        ImGui::TableSetupColumn("Lượt Đặt", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Lượt Hủy", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Mức Ưu Đãi", ImGuiTableColumnFlags_WidthFixed, 95.0f);
        ImGui::TableSetupColumn("Trạng Thái / Thao Tác", ImGuiTableColumnFlags_WidthFixed, 195.0f);
        ImGui::TableHeadersRow();

        for (const auto& c : ctx.custRepo.getAllCustomers()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", c.getUserID().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", c.getUsername().c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", c.getFullName().c_str());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", c.getPhone().c_str());
            ImGui::TableSetColumnIndex(4); ImGui::AlignTextToFramePadding(); ImGui::Text("%d lần", c.getBookingCount());
            ImGui::TableSetColumnIndex(5); ImGui::AlignTextToFramePadding(); ImGui::Text("%d lần", c.getCancelCount());
            ImGui::TableSetColumnIndex(6); ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::PrimaryOrange, "%d%%", c.getDiscountPercent());

            ImGui::TableSetColumnIndex(7);
            if (c.isBlacklisted()) {
                UI::StatusBadge("BLACKLIST");
                ImGui::SameLine(0.0f, 8.0f);
                std::string unbanLbl = "Gỡ cấm##ub_" + c.getUserID();
                if (UI::SuccessButton(unbanLbl.c_str(), ImVec2(80.0f, 28.0f))) {
                    if (ctx.custService.unbanCustomer(c.getUserID())) {
                        ctx.saveAllData();
                        ctx.notify("Đã gỡ Blacklist và đặt lại bộ đếm hủy cho KH " + c.getFullName(), false, glfwGetTime());
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
        ImGui::OpenPopup("Lịch Sử Bản Ghi Blacklist");
        openBlacklistModal = false;
    }

    UI::CenterNextModal(650.0f, 360.0f);
    if (ImGui::BeginPopupModal("Lịch Sử Bản Ghi Blacklist", nullptr,
                               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
        ImGui::TextColored(Theme::TextMuted, "Danh sách các bản ghi vi phạm hủy sân >= 3 lần lưu tại data/blacklist.txt:");
        ImGui::Spacing();

        ImGui::BeginChild("BlacklistModalBody", ImVec2(0.0f, 220.0f), false);
        if (ctx.custRepo.getAllBlacklists().empty()) {
            ImGui::SetCursorPosY(85.0f);
            UI::RenderEmptyState("Hiện chưa có khách hàng nào vi phạm nằm trong Blacklist.");
        } else if (ImGui::BeginTable("ModalBlacklistTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Mã Bản Ghi", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableSetupColumn("Mã KH", ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableSetupColumn("Số Điện Thoại", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableSetupColumn("Trạng Thái Hiệu Lực", ImGuiTableColumnFlags_WidthStretch);
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
        ImGui::EndChild();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 135.0f);
        if (UI::SecondaryButton("Đóng cửa sổ", ImVec2(135.0f, 36.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}