#include "GUI/Screens/Owner/OwnerFieldScreen.h"
#include "GUI/Components/UIComponents.h"
#include "GUI/Theme/OrangeLightTheme.h"
#include "Core/DateTime.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstring>

OwnerFieldScreen::OwnerFieldScreen(AppContext& context)
    : ctx(context),
      filterTypeIdx(0),
      openCreateModal(false),
      openDetailModal(false),
      newTypeIdx(0),
      newPrice(200000.0),
      editPrice(200000.0) {
    std::string today = DateTime::todayDate();
    std::snprintf(checkDateBuf, sizeof(checkDateBuf), "%s", today.c_str());
    std::snprintf(newIdBuf, sizeof(newIdBuf), "F04");
    std::snprintf(newNameBuf, sizeof(newNameBuf), "Sân D (Cỏ Nhân Tạo)");
}

void OwnerFieldScreen::render() {
    if (UI::PageHeader("QUẢN LÝ DANH SÁCH SÂN BÓNG",
                       "Quản lý trạng thái vận hành, đơn giá và sơ đồ khung giờ thuê của từng sân",
                       "+ Thêm sân bóng mới")) {
        int nextIdx = static_cast<int>(ctx.fieldRepo.getAllFields().size()) + 1;
        std::snprintf(newIdBuf, sizeof(newIdBuf), "F0%d", nextIdx);
        openCreateModal = true;
    }

    UI::BeginSectionCard("FieldListSection", "DANH SÁCH SÂN BÓNG HIỆN CÓ");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Lọc theo loại sân:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(170.0f);
    const char* filterItems[] = { "Tất cả loại sân", "Sân 5 người", "Sân 7 người", "Sân 11 người" };
    ImGui::Combo("##filter_type", &filterTypeIdx, filterItems, IM_ARRAYSIZE(filterItems));

    ImGui::SameLine(0.0f, 24.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Ngày kiểm tra ca trống:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(135.0f);
    ImGui::InputText("##chk_date", checkDateBuf, sizeof(checkDateBuf));
    ImGui::SameLine();
    if (UI::SecondaryButton("Hôm nay", ImVec2(95.0f, 0.0f))) {
        std::string today = DateTime::todayDate();
        std::snprintf(checkDateBuf, sizeof(checkDateBuf), "%s", today.c_str());
    }

    ImGui::Spacing();

    std::string dateStr = AppContext::sanitize(checkDateBuf);
    if (!DateTime::isValidDate(dateStr)) dateStr = DateTime::todayDate();

    int filterTypeVal = (filterTypeIdx == 1) ? 5 : ((filterTypeIdx == 2) ? 7 : ((filterTypeIdx == 3) ? 11 : 0));

    // Cân đối độ rộng các cột để cột "Tên Sân Bóng" rộng rãi, không bị cắt chữ
    ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_HighlightHoveredColumn;
    if (ImGui::BeginTable("OwnerFieldsTable", 7, tableFlags)) {
        ImGui::TableSetupColumn("Mã Sân", ImGuiTableColumnFlags_WidthFixed, 68.0f);
        ImGui::TableSetupColumn("Tên Sân Bóng", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Loại Sân", ImGuiTableColumnFlags_WidthFixed, 98.0f);
        ImGui::TableSetupColumn("Đơn Giá / Ca", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Tình Trạng Khung Giờ", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Trạng Thái", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Thao Tác", ImGuiTableColumnFlags_WidthFixed, 205.0f);
        ImGui::TableHeadersRow();

        for (const auto& f : ctx.fieldRepo.getAllFields()) {
            if (filterTypeVal != 0 && f.getFieldType() != filterTypeVal) continue;

            LinkedList<TimeSlot> slots = ctx.fieldRepo.getSlotsByField(f.getFieldId());
            int totalSlots = 0, freeSlots = 0;
            for (const auto& s : slots) {
                totalSlots++;
                if (!ctx.bookingRepo.isSlotBookedOnDate(s.getSlotId(), dateStr) &&
                    !ctx.tourRepo.isSlotUsedByMatch(s.getSlotId(), dateStr)) {
                    freeSlots++;
                }
            }

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", f.getFieldId().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", f.getFieldName().c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Sân %d người", f.getFieldType());

            ImGui::TableSetColumnIndex(3);
            ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::PrimaryOrange, "%s", UI::FormatMoney(f.getPricePerSlot()).c_str());

            ImGui::TableSetColumnIndex(4);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Trống %d / %d ca", freeSlots, totalSlots);

            ImGui::TableSetColumnIndex(5);
            UI::StatusBadge(f.getStatus());

            ImGui::TableSetColumnIndex(6);
            std::string detailBtn = "Chi tiết & Lịch##det_" + f.getFieldId();
            if (UI::SecondaryButton(detailBtn.c_str(), ImVec2(112.0f, 30.0f))) {
                selectedFieldId = f.getFieldId();
                editPrice = f.getPricePerSlot();
                openDetailModal = true;
            }
            ImGui::SameLine(0.0f, 6.0f);

            bool isAvail = (f.getStatus() == "AVAILABLE");
            std::string toggleLbl = isAvail ? ("Bảo trì##t_" + f.getFieldId()) : ("Mở cửa##t_" + f.getFieldId());
            bool toggled = isAvail ? UI::DangerButton(toggleLbl.c_str(), ImVec2(78.0f, 30.0f))
                                   : UI::SuccessButton(toggleLbl.c_str(), ImVec2(78.0f, 30.0f));
            if (toggled) {
                ctx.fieldService.setFieldMaintenance(f.getFieldId(), isAvail);
                ctx.saveAllData();
                ctx.notify("Đã chuyển sân " + f.getFieldId() + " sang trạng thái " +
                           (isAvail ? "BẢO TRÌ" : "MỞ CỬA"), false, glfwGetTime());
            }
        }
        ImGui::EndTable();
    }

    UI::EndSectionCard();

    renderCreateFieldModal();
    renderFieldDetailAndScheduleModal();
}

void OwnerFieldScreen::renderCreateFieldModal() {
    if (openCreateModal) {
        ImGui::OpenPopup("Thêm Sân Bóng Mới");
        openCreateModal = false;
    }

    UI::CenterNextModal(520.0f, 315.0f);
    if (ImGui::BeginPopupModal("Thêm Sân Bóng Mới", nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::TextColored(Theme::TextMuted, "Hệ thống sẽ tự động sinh 10 khung giờ (90 phút/ca) cho sân mới.");
        ImGui::Spacing();

        if (UI::BeginFormTable("CreateFieldForm", 160.0f)) {
            UI::FormRowLabel("Mã sân (VD: F04):");
            ImGui::InputText("##new_fid", newIdBuf, sizeof(newIdBuf));

            UI::FormRowLabel("Tên sân bóng:");
            ImGui::InputText("##new_fname", newNameBuf, sizeof(newNameBuf));

            UI::FormRowLabel("Loại sân:");
            const char* types[] = { "Sân 5 người", "Sân 7 người", "Sân 11 người" };
            ImGui::Combo("##new_ftype", &newTypeIdx, types, IM_ARRAYSIZE(types));

            UI::FormRowLabel("Đơn giá / ca (VNĐ):");
            ImGui::InputDouble("##new_fprice", &newPrice, 10000.0, 50000.0, "%.0f");
            UI::EndFormTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float btnW = 135.0f;
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - btnW * 2.0f - 12.0f);
        if (UI::SecondaryButton("Hủy bỏ", ImVec2(btnW, 36.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (UI::PrimaryButton("Tạo sân mới", ImVec2(btnW, 36.0f))) {
            std::string fid   = AppContext::toUpper(AppContext::sanitize(newIdBuf));
            std::string fname = AppContext::sanitize(newNameBuf);
            int typeVal       = (newTypeIdx == 0) ? 5 : ((newTypeIdx == 1) ? 7 : 11);

            if (fid.empty() || fname.empty() || newPrice <= 0) {
                ctx.notify("Vui lòng nhập đầy đủ Mã sân, Tên sân và Giá > 0!", true, glfwGetTime());
            } else if (ctx.fieldService.createNewField(fid, fname, typeVal, newPrice)) {
                ctx.saveAllData();
                ctx.notify("Đã tạo sân " + fid + " và sinh 10 khung giờ thành công!", false, glfwGetTime());
                ImGui::CloseCurrentPopup();
            } else {
                ctx.notify("Mã sân " + fid + " đã tồn tại!", true, glfwGetTime());
            }
        }
        ImGui::EndPopup();
    }
}

void OwnerFieldScreen::renderFieldDetailAndScheduleModal() {
    if (openDetailModal) {
        ImGui::OpenPopup("Chi Tiết Sân & Sơ Đồ Khung Giờ");
        openDetailModal = false;
    }

    UI::CenterNextModal(880.0f, 540.0f);
    if (ImGui::BeginPopupModal("Chi Tiết Sân & Sơ Đồ Khung Giờ", nullptr,
                               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
        Field* f = ctx.fieldRepo.findFieldById(selectedFieldId);
        if (!f) {
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return;
        }

        if (UI::BeginFormTable("EditFieldPriceForm", 175.0f)) {
            UI::FormRowLabel("Sân đang chọn:");
            ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::PrimaryOrange, "[%s] %s (Sân %d người)",
                               f->getFieldId().c_str(), f->getFieldName().c_str(), f->getFieldType());

            UI::FormRowLabel("Cập nhật giá gốc / ca:");
            ImGui::SetNextItemWidth(220.0f);
            ImGui::InputDouble("##edit_fprice", &editPrice, 10000.0, 50000.0, "%.0f");
            ImGui::SameLine();
            if (UI::PrimaryButton("Lưu giá mới", ImVec2(130.0f, 0.0f)) && editPrice > 0) {
                f->setPrice(editPrice);
                ctx.saveAllData();
                ctx.notify("Đã cập nhật giá sân " + f->getFieldId() + " thành công!", false, glfwGetTime());
            }
            UI::EndFormTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        std::string dateStr = AppContext::sanitize(checkDateBuf);
        if (!DateTime::isValidDate(dateStr)) dateStr = DateTime::todayDate();
        ImGui::TextColored(Theme::PrimaryOrange, "SƠ ĐỒ 10 KHUNG GIỜ TRONG NGÀY %s:", dateStr.c_str());
        ImGui::Spacing();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 8.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 4.0f));

        if (ImGui::BeginTable("ModalSlotGrid", 5, ImGuiTableFlags_SizingStretchSame)) {
            int idx = 0;
            for (const auto& s : ctx.fieldRepo.getSlotsByField(selectedFieldId)) {
                if (idx % 5 == 0) ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(idx % 5);

                bool booked = ctx.bookingRepo.isSlotBookedOnDate(s.getSlotId(), dateStr);
                bool match  = ctx.tourRepo.isSlotUsedByMatch(s.getSlotId(), dateStr);
                double p    = f->getPricePerSlot() * s.getPriceMultiplier();

                ImVec4 bg = booked ? Theme::SoftRedBg : (match ? Theme::SoftAmberBg : Theme::SoftGreenBg);
                ImVec4 fg = booked ? Theme::DangerRed : (match ? Theme::WarningAmber : Theme::SuccessGreen);
                const char* st = booked ? "ĐÃ CÓ KHÁCH" : (match ? "LỊCH GIẢI ĐẤU" : "CÒN TRỐNG");

                ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
                char cid[32];
                std::snprintf(cid, sizeof(cid), "mslot_%d", idx);
                ImGui::BeginChild(cid, ImVec2(0.0f, 96.0f), true, ImGuiWindowFlags_NoScrollbar);

                ImGui::TextColored(Theme::PrimaryOrange, "%s", s.getSlotId().c_str());
                if (s.getPriceMultiplier() > 1.0) {
                    ImGui::SameLine();
                    ImGui::TextColored(Theme::WarningAmber, "(Giờ vàng)");
                }

                ImGui::TextColored(Theme::TextPrimary, "Giờ: %s - %s", s.getStartTime().c_str(), s.getEndTime().c_str());
                ImGui::TextColored(Theme::TextMuted, "%s", UI::FormatMoney(p).c_str());

                // Vẽ chấm tròn trạng thái thay vì ký tự Unicode gây lỗi '?'
                ImVec2 dotPos = ImGui::GetCursorScreenPos();
                float lineH = ImGui::GetTextLineHeight();
                ImGui::GetWindowDrawList()->AddCircleFilled(
                    ImVec2(dotPos.x + 5.0f, dotPos.y + lineH * 0.5f),
                    4.0f, ImGui::ColorConvertFloat4ToU32(fg));
                ImGui::Dummy(ImVec2(12.0f, lineH));
                ImGui::SameLine(0.0f, 3.0f);
                ImGui::TextColored(fg, "%s", st);

                ImGui::EndChild();
                ImGui::PopStyleColor();
                idx++;
            }
            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);

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