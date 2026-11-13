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
    if (UI::PageHeader("TONG QUAN HOAT DONG CUM SAN",
                       "So lieu thong ke thoi gian thuc tu he thong luu tru data/",
                       "Cap nhat Thong tin Cum san")) {
        if (Owner* own = ctx.authService.getPrimaryOwner()) {
            std::snprintf(stadiumNameBuf, sizeof(stadiumNameBuf), "%s", own->getStadiumName().c_str());
            std::snprintf(addressBuf, sizeof(addressBuf), "%s", own->getAddress().c_str());
            std::snprintf(rulesBuf, sizeof(rulesBuf), "%s", own->getFieldRules().c_str());
        }
        openEditStadiumModal = true;
    }

    // 1. Hàng 4 thẻ KPI được tính toán độ rộng khớp tuyệt đối mép phải
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    float availW  = ImGui::GetContentRegionAvail().x;
    float cardW   = (availW - spacing * 3.0f) / 4.0f;

    char revBuf[64];
    std::snprintf(revBuf, sizeof(revBuf), "%.0f VND", ctx.bookingRepo.calculateTotalRevenue());

    int activeFields = 0;
    for (const auto& f : ctx.fieldRepo.getAllFields()) {
        if (f.getStatus() == "AVAILABLE") activeFields++;
    }

    UI::MetricCard("kpi_rev", "TONG DOANH THU THUC THU", revBuf,
                   ("Da thanh toan: " + std::to_string(ctx.bookingRepo.getAllInvoices().size()) + " hoa don").c_str(),
                   true, cardW);
    ImGui::SameLine();
    UI::MetricCard("kpi_fld", "QUY MO SAN BONG",
                   std::to_string(activeFields) + " / " + std::to_string(ctx.fieldRepo.getAllFields().size()) + " san",
                   "Dang mo cua phuc vu", false, cardW);
    ImGui::SameLine();
    UI::MetricCard("kpi_bk", "TONG PHIEU DAT SAN",
                   std::to_string(ctx.bookingRepo.getAllBookings().size()) + " phieu",
                   "Luu tru tai data/bookings.txt", false, cardW);
    ImGui::SameLine();
    UI::MetricCard("kpi_cus", "KHACH HANG THANH VIEN",
                   std::to_string(ctx.custRepo.getAllCustomers().size()) + " khach",
                   ("Giai dau: " + std::to_string(ctx.tourRepo.getAllTournaments().size()) + " giai").c_str(),
                   false, cardW);

    ImGui::Spacing();

    // 2. Khối thông tin cụm sân
    UI::BeginSectionCard("StadiumInfoCard", "THONG TIN VAN HANH CUM SAN", 132.0f);
    if (Owner* own = ctx.authService.getPrimaryOwner()) {
        if (ImGui::BeginTable("StadiumMetaGrid", 2, ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(Theme::TextMuted, "Ten cum san:"); ImGui::SameLine(130.0f);
            ImGui::Text("%s", own->getStadiumName().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(Theme::TextMuted, "Chu so huu:"); ImGui::SameLine(130.0f);
            ImGui::Text("%s (%s)", own->getFullName().c_str(), own->getPhone().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(Theme::TextMuted, "Dia chi:"); ImGui::SameLine(130.0f);
            ImGui::Text("%s", own->getAddress().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(Theme::TextMuted, "Gio hoat dong:"); ImGui::SameLine(130.0f);
            ImGui::Text("%s - %s (Noi quy: %s)", own->getOpenTime().c_str(), own->getCloseTime().c_str(), own->getFieldRules().c_str());
            ImGui::EndTable();
        }
    }
    UI::EndSectionCard();

    ImGui::Spacing();

    // 3. Hai bảng đối xứng: Hóa đơn mới nhất (trái) & Đánh giá của khách hàng (phải)
    float halfW = (availW - spacing) * 0.5f;

    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18.0f, 16.0f));
    ImGui::BeginChild("RecentInvoicesCard", ImVec2(halfW, 0.0f), true);
    ImGui::TextColored(Theme::PrimaryOrange, "HOA DON THANH TOAN GAN DAY");
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::BeginTable("DashInvTable", 4, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Ma HD", ImGuiTableColumnFlags_WidthFixed, 95.0f);
        ImGui::TableSetupColumn("Ma Phieu", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Giam gia", ImGuiTableColumnFlags_WidthFixed, 90.0f);
        ImGui::TableSetupColumn("Thuc thu", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (const auto& inv : ctx.bookingRepo.getAllInvoices()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", inv.getInvoiceId().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", inv.getBookingId().c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding(); ImGui::Text("%.0f VND", inv.getDiscountAmount());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::SuccessGreen, "%.0f VND", inv.getFinalAmount());
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
    ImGui::TextColored(Theme::PrimaryOrange, "DANH GIA CHAT LUONG SAN BONG");
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::BeginTable("DashRevTable", 4, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("San", ImGuiTableColumnFlags_WidthFixed, 55.0f);
        ImGui::TableSetupColumn("Khach", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Diem", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("Nhan xet", ImGuiTableColumnFlags_WidthStretch);
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
        ImGui::OpenPopup("Cap Nhat Thong Tin Cum San");
        openEditStadiumModal = false;
    }

    UI::CenterNextModal(520.0f, 280.0f);
    if (ImGui::BeginPopupModal("Cap Nhat Thong Tin Cum San", nullptr, ImGuiWindowFlags_NoResize)) {
        if (UI::BeginFormTable("EditStadiumForm", 140.0f)) {
            UI::FormRowLabel("Ten cum san:");
            ImGui::InputText("##st_name", stadiumNameBuf, sizeof(stadiumNameBuf));

            UI::FormRowLabel("Dia chi:");
            ImGui::InputText("##st_addr", addressBuf, sizeof(addressBuf));

            UI::FormRowLabel("Noi quy san:");
            ImGui::InputText("##st_rules", rulesBuf, sizeof(rulesBuf));
            UI::EndFormTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float btnW = 140.0f;
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - btnW * 2.0f - 12.0f);
        if (UI::SecondaryButton("Huy bo", ImVec2(btnW, 36.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (UI::PrimaryButton("Luu thay doi", ImVec2(btnW, 36.0f))) {
            if (Owner* own = ctx.authService.getPrimaryOwner()) {
                own->setStadiumName(AppContext::sanitize(stadiumNameBuf));
                own->setAddress(AppContext::sanitize(addressBuf));
                own->setFieldRules(AppContext::sanitize(rulesBuf));
                ctx.saveAllData();
                ctx.notify("Da cap nhat thong tin cum san thanh cong!", false, glfwGetTime());
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}