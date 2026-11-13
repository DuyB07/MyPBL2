#include "GUI/Screens/Owner/OwnerTournamentScreen.h"
#include "GUI/Components/UIComponents.h"
#include "GUI/Theme/OrangeLightTheme.h"
#include <GLFW/glfw3.h>

OwnerTournamentScreen::OwnerTournamentScreen(AppContext& context)
    : ctx(context), openMatchesModal(false), scoreA(0), scoreB(0) {}

void OwnerTournamentScreen::render() {
    UI::PageHeader("QUAN LY GIAI DAU & XEP LICH TU DONG",
                   "Su dung hang doi Queue<TournamentMatch> (FIFO) de xep lich vong tron khong trung ca dat le");

    UI::BeginSectionCard("OwnerTourListCard", "DANH SACH GIAI DAU DANG KY (data/tournaments.txt)");

    if (ImGui::BeginTable("OwnerTourTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Ma Giai", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("Ten Giai Dau", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("San & Ngay", ImGuiTableColumnFlags_WidthFixed, 145.0f);
        ImGui::TableSetupColumn("So Doi", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("Ket Qua Chung Cuoc", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Trang Thai", ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableSetupColumn("Dieu Hanh Giai", ImGuiTableColumnFlags_WidthFixed, 250.0f);
        ImGui::TableHeadersRow();

        for (const auto& t : ctx.tourRepo.getAllTournaments()) {
            LinkedList<Team> tTeams = ctx.tourRepo.getTeamsByTournament(t.getTournamentId());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", t.getTournamentId().c_str());
            ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", t.getName().c_str());
            ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding();
            ImGui::Text("%s (%s)", t.getFieldId().c_str(), t.getStartDate().c_str());
            ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding();
            ImGui::Text("%zu / %d", tTeams.size(), t.getMaxTeams());

            ImGui::TableSetColumnIndex(4);
            ImGui::AlignTextToFramePadding();
            TournamentResult res("", "", "", 0);
            if (ctx.tourService.calculateTournamentResult(t.getTournamentId(), res)) {
                ImGui::TextColored(Theme::PrimaryOrange, "VD: %s | AQ: %s",
                                   res.getChampionTeamId().c_str(), res.getRunnerUpTeamId().c_str());
            } else {
                ImGui::TextDisabled("Chua co ket qua");
            }

            ImGui::TableSetColumnIndex(5);
            UI::StatusBadge(t.getStatus());

            ImGui::TableSetColumnIndex(6);
            std::string viewBtn = "Lich & Ti so##v_" + t.getTournamentId();
            if (UI::SecondaryButton(viewBtn.c_str(), ImVec2(96.0f, 28.0f))) {
                selectedTourId = t.getTournamentId();
                selectedMatchId.clear();
                openMatchesModal = true;
            }

            if (t.getStatus() == "PENDING") {
                ImGui::SameLine(0.0f, 5.0f);
                std::string appBtn = "Duyet##a_" + t.getTournamentId();
                if (UI::PrimaryButton(appBtn.c_str(), ImVec2(68.0f, 28.0f))) {
                    if (ctx.tourService.approveAndScheduleTournament(t.getTournamentId())) {
                        ctx.saveAllData();
                        selectedTourId = t.getTournamentId();
                        openMatchesModal = true;
                        ctx.notify("Da duyet giai " + t.getTournamentId() + " va xep lich qua Queue!", false, glfwGetTime());
                    } else {
                        ctx.notify("Khong the duyet! Can >= 2 doi dang ky va san du khung gio trong.", true, glfwGetTime());
                    }
                }
                ImGui::SameLine(0.0f, 5.0f);
                std::string rejBtn = "Huy##r_" + t.getTournamentId();
                if (UI::DangerButton(rejBtn.c_str(), ImVec2(62.0f, 28.0f))) {
                    if (ctx.tourService.rejectTournament(t.getTournamentId())) {
                        ctx.saveAllData();
                        ctx.notify("Da tu choi yeu cau tao giai " + t.getTournamentId(), false, glfwGetTime());
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
        ImGui::OpenPopup("Lich Thi Dau & Cap Nhat Ti So");
        openMatchesModal = false;
    }

    UI::CenterNextModal(740.0f, 470.0f);
    if (ImGui::BeginPopupModal("Lich Thi Dau & Cap Nhat Ti So", nullptr, ImGuiWindowFlags_NoResize)) {
        Tournament* t = ctx.tourRepo.findTournamentById(selectedTourId);
        if (!t) {
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return;
        }

        ImGui::TextColored(Theme::PrimaryOrange, "GIAI DAU: [%s] %s  |  San: %s  |  Ngay: %s",
                           t->getTournamentId().c_str(), t->getName().c_str(),
                           t->getFieldId().c_str(), t->getStartDate().c_str());
        ImGui::Spacing();

        if (ImGui::BeginTable("ModalMatchesTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 240.0f))) {
            ImGui::TableSetupColumn("Ma Tran", ImGuiTableColumnFlags_WidthFixed, 85.0f);
            ImGui::TableSetupColumn("Khung Gio", ImGuiTableColumnFlags_WidthFixed, 105.0f);
            ImGui::TableSetupColumn("Cap Thi Dau", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Ti So", ImGuiTableColumnFlags_WidthFixed, 85.0f);
            ImGui::TableSetupColumn("Trang Thai", ImGuiTableColumnFlags_WidthFixed, 105.0f);
            ImGui::TableSetupColumn("Thao Tac", ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableHeadersRow();

            for (const auto& m : ctx.tourRepo.getMatchesByTournament(selectedTourId)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", m.getMatchId().c_str());
                ImGui::TableSetColumnIndex(1); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", m.getSlotId().c_str());
                ImGui::TableSetColumnIndex(2); ImGui::AlignTextToFramePadding();
                ImGui::Text("%s   vs   %s", m.getTeamAId().c_str(), m.getTeamBId().c_str());
                ImGui::TableSetColumnIndex(3); ImGui::AlignTextToFramePadding();
                ImGui::TextColored(Theme::PrimaryOrange, "%d  -  %d", m.getScoreA(), m.getScoreB());
                ImGui::TableSetColumnIndex(4);
                UI::StatusBadge(m.isCompleted() ? "COMPLETED" : "PENDING");
                ImGui::TableSetColumnIndex(5);
                std::string btn = "Nhap ti so##ms_" + m.getMatchId();
                if (UI::SecondaryButton(btn.c_str(), ImVec2(96.0f, 28.0f))) {
                    selectedMatchId = m.getMatchId();
                    scoreA = m.getScoreA();
                    scoreB = m.getScoreB();
                }
            }
            ImGui::EndTable();
        }

        if (!selectedMatchId.empty()) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::AlignTextToFramePadding();
            ImGui::TextColored(Theme::PrimaryOrange, "Cap nhat ti so tran [%s]:", selectedMatchId.c_str());
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100.0f);
            ImGui::InputInt("Doi A", &scoreA);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100.0f);
            ImGui::InputInt("Doi B", &scoreB);
            ImGui::SameLine();
            if (UI::PrimaryButton("Luu ket qua", ImVec2(120.0f, 0.0f))) {
                if (ctx.tourService.recordMatchScore(selectedMatchId, scoreA, scoreB)) {
                    ctx.saveAllData();
                    ctx.notify("Da luu ti so tran " + selectedMatchId + " thanh cong!", false, glfwGetTime());
                } else {
                    ctx.notify("Ti so phai >= 0!", true, glfwGetTime());
                }
            }
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