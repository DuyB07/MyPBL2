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
    std::snprintf(newIdBuf, sizeof(newIdBuf), "F03");
    std::snprintf(newNameBuf, sizeof(newNameBuf), "San C (Co Nhan Tao)");
}

void OwnerFieldScreen::render() {
    if (UI::PageHeader("QUAN LY DANH SACH SAN BONG",
                       "Quan ly trang thai van hanh, don gia va khung gio thue cua tung san",
                       "+ Them san bong moi")) {
        int nextIdx = static_cast<int>(ctx.fieldRepo.getAllFields().size()) + 1;
        std::snprintf(newIdBuf, sizeof(newIdBuf), "F0%d", nextIdx);
        openCreateModal = true;
    }

    UI::BeginSectionCard("FieldListSection", "DANH SACH SAN BONG HIEN CO");

    // Thanh công cụ lọc được căn thẳng hàng Baseline
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Loc theo loai san:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(170.0f);
    const char* filterItems[] = { "Tat ca loai san", "San 5 nguoi", "San 7 nguoi", "San 11 nguoi" };
    ImGui::Combo("##filter_type", &filterTypeIdx, filterItems, IM_ARRAYSIZE(filterItems));

    ImGui::SameLine(0.0f, 24.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Ngay kiem tra ca trong:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(135.0f);
    ImGui::InputText("##chk_date", checkDateBuf, sizeof(checkDateBuf));
    ImGui::SameLine();
    if (UI::SecondaryButton("Hom nay", ImVec2(90.0f, 0.0f))) {
        std::string today = DateTime::todayDate();
        std::snprintf(checkDateBuf, sizeof(checkDateBuf), "%s", today.c_str());
    }

    ImGui::Spacing();

    std::string dateStr = AppContext::sanitize(checkDateBuf);
    if (!DateTime::isValidDate(dateStr)) dateStr = DateTime::todayDate();

    int filterTypeVal = (filterTypeIdx == 1) ? 5 : ((filterTypeIdx == 2) ? 7 : ((filterTypeIdx == 3) ? 11 : 0));

    // Bảng danh sách sân với độ rộng cột cố định/co giãn chuẩn
    ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_HighlightHoveredColumn;
    if (ImGui::BeginTable("OwnerFieldsTable", 7, tableFlags)) {
        ImGui::TableSetupColumn("Ma San", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("Ten San Bong", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Loai San", ImGuiTableColumnFlags_WidthFixed, 105.0f);
        ImGui::TableSetupColumn("Don Gia / Ca", ImGuiTableColumnFlags_WidthFixed, 125.0f);
        ImGui::TableSetupColumn("Tinh Trang Khung Gio", ImGuiTableColumnFlags_WidthFixed, 175.0f);
        ImGui::TableSetupColumn("Trang Thai", ImGuiTableColumnFlags_WidthFixed, 125.0f);
        ImGui::TableSetupColumn("Thao Tac", ImGuiTableColumnFlags_WidthFixed, 215.0f);
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
            ImGui::Text("San %d nguoi", f.getFieldType());

            ImGui::TableSetColumnIndex(3);
            ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::PrimaryOrange, "%.0f VND", f.getPricePerSlot());

            ImGui::TableSetColumnIndex(4);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Trong %d / %d khung gio", freeSlots, totalSlots);

            ImGui::TableSetColumnIndex(5);
            UI::StatusBadge(f.getStatus());

            ImGui::TableSetColumnIndex(6);
            std::string detailBtn = "Chi tiet & Lich##det_" + f.getFieldId();
            if (UI::SecondaryButton(detailBtn.c_str(), ImVec2(115.0f, 30.0f))) {
                selectedFieldId = f.getFieldId();
                editPrice = f.getPricePerSlot();
                openDetailModal = true;
            }
            ImGui::SameLine(0.0f, 6.0f);

            bool isAvail = (f.getStatus() == "AVAILABLE");
            std::string toggleLbl = isAvail ? ("Bao tri##t_" + f.getFieldId()) : ("Mo cua##t_" + f.getFieldId());
            bool toggled = isAvail ? UI::DangerButton(toggleLbl.c_str(), ImVec2(82.0f, 30.0f))
                                   : UI::SuccessButton(toggleLbl.c_str(), ImVec2(82.0f, 30.0f));
            if (toggled) {
                ctx.fieldService.setFieldMaintenance(f.getFieldId(), isAvail);
                ctx.saveAllData();
                ctx.notify("Da chuyen san " + f.getFieldId() + " sang trang thai " +
                           (isAvail ? "MAINTENANCE" : "AVAILABLE"), false, glfwGetTime());
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
        ImGui::OpenPopup("Them San Bong Moi");
        openCreateModal = false;
    }

    UI::CenterNextModal(500.0f, 310.0f);
    if (ImGui::BeginPopupModal("Them San Bong Moi", nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::TextColored(Theme::TextMuted, "He thong se tu dong sinh 10 khung gio (90 phut/ca) cho san moi.");
        ImGui::Spacing();

        if (UI::BeginFormTable("CreateFieldForm", 155.0f)) {
            UI::FormRowLabel("Ma san (VD: F03):");
            ImGui::InputText("##new_fid", newIdBuf, sizeof(newIdBuf));

            UI::FormRowLabel("Ten san bong:");
            ImGui::InputText("##new_fname", newNameBuf, sizeof(newNameBuf));

            UI::FormRowLabel("Loai san:");
            const char* types[] = { "San 5 nguoi", "San 7 nguoi", "San 11 nguoi" };
            ImGui::Combo("##new_ftype", &newTypeIdx, types, IM_ARRAYSIZE(types));

            UI::FormRowLabel("Don gia / ca (VND):");
            ImGui::InputDouble("##new_fprice", &newPrice, 10000.0, 50000.0, "%.0f");
            UI::EndFormTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float btnW = 135.0f;
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - btnW * 2.0f - 12.0f);
        if (UI::SecondaryButton("Huy bo", ImVec2(btnW, 36.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (UI::PrimaryButton("Tao san moi", ImVec2(btnW, 36.0f))) {
            std::string fid   = AppContext::toUpper(AppContext::sanitize(newIdBuf));
            std::string fname = AppContext::sanitize(newNameBuf);
            int typeVal       = (newTypeIdx == 0) ? 5 : ((newTypeIdx == 1) ? 7 : 11);

            if (fid.empty() || fname.empty() || newPrice <= 0) {
                ctx.notify("Vui long nhap day du Ma san, Ten san va Gia > 0!", true, glfwGetTime());
            } else if (ctx.fieldService.createNewField(fid, fname, typeVal, newPrice)) {
                ctx.saveAllData();
                ctx.notify("Da tao san " + fid + " va sinh 10 khung gio thanh cong!", false, glfwGetTime());
                ImGui::CloseCurrentPopup();
            } else {
                ctx.notify("Ma san " + fid + " da ton tai!", true, glfwGetTime());
            }
        }
        ImGui::EndPopup();
    }
}

void OwnerFieldScreen::renderFieldDetailAndScheduleModal() {
    if (openDetailModal) {
        ImGui::OpenPopup("Chi Tiet San & So Do Khung Gio");
        openDetailModal = false;
    }

    UI::CenterNextModal(760.0f, 490.0f);
    if (ImGui::BeginPopupModal("Chi Tiet San & So Do Khung Gio", nullptr, ImGuiWindowFlags_NoResize)) {
        Field* f = ctx.fieldRepo.findFieldById(selectedFieldId);
        if (!f) {
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return;
        }

        if (UI::BeginFormTable("EditFieldPriceForm", 170.0f)) {
            UI::FormRowLabel("San dang chon:");
            ImGui::TextColored(Theme::PrimaryOrange, "[%s] %s (San %d nguoi)",
                               f->getFieldId().c_str(), f->getFieldName().c_str(), f->getFieldType());

            UI::FormRowLabel("Cap nhat gia goc / ca:");
            ImGui::SetNextItemWidth(220.0f);
            ImGui::InputDouble("##edit_fprice", &editPrice, 10000.0, 50000.0, "%.0f");
            ImGui::SameLine();
            if (UI::PrimaryButton("Luu gia moi", ImVec2(130.0f, 0.0f)) && editPrice > 0) {
                f->setPrice(editPrice);
                ctx.saveAllData();
                ctx.notify("Da cap nhat gia san " + f->getFieldId() + " thanh cong!", false, glfwGetTime());
            }
            UI::EndFormTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        std::string dateStr = AppContext::sanitize(checkDateBuf);
        if (!DateTime::isValidDate(dateStr)) dateStr = DateTime::todayDate();
        ImGui::TextColored(Theme::PrimaryOrange, "SO DO 10 KHUNG GIO TRONG NGAY %s:", dateStr.c_str());

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
                const char* st = booked ? "DA CO KHACH" : (match ? "GIAI DAU" : "CON TRONG");

                ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
                char cid[32];
                std::snprintf(cid, sizeof(cid), "mslot_%d", idx);
                ImGui::BeginChild(cid, ImVec2(0, 86.0f), true, ImGuiWindowFlags_NoScrollbar);
                ImGui::TextColored(Theme::TextPrimary, "%s (%s-%s)", s.getSlotId().c_str(),
                                   s.getStartTime().c_str(), s.getEndTime().c_str());
                ImGui::TextColored(Theme::TextMuted, "%.0f VND %s", p, s.getPriceMultiplier() > 1.0 ? "(x1.2)" : "");
                ImGui::TextColored(fg, "%s", st);
                ImGui::EndChild();
                ImGui::PopStyleColor();
                idx++;
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