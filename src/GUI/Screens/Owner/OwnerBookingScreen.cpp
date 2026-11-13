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
    UI::PageHeader("QUAN LY PHIEU DAT SAN & HOA DON",
                   "Xac nhan thanh toan (ap dung uu dai thanh vien), huy phieu va doi soat doanh thu");

    UI::BeginSectionCard("BookingListCard", "DANH SACH PHIEU DAT SAN (data/bookings.txt)", 360.0f);

    // Thanh lọc căn thẳng hàng Baseline
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Trang thai:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(175.0f);
    const char* statusItems[] = { "Tat ca trang thai", "CONFIRMED (Cho TT)", "COMPLETED (Da TT)", "CANCELED (Da huy)" };
    ImGui::Combo("##bk_status_filter", &statusFilterIdx, statusItems, IM_ARRAYSIZE(statusItems));

    ImGui::SameLine(0.0f, 24.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Tim Ma phieu / Ma KH:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(180.0f);
    ImGui::InputText("##bk_search", searchIdBuf, sizeof(searchIdBuf));
    ImGui::SameLine();
    if (UI::SecondaryButton("Xoa loc", ImVec2(85.0f, 0.0f))) {
        statusFilterIdx = 0;
        std::memset(searchIdBuf, 0, sizeof(searchIdBuf));
    }

    ImGui::Spacing();

    std::string keyword = AppContext::toUpper(AppContext::sanitize(searchIdBuf));
    const char* targetStatus = (statusFilterIdx == 1) ? "CONFIRMED"
                             : (statusFilterIdx == 2) ? "COMPLETED"
                             : (statusFilterIdx == 3) ? "CANCELED" : "";

    ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
    if (ImGui::BeginTable("OwnerBookingsTable", 6, flags)) {
        ImGui::TableSetupColumn("Ma Phieu", ImGuiTableColumnFlags_WidthFixed, 85.0f);
        ImGui::TableSetupColumn("Khach Hang", ImGuiTableColumnFlags_WidthFixed, 165.0f);
        ImGui::TableSetupColumn("Ngay Dat San", ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableSetupColumn("Chi Tiet Khung Gio & Don Gia", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Trang Thai", ImGuiTableColumnFlags_WidthFixed, 115.0f);
        ImGui::TableSetupColumn("Thao Tac Nghiep Vu", ImGuiTableColumnFlags_WidthFixed, 200.0f);
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
                slotSummary += d.getSlotId() + " (" + std::to_string(static_cast<int>(d.getPrice())) + " VND)  ";
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", b.getBookingId().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", custDisplay.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", b.getBookingDate().c_str());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", slotSummary.c_str());
            ImGui::TableSetColumnIndex(4); UI::StatusBadge(b.getStatus());

            ImGui::TableSetColumnIndex(5);
            if (b.getStatus() == "CONFIRMED") {
                std::string payLbl = "Thanh toan##p_" + b.getBookingId();
                if (UI::PrimaryButton(payLbl.c_str(), ImVec2(105.0f, 28.0f))) {
                    if (ctx.bookingService.checkoutBooking(b.getBookingId())) {
                        ctx.saveAllData();
                        ctx.notify("Da thanh toan va xuat hoa don cho phieu " + b.getBookingId(), false, glfwGetTime());
                    }
                }
                ImGui::SameLine(0.0f, 6.0f);
                std::string canLbl = "Huy##c_" + b.getBookingId();
                if (UI::DangerButton(canLbl.c_str(), ImVec2(75.0f, 28.0f))) {
                    if (ctx.bookingService.cancelBooking(b.getBookingId(), "Chu san huy theo yeu cau")) {
                        ctx.saveAllData();
                        ctx.notify("Da huy phieu dat san " + b.getBookingId(), false, glfwGetTime());
                    }
                }
            } else {
                ImGui::AlignTextToFramePadding();
                ImGui::TextDisabled("Da hoan tat / Dong");
            }
        }
        ImGui::EndTable();
    }
    UI::EndSectionCard();

    ImGui::Spacing();

    // Bảng Hóa đơn bên dưới
    UI::BeginSectionCard("InvoiceListCard", "DANH SACH HOA DON DA XUAT (data/invoices.txt)");
    if (ImGui::BeginTable("OwnerInvoicesTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Ma Hoa Don", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Ma Phieu Dat", ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableSetupColumn("Tong Tien Goc", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Tien Giam Gia Uu Dai", ImGuiTableColumnFlags_WidthFixed, 160.0f);
        ImGui::TableSetupColumn("So Tien Thuc Thu", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (const auto& inv : ctx.bookingRepo.getAllInvoices()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", inv.getInvoiceId().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", inv.getBookingId().c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding(); ImGui::Text("%.0f VND", inv.getTotalAmount());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding(); ImGui::Text("%.0f VND", inv.getDiscountAmount());
            ImGui::TableSetColumnIndex(4); ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::SuccessGreen, "%.0f VND", inv.getFinalAmount());
        }
        ImGui::EndTable();
    }
    UI::EndSectionCard();
}