#include "Service/TournamentService.h"
#include <iostream>

bool TournamentService::requestTournament(const std::string& tourId, const std::string& name,
                                          const std::string& customerId, const std::string& fieldId,
                                          const std::string& startDate, int maxTeams) {
    if (customerService.isCustomerBlacklisted(customerId)) {
        std::cerr << "  [-] Khach hang dang trong Blacklist, khong the tao giai dau!\n";
        return false;
    }

    Field* f = fieldRepo.findFieldById(fieldId);
    if (!f || f->getStatus() != "AVAILABLE") {
        std::cerr << "  [-] San khong ton tai hoac dang bao tri!\n";
        return false;
    }

    if (tourRepo.findTournamentById(tourId) != nullptr) {
        std::cerr << "  [-] Ma giai dau da ton tai!\n";
        return false;
    }

    Tournament t(tourId, name, customerId, fieldId, startDate, maxTeams, "PENDING");
    tourRepo.addTournament(t);
    std::cout << "  [+] Da gui yeu cau tao giai dau: " << name << " (Trang thai: PENDING)\n";
    return true;
}

bool TournamentService::registerTeam(const std::string& teamId, const std::string& tourId,
                                     const std::string& teamName, const std::string& phone) {
    Tournament* t = tourRepo.findTournamentById(tourId);
    if (!t) {
        std::cerr << "  [-] Khong tim thay ma giai dau!\n";
        return false;
    }
    if (t->getStatus() != "PENDING") {
        std::cerr << "  [-] Giai dau da duoc duyet hoac ket thuc, khong the dang ky them doi!\n";
        return false;
    }

    LinkedList<Team> currentTeams = tourRepo.getTeamsByTournament(tourId);
    if (static_cast<int>(currentTeams.size()) >= t->getMaxTeams()) {
        std::cerr << "  [-] Giai dau da du so luong doi toi da (" << t->getMaxTeams() << " doi)!\n";
        return false;
    }

    for (const auto& tm : currentTeams) {
        if (tm.getTeamId() == teamId || tm.getTeamName() == teamName) {
            std::cerr << "  [-] Ma doi hoac Ten doi da ton tai trong giai nay!\n";
            return false;
        }
    }

    tourRepo.addTeam(Team(teamId, tourId, teamName, phone));
    std::cout << "  [+] Dang ky doi thanh cong: " << teamName << " (" << teamId << ")\n";
    return true;
}

bool TournamentService::approveAndScheduleTournament(const std::string& tourId) {
    Tournament* t = tourRepo.findTournamentById(tourId);
    if (!t) {
        std::cerr << "  [-] Khong tim thay ma giai dau!\n";
        return false;
    }
    if (t->getStatus() != "PENDING") {
        std::cerr << "  [-] Giai dau nay khong o trang thai PENDING!\n";
        return false;
    }

    LinkedList<Team> teams = tourRepo.getTeamsByTournament(tourId);
    if (teams.size() < 2) {
        std::cerr << "  [-] Can it nhat 2 doi dang ky de xep lich thi dau!\n";
        return false;
    }

    // 1. Đưa các cặp đấu vòng tròn vào Hàng đợi Queue<TournamentMatch> (FIFO)
    Queue<TournamentMatch> matchQueue;
    int matchCounter = 1;

    for (auto itA = teams.begin(); itA != teams.end(); ++itA) {
        auto itB = itA;
        ++itB;
        for (; itB != teams.end(); ++itB) {
            std::string mId = tourId + "_M" + std::to_string(matchCounter++);
            TournamentMatch m(mId, tourId, t->getFieldId(), t->getStartDate(), "UNASSIGNED",
                              (*itA).getTeamId(), (*itB).getTeamId());
            matchQueue.enqueue(m);
        }
    }

    // 2. Kiểm tra trước xem ngày diễn ra có đủ khung giờ trống cho toàn bộ trận đấu không
    LinkedList<TimeSlot> fieldSlots = fieldRepo.getSlotsByField(t->getFieldId());
    std::size_t freeSlotsCount = 0;
    for (const auto& slot : fieldSlots) {
        if (!bookingRepo.isSlotBookedOnDate(slot.getSlotId(), t->getStartDate()) &&
            !tourRepo.isSlotUsedByMatch(slot.getSlotId(), t->getStartDate())) {
            freeSlotsCount++;
        }
    }

    if (freeSlotsCount < matchQueue.size()) {
        std::cerr << "  [-] San " << t->getFieldId() << " ngay " << t->getStartDate()
                  << " chi con " << freeSlotsCount << " khung gio trong, khong du cho "
                  << matchQueue.size() << " tran dau!\n";
        return false;
    }

    std::cout << "  [*] Da dua " << matchQueue.size() << " tran dau vao Queue<TournamentMatch>.\n";

    // 3. Lấy từng trận từ Queue (FIFO) gán vào các khung giờ trống
    for (const auto& slot : fieldSlots) {
        if (matchQueue.empty()) break;

        bool isBooked = bookingRepo.isSlotBookedOnDate(slot.getSlotId(), t->getStartDate());
        bool isUsedByMatch = tourRepo.isSlotUsedByMatch(slot.getSlotId(), t->getStartDate());

        if (!isBooked && !isUsedByMatch) {
            TournamentMatch match = matchQueue.dequeue();
            match.setSchedule(t->getStartDate(), slot.getSlotId());
            tourRepo.addMatch(match);

            std::cout << "    -> Xep lich tran " << match.getMatchId() << " ("
                      << match.getTeamAId() << " vs " << match.getTeamBId()
                      << ") vao Slot " << slot.getSlotId() << " ["
                      << slot.getStartTime() << " - " << slot.getEndTime() << "]\n";
        } else {
            std::cout << "    [!] Bo qua Slot " << slot.getSlotId()
                      << " do da co lich dat truoc!\n";
        }
    }

    t->setStatus("APPROVED");
    std::cout << "  [+] Da duyet giai dau " << t->getName() << " thanh cong!\n";
    return true;
}

bool TournamentService::rejectTournament(const std::string& tourId) {
    Tournament* t = tourRepo.findTournamentById(tourId);
    if (!t || t->getStatus() != "PENDING") {
        std::cerr << "  [-] Giai dau khong ton tai hoac khong o trang thai PENDING!\n";
        return false;
    }
    t->setStatus("REJECTED");
    std::cout << "  [+] Da tu choi giai dau " << t->getName() << " (Trang thai: REJECTED).\n";
    return true;
}

bool TournamentService::recordMatchScore(const std::string& matchId, int scoreA, int scoreB) {
    if (scoreA < 0 || scoreB < 0) {
        std::cerr << "  [-] Ti so tran dau phai >= 0!\n";
        return false;
    }
    TournamentMatch* m = tourRepo.findMatchById(matchId);
    if (!m) return false;

    m->setResult(scoreA, scoreB);
    std::cout << "  [+] Cap nhat ti so tran " << matchId << ": "
              << m->getTeamAId() << " (" << scoreA << " - " << scoreB << ") " << m->getTeamBId() << "\n";

    // Kiểm tra nếu toàn bộ trận đấu của giải đã hoàn tất -> Chuyển giải sang COMPLETED
    std::string tourId = m->getTournamentId();
    LinkedList<TournamentMatch> allMatches = tourRepo.getMatchesByTournament(tourId);
    bool allDone = !allMatches.empty();
    for (const auto& item : allMatches) {
        if (!item.isCompleted()) {
            allDone = false;
            break;
        }
    }
    if (allDone) {
        Tournament* t = tourRepo.findTournamentById(tourId);
        if (t) {
            t->setStatus("COMPLETED");
            std::cout << "  [*] Tat ca tran dau da hoan tat! Giai dau " << t->getName()
                      << " chuyen sang trang thai COMPLETED.\n";
        }
    }
    return true;
}

bool TournamentService::calculateTournamentResult(const std::string& tourId, TournamentResult& outResult) const {
    LinkedList<Team> teams = tourRepo.getTeamsByTournament(tourId);
    LinkedList<TournamentMatch> matches = tourRepo.getMatchesByTournament(tourId);
    if (teams.size() < 2 || matches.empty()) return false;

    std::string champId = "N/A", runnerUpId = "N/A";
    int maxPts = -1, secondPts = -1;
    int playedCount = 0;

    for (const auto& m : matches) {
        if (m.isCompleted()) playedCount++;
    }
    if (playedCount == 0) return false;

    for (const auto& tm : teams) {
        int pts = 0;
        for (const auto& m : matches) {
            if (!m.isCompleted()) continue;
            if (m.getTeamAId() == tm.getTeamId()) {
                if (m.getScoreA() > m.getScoreB()) pts += 3;
                else if (m.getScoreA() == m.getScoreB()) pts += 1;
            } else if (m.getTeamBId() == tm.getTeamId()) {
                if (m.getScoreB() > m.getScoreA()) pts += 3;
                else if (m.getScoreB() == m.getScoreA()) pts += 1;
            }
        }
        if (pts > maxPts) {
            secondPts = maxPts;
            runnerUpId = champId;
            maxPts = pts;
            champId = tm.getTeamId() + " (" + tm.getTeamName() + " - " + std::to_string(pts) + "d)";
        } else if (pts > secondPts) {
            secondPts = pts;
            runnerUpId = tm.getTeamId() + " (" + tm.getTeamName() + " - " + std::to_string(pts) + "d)";
        }
    }

    outResult = TournamentResult(tourId, champId, runnerUpId, playedCount);
    return true;
}