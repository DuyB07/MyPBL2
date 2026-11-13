#include "GUI/Screens/Owner/OwnerDashboardScreen.h"
#include "GUI/Components/UIComponents.h"
#include "GUI/Theme/OrangeLightTheme.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstring>

OwnerDashboardScreen::OwnerDashboardScreen(AppContext& context)
    : ctx(context), openEditStadiumModal(false) {
    std::memset(stadiumNameBuf, 0, sizeof(stadiumNameBuf));
    std::memset(addressBuf, 0, sizeof(addressBuf));
    std::memset(rulesBuf, 0, sizeof(rulesBuf));
}

void OwnerDashboardScreen::render() {
    if (UI::PageHeader("TỔNG QUAN HOẠT ĐỘNG CỤM SÂN",
                       "Số liệu thống kê thời gian thực đồng bộ từ thư mục lưu trữ data/",
                       "Cập nhật Thông tin Cụm sân")) {
        if (Owner* own = ctx.authService.getPrimaryOwner()) {
            std::snprintf(stadiumNameBuf, sizeof(stadiumNameBuf), "%s", own->getStadiumName().c_str());
            std::snprintf(addressBuf, sizeof(addressBuf), "%s", own->getAddress().c_str());
            std::snprintf(rulesBuf, sizeof(rulesBuf), "%s", own->getFieldRules().c_str());
        }
        openEditStadiumModal = true;
    }

    // 1. Hàng 4 thẻ KPI
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    float availW  = ImGui::GetContentRegionAvail().x;
    float cardW   = (availW - spacing * 3.0f) / 4.0f;

    int activeFields = 0;
    for (const auto& f : ctx.fieldRepo.getAllFields()) {
        if (f.getStatus() == "AVAILABLE") activeFields++;
    }

    UI::MetricCard("kpi_rev", "TỔNG DOANH THU THỰC THU",
                   UI::FormatMoney(ctx.bookingRepo.calculateTotalRevenue()),
                   ("Đã xuất: " + std::to_string(ctx.bookingRepo.getAllInvoices().size()) + " hóa đơn").c_str(),
                   true, cardW);
    ImGui::SameLine();
    UI::MetricCard("kpi_fld", "QUY MÔ SÂN BÓNG",
                   std::to_string(activeFields) + " / " + std::to_string(ctx.fieldRepo.getAllFields().size()) + " sân",
                   "Đang mở cửa phục vụ", false, cardW);
    ImGui::SameLine();
    UI::MetricCard("kpi_bk", "TỔNG PHIẾU ĐẶT SÂN",
                   std::to_string(ctx.bookingRepo.getAllBookings().size()) + " phiếu",
                   "Lưu trữ tại data/bookings.txt", false, cardW);
    ImGui::SameLine();
    UI::MetricCard("kpi_cus", "KHÁCH HÀNG THÀNH VIÊN",
                   std::to_string(ctx.custRepo.getAllCustomers().size()) + " khách",
                   ("Giải đấu: " + std::to_string(ctx.tourRepo.getAllTournaments().size()) + " giải").c_str(),
                   false, cardW);

    ImGui::Spacing();

    // 2. Khung Thông tin vận hành cụm sân (Chiều cao 158px + NoScrollbar để không bị cắt chân chữ)
    UI::BeginSectionCard("StadiumInfoCard", "THÔNG TIN VẬN HÀNH CỤM SÂN", 158.0f, ImGuiWindowFlags_NoScrollbar);
    if (Owner* own = ctx.authService.getPrimaryOwner()) {
        if (ImGui::BeginTable("StadiumMetaGrid", 2, ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(Theme::TextMuted, "Tên cụm sân:"); ImGui::SameLine(135.0f);
            ImGui::Text("%s", own->getStadiumName().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(Theme::TextMuted, "Chủ sở hữu:"); ImGui::SameLine(135.0f);
            ImGui::Text("%s (%s)", own->getFullName().c_str(), own->getPhone().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(Theme::TextMuted, "Địa chỉ:"); ImGui::SameLine(135.0f);
            ImGui::Text("%s", own->getAddress().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(Theme::TextMuted, "Giờ mở cửa:"); ImGui::SameLine(135.0f);
            ImGui::Text("%s - %s  |  Nội quy: %s", own->getOpenTime().c_str(), own->getCloseTime().c_str(), own->getFieldRules().c_str());
            ImGui::EndTable();
        }
    }
    UI::EndSectionCard();

    ImGui::Spacing();

    // 3. Hai bảng đối xứng bên dưới
    float halfW = (availW - spacing) * 0.5f;

    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18.0f, 16.0f));
    ImGui::BeginChild("RecentInvoicesCard", ImVec2(halfW, 0.0f), true);
    ImGui::TextColored(Theme::PrimaryOrange, "HÓA ĐƠN THANH TOÁN GẦN ĐÂY");
    ImGui::Separator();
    ImGui::Spacing();

    if (ctx.bookingRepo.getAllInvoices().empty()) {
        UI::RenderEmptyState("Chưa có hóa đơn thanh toán nào.");
    } else if (ImGui::BeginTable("DashInvTable", 4, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Mã HĐ", ImGuiTableColumnFlags_WidthFixed, 95.0f);
        ImGui::TableSetupColumn("Mã Phiếu", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Giảm giá", ImGuiTableColumnFlags_WidthFixed, 105.0f);
        ImGui::TableSetupColumn("Thực thu", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (const auto& inv : ctx.bookingRepo.getAllInvoices()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", inv.getInvoiceId().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", inv.getBookingId().c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", UI::FormatMoney(inv.getDiscountAmount()).c_str());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::SuccessGreen, "%s", UI::FormatMoney(inv.getFinalAmount()).c_str());
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18.0f, 16.0f));
    ImGui::BeginChild("RecentReviewsCard", ImVec2(halfW, 0.0f), true);
    ImGui::TextColored(Theme::PrimaryOrange, "ĐÁNH GIÁ CHẤT LƯỢNG SÂN BÓNG");
    ImGui::Separator();
    ImGui::Spacing();

    if (ctx.custRepo.getAllReviews().empty()) {
        UI::RenderEmptyState("Chưa có đánh giá nào từ khách hàng.");
    } else if (ImGui::BeginTable("DashRevTable", 4, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Sân", ImGuiTableColumnFlags_WidthFixed, 55.0f);
        ImGui::TableSetupColumn("Khách", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Điểm", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Nhận xét của khách", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (const auto& rv : ctx.custRepo.getAllReviews()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", rv.getFieldId().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", rv.getCustomerId().c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::PrimaryOrange, "%d / 5 sao", rv.getRating());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding(); ImGui::TextWrapped("%s", rv.getComment().c_str());
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::EndGroup();

    renderEditStadiumPopup();
}

void OwnerDashboardScreen::renderEditStadiumPopup() {
    if (openEditStadiumModal) {
        ImGui::OpenPopup("Cập Nhật Thông Tin Cụm Sân");
        openEditStadiumModal = false;
    }

    UI::CenterNextModal(540.0f, 285.0f);
    if (ImGui::BeginPopupModal("Cập Nhật Thông Tin Cụm Sân", nullptr, ImGuiWindowFlags_NoResize)) {
        if (UI::BeginFormTable("EditStadiumForm", 145.0f)) {
            UI::FormRowLabel("Tên cụm sân:");
            ImGui::InputText("##st_name", stadiumNameBuf, sizeof(stadiumNameBuf));

            UI::FormRowLabel("Địa chỉ:");
            ImGui::InputText("##st_addr", addressBuf, sizeof(addressBuf));

            UI::FormRowLabel("Nội quy sân:");
            ImGui::InputText("##st_rules", rulesBuf, sizeof(rulesBuf));
            UI::EndFormTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float btnW = 140.0f;
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - btnW * 2.0f - 12.0f);
        if (UI::SecondaryButton("Hủy bỏ", ImVec2(btnW, 36.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (UI::PrimaryButton("Lưu thay đổi", ImVec2(btnW, 36.0f))) {
            if (Owner* own = ctx.authService.getPrimaryOwner()) {
                own->setStadiumName(AppContext::sanitize(stadiumNameBuf));
                own->setAddress(AppContext::sanitize(addressBuf));
                own->setFieldRules(AppContext::sanitize(rulesBuf));
                ctx.saveAllData();
                ctx.notify("Đã cập nhật thông tin cụm sân thành công!", false, glfwGetTime());
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}