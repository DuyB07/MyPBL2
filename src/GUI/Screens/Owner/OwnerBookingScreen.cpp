#include "GUI/Screens/Owner/OwnerBookingScreen.h"
#include "GUI/Components/UIComponents.h"
#include "GUI/Theme/OrangeLightTheme.h"
#include <GLFW/glfw3.h>
#include <cstring>

OwnerBookingScreen::OwnerBookingScreen(AppContext& context)
    : ctx(context), statusFilterIdx(0) {
    std::memset(searchIdBuf, 0, sizeof(searchIdBuf));
}

void OwnerBookingScreen::render() {
    UI::PageHeader("QUẢN LÝ PHIẾU ĐẶT SÂN & HÓA ĐƠN",
                   "Xác nhận thanh toán (áp dụng ưu đãi thành viên), hủy phiếu và đối soát doanh thu");

    float availH = ImGui::GetContentRegionAvail().y;
    float topCardH = availH * 0.52f;
    if (topCardH < 260.0f) topCardH = 260.0f;

    UI::BeginSectionCard("BookingListCard", "DANH SÁCH PHIẾU ĐẶT SÂN (data/bookings.txt)", topCardH);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Trạng thái:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(235.0f); // Đủ rộng để hiển thị trọn chữ "Chờ thanh toán (CONFIRMED)"
    const char* statusItems[] = { "Tất cả trạng thái", "Chờ thanh toán (CONFIRMED)", "Đã hoàn tất (COMPLETED)", "Đã hủy (CANCELED)" };
    ImGui::Combo("##bk_status_filter", &statusFilterIdx, statusItems, IM_ARRAYSIZE(statusItems));

    ImGui::SameLine(0.0f, 24.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Tìm Mã phiếu / Mã KH:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(180.0f);
    ImGui::InputText("##bk_search", searchIdBuf, sizeof(searchIdBuf));
    ImGui::SameLine();
    if (UI::SecondaryButton("Xóa lọc", ImVec2(90.0f, 0.0f))) {
        statusFilterIdx = 0;
        std::memset(searchIdBuf, 0, sizeof(searchIdBuf));
    }

    ImGui::Spacing();

    std::string keyword = AppContext::toUpper(AppContext::sanitize(searchIdBuf));
    const char* targetStatus = (statusFilterIdx == 1) ? "CONFIRMED"
                             : (statusFilterIdx == 2) ? "COMPLETED"
                             : (statusFilterIdx == 3) ? "CANCELED" : "";

    // Đếm số bản ghi khớp với bộ lọc trước khi vẽ bảng
    int matchCount = 0;
    for (const auto& b : ctx.bookingRepo.getAllBookings()) {
        if (targetStatus[0] != '\0' && b.getStatus() != targetStatus) continue;
        if (!keyword.empty() &&
            AppContext::toUpper(b.getBookingId()).find(keyword) == std::string::npos &&
            AppContext::toUpper(b.getCustomerId()).find(keyword) == std::string::npos) {
            continue;
        }
        matchCount++;
    }

    if (matchCount == 0) {
        UI::RenderEmptyState("Không có phiếu đặt sân nào khớp với bộ lọc hiện tại.");
    } else if (ImGui::BeginTable("OwnerBookingsTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Mã Phiếu", ImGuiTableColumnFlags_WidthFixed, 85.0f);
        ImGui::TableSetupColumn("Khách Hàng", ImGuiTableColumnFlags_WidthFixed, 175.0f);
        ImGui::TableSetupColumn("Ngày Đặt Sân", ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableSetupColumn("Chi Tiết Khung Giờ & Đơn Giá", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Trạng Thái", ImGuiTableColumnFlags_WidthFixed, 135.0f);
        ImGui::TableSetupColumn("Thao Tác Nghiệp Vụ", ImGuiTableColumnFlags_WidthFixed, 195.0f);
        ImGui::TableHeadersRow();

        for (const auto& b : ctx.bookingRepo.getAllBookings()) {
            if (targetStatus[0] != '\0' && b.getStatus() != targetStatus) continue;
            if (!keyword.empty() &&
                AppContext::toUpper(b.getBookingId()).find(keyword) == std::string::npos &&
                AppContext::toUpper(b.getCustomerId()).find(keyword) == std::string::npos) {
                continue;
            }

            Customer* cust = ctx.custService.getCustomerById(b.getCustomerId());
            std::string custDisplay = cust ? (b.getCustomerId() + " - " + cust->getFullName()) : b.getCustomerId();

            std::string slotSummary;
            for (const auto& d : ctx.bookingRepo.getDetailsByBookingId(b.getBookingId())) {
                slotSummary += d.getSlotId() + " (" + UI::FormatMoney(d.getPrice()) + ")  ";
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", b.getBookingId().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", custDisplay.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", b.getBookingDate().c_str());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", slotSummary.c_str());
            ImGui::TableSetColumnIndex(4); UI::StatusBadge(b.getStatus());

            ImGui::TableSetColumnIndex(5);
            if (b.getStatus() == "CONFIRMED") {
                std::string payLbl = "Thanh toán##p_" + b.getBookingId();
                if (UI::PrimaryButton(payLbl.c_str(), ImVec2(102.0f, 28.0f))) {
                    if (ctx.bookingService.checkoutBooking(b.getBookingId())) {
                        ctx.saveAllData();
                        ctx.notify("Đã thanh toán và xuất hóa đơn cho phiếu " + b.getBookingId(), false, glfwGetTime());
                    }
                }
                ImGui::SameLine(0.0f, 6.0f);
                std::string canLbl = "Hủy##c_" + b.getBookingId();
                if (UI::DangerButton(canLbl.c_str(), ImVec2(72.0f, 28.0f))) {
                    if (ctx.bookingService.cancelBooking(b.getBookingId(), "Chủ sân hủy theo yêu cầu")) {
                        ctx.saveAllData();
                        ctx.notify("Đã hủy phiếu đặt sân " + b.getBookingId(), false, glfwGetTime());
                    }
                }
            } else {
                ImGui::AlignTextToFramePadding();
                ImGui::TextDisabled("Đã đóng");
            }
        }
        ImGui::EndTable();
    }
    UI::EndSectionCard();

    ImGui::Spacing();

    UI::BeginSectionCard("InvoiceListCard", "DANH SÁCH HÓA ĐƠN ĐÃ XUẤT (data/invoices.txt)");
    if (ctx.bookingRepo.getAllInvoices().empty()) {
        UI::RenderEmptyState("Chưa có hóa đơn nào được xuất.");
    } else if (ImGui::BeginTable("OwnerInvoicesTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Mã Hóa Đơn", ImGuiTableColumnFlags_WidthFixed, 125.0f);
        ImGui::TableSetupColumn("Mã Phiếu Đặt", ImGuiTableColumnFlags_WidthFixed, 115.0f);
        ImGui::TableSetupColumn("Tổng Tiền Gốc", ImGuiTableColumnFlags_WidthFixed, 155.0f);
        ImGui::TableSetupColumn("Tiền Giảm Giá Ưu Đãi", ImGuiTableColumnFlags_WidthFixed, 165.0f);
        ImGui::TableSetupColumn("Số Tiền Thực Thu", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (const auto& inv : ctx.bookingRepo.getAllInvoices()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", inv.getInvoiceId().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", inv.getBookingId().c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", UI::FormatMoney(inv.getTotalAmount()).c_str());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", UI::FormatMoney(inv.getDiscountAmount()).c_str());
            ImGui::TableSetColumnIndex(4); ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::SuccessGreen, "%s", UI::FormatMoney(inv.getFinalAmount()).c_str());
        }
        ImGui::EndTable();
    }
    UI::EndSectionCard();
}