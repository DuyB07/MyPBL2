#include "GUI/Screens/Owner/OwnerTournamentScreen.h"
#include "GUI/Components/UIComponents.h"
#include "GUI/Theme/OrangeLightTheme.h"
#include <GLFW/glfw3.h>

OwnerTournamentScreen::OwnerTournamentScreen(AppContext& context)
    : ctx(context), openMatchesModal(false), scoreA(0), scoreB(0) {}

void OwnerTournamentScreen::render() {
    UI::PageHeader("QUẢN LÝ GIẢI ĐẤU & XẾP LỊCH TỰ ĐỘNG",
                   "Sử dụng hàng đợi Queue<TournamentMatch> (FIFO) để xếp lịch vòng tròn không trùng ca đặt lẻ");

    UI::BeginSectionCard("OwnerTourListCard", "DANH SÁCH GIẢI ĐẤU ĐĂNG KÝ (data/tournaments.txt)");

    if (ctx.tourRepo.getAllTournaments().empty()) {
        UI::RenderEmptyState("Chưa có giải đấu nào được đăng ký.");
    } else if (ImGui::BeginTable("OwnerTourTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Mã Giải", ImGuiTableColumnFlags_WidthFixed, 68.0f);
        ImGui::TableSetupColumn("Tên Giải Đấu", ImGuiTableColumnFlags_WidthStretch, 1.35f);
        ImGui::TableSetupColumn("Sân & Ngày", ImGuiTableColumnFlags_WidthFixed, 138.0f);
        ImGui::TableSetupColumn("Số Đội", ImGuiTableColumnFlags_WidthFixed, 72.0f);
        ImGui::TableSetupColumn("Kết Quả Chung Cuộc", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Trạng Thái", ImGuiTableColumnFlags_WidthFixed, 125.0f);
        ImGui::TableSetupColumn("Điều Hành Giải", ImGuiTableColumnFlags_WidthFixed, 245.0f);
        ImGui::TableHeadersRow();

        for (const auto& t : ctx.tourRepo.getAllTournaments()) {
            LinkedList<Team> tTeams = ctx.tourRepo.getTeamsByTournament(t.getTournamentId());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", t.getTournamentId().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", t.getName().c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding();
            ImGui::Text("%s (%s)", t.getFieldId().c_str(), t.getStartDate().c_str());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding();
            ImGui::Text("%zu / %d đội", tTeams.size(), t.getMaxTeams());

            ImGui::TableSetColumnIndex(4);
            ImGui::AlignTextToFramePadding();
            TournamentResult res("", "", "", 0);
            if (ctx.tourService.calculateTournamentResult(t.getTournamentId(), res)) {
                ImGui::TextColored(Theme::PrimaryOrange, "VĐ: %s | Á quân: %s",
                                   res.getChampionTeamId().c_str(), res.getRunnerUpTeamId().c_str());
            } else {
                ImGui::TextDisabled("Chưa có kết quả");
            }

            ImGui::TableSetColumnIndex(5);
            UI::StatusBadge(t.getStatus());

            ImGui::TableSetColumnIndex(6);
            std::string viewBtn = "Lịch & Tỉ số##v_" + t.getTournamentId();
            if (UI::SecondaryButton(viewBtn.c_str(), ImVec2(96.0f, 28.0f))) {
                selectedTourId = t.getTournamentId();
                selectedMatchId.clear();
                openMatchesModal = true;
            }

            if (t.getStatus() == "PENDING") {
                ImGui::SameLine(0.0f, 5.0f);
                std::string appBtn = "Duyệt##a_" + t.getTournamentId();
                if (UI::PrimaryButton(appBtn.c_str(), ImVec2(66.0f, 28.0f))) {
                    if (ctx.tourService.approveAndScheduleTournament(t.getTournamentId())) {
                        ctx.saveAllData();
                        selectedTourId = t.getTournamentId();
                        openMatchesModal = true;
                        ctx.notify("Đã duyệt giải " + t.getTournamentId() + " và xếp lịch qua Queue!", false, glfwGetTime());
                    } else {
                        ctx.notify("Không thể duyệt! Cần >= 2 đội đăng ký và sân đủ khung giờ trống.", true, glfwGetTime());
                    }
                }
                ImGui::SameLine(0.0f, 5.0f);
                std::string rejBtn = "Từ chối##r_" + t.getTournamentId();
                if (UI::DangerButton(rejBtn.c_str(), ImVec2(66.0f, 28.0f))) {
                    if (ctx.tourService.rejectTournament(t.getTournamentId())) {
                        ctx.saveAllData();
                        ctx.notify("Đã từ chối yêu cầu tạo giải " + t.getTournamentId(), false, glfwGetTime());
                    }
                }
            }
        }
        ImGui::EndTable();
    }

    UI::EndSectionCard();

    renderMatchesAndScoreModal();
}

void OwnerTournamentScreen::renderMatchesAndScoreModal() {
    if (openMatchesModal) {
        ImGui::OpenPopup("Lịch Thi Đấu & Cập Nhật Tỉ Số");
        openMatchesModal = false;
    }

    UI::CenterNextModal(820.0f, 480.0f);
    if (ImGui::BeginPopupModal("Lịch Thi Đấu & Cập Nhật Tỉ Số", nullptr,
                               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
        Tournament* t = ctx.tourRepo.findTournamentById(selectedTourId);
        if (!t) {
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return;
        }

        ImGui::TextColored(Theme::PrimaryOrange, "GIẢI ĐẤU: [%s] %s  |  Sân: %s  |  Ngày: %s",
                           t->getTournamentId().c_str(), t->getName().c_str(),
                           t->getFieldId().c_str(), t->getStartDate().c_str());
        ImGui::Spacing();

        LinkedList<TournamentMatch> matches = ctx.tourRepo.getMatchesByTournament(selectedTourId);
        LinkedList<Team> tourTeams = ctx.tourRepo.getTeamsByTournament(selectedTourId);

        auto resolveTeamLabel = [&tourTeams](const std::string& tmId) -> std::string {
            for (const auto& tm : tourTeams) {
                if (tm.getTeamId() == tmId) {
                    return tmId + " (" + tm.getTeamName() + ")";
                }
            }
            return tmId;
        };

        if (matches.empty()) {
            ImGui::BeginChild("EmptyMatchesBox", ImVec2(0, 250.0f), false);
            ImGui::SetCursorPosY(95.0f);
            UI::RenderEmptyState("Giải đấu này đang chờ duyệt (PENDING), chưa sinh lịch thi đấu.");
            ImGui::EndChild();
        } else if (ImGui::BeginTable("ModalMatchesTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 250.0f))) {
            ImGui::TableSetupColumn("Mã Trận", ImGuiTableColumnFlags_WidthFixed, 82.0f);
            ImGui::TableSetupColumn("Khung Giờ", ImGuiTableColumnFlags_WidthFixed, 92.0f);
            ImGui::TableSetupColumn("Cặp Thi Đấu", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Tỉ Số", ImGuiTableColumnFlags_WidthFixed, 78.0f);
            ImGui::TableSetupColumn("Trạng Thái", ImGuiTableColumnFlags_WidthFixed, 125.0f);
            ImGui::TableSetupColumn("Thao Tác", ImGuiTableColumnFlags_WidthFixed, 108.0f);
            ImGui::TableHeadersRow();

            for (const auto& m : matches) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", m.getMatchId().c_str());
                ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", m.getSlotId().c_str());
                ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding();
                ImGui::Text("%s  vs  %s",
                            resolveTeamLabel(m.getTeamAId()).c_str(),
                            resolveTeamLabel(m.getTeamBId()).c_str());
                ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding();
                ImGui::TextColored(Theme::PrimaryOrange, "%d - %d", m.getScoreA(), m.getScoreB());
                ImGui::TableSetColumnIndex(4);
                UI::StatusBadge(m.isCompleted() ? "COMPLETED" : "UNPLAYED");
                ImGui::TableSetColumnIndex(5);
                std::string btn = "Nhập tỉ số##ms_" + m.getMatchId();
                if (UI::SecondaryButton(btn.c_str(), ImVec2(96.0f, 28.0f))) {
                    selectedMatchId = m.getMatchId();
                    scoreA = m.getScoreA();
                    scoreB = m.getScoreB();
                }
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (!selectedMatchId.empty()) {
            ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::PrimaryOrange, "Cập nhật tỉ số trận [%s]:", selectedMatchId.c_str());
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::InputInt("Đội A", &scoreA);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::InputInt("Đội B", &scoreB);
            ImGui::SameLine();
            if (UI::PrimaryButton("Lưu kết quả", ImVec2(120.0f, 0.0f))) {
                if (ctx.tourService.recordMatchScore(selectedMatchId, scoreA, scoreB)) {
                    ctx.saveAllData();
                    ctx.notify("Đã lưu tỉ số trận " + selectedMatchId + " thành công!", false, glfwGetTime());
                } else {
                    ctx.notify("Tỉ số phải >= 0!", true, glfwGetTime());
                }
            }
        } else {
            ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::TextMuted, "Gợi ý: Bấm nút [Nhập tỉ số] ở một trận đấu phía trên để cập nhật kết quả.");
        }

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