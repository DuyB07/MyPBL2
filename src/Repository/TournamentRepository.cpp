#include "Repository/TournamentRepository.h"

bool TournamentRepository::loadTournaments(const std::string& path) {
    tournaments.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 7) {
            auto it = tokens.begin();
            std::string tId = *it; ++it;
            std::string name = *it; ++it;
            std::string cId = *it; ++it;
            std::string fId = *it; ++it;
            std::string sDate = *it; ++it;
            int maxT = 4;
            try { maxT = std::stoi(*it); } catch (...) {}
            ++it;
            std::string stat = *it;
            tournaments.pushBack(Tournament(tId, name, cId, fId, sDate, maxT, stat));
        }
    }
    return true;
}

bool TournamentRepository::saveTournaments(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& t : tournaments) {
        lines.pushBack(t.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void TournamentRepository::addTournament(const Tournament& t) {
    tournaments.pushBack(t);
}

Tournament* TournamentRepository::findTournamentById(const std::string& id) {
    return tournaments.findIf([&id](const Tournament& t) { return t.getTournamentId() == id; });
}

bool TournamentRepository::loadTeams(const std::string& path) {
    teams.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 4) {
            auto it = tokens.begin();
            std::string tmId = *it; ++it;
            std::string tourId = *it; ++it;
            std::string name = *it; ++it;
            std::string phone = *it;
            teams.pushBack(Team(tmId, tourId, name, phone));
        }
    }
    return true;
}

bool TournamentRepository::saveTeams(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& tm : teams) {
        lines.pushBack(tm.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void TournamentRepository::addTeam(const Team& team) {
    teams.pushBack(team);
}

LinkedList<Team> TournamentRepository::getTeamsByTournament(const std::string& tournamentId) const {
    LinkedList<Team> result;
    for (const auto& tm : teams) {
        if (tm.getTournamentId() == tournamentId) {
            result.pushBack(tm);
        }
    }
    return result;
}

bool TournamentRepository::loadMatches(const std::string& path) {
    matches.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 10) {
            auto it = tokens.begin();
            std::string mId = *it; ++it;
            std::string tourId = *it; ++it;
            std::string fId = *it; ++it;
            std::string mDate = *it; ++it;
            std::string sId = *it; ++it;
            std::string tA = *it; ++it;
            std::string tB = *it; ++it;
            int scA = 0, scB = 0;
            bool comp = false;
            try {
                scA = std::stoi(*it); ++it;
                scB = std::stoi(*it); ++it;
                comp = (std::stoi(*it) != 0);
            } catch (...) {}
            matches.pushBack(TournamentMatch(mId, tourId, fId, mDate, sId, tA, tB, scA, scB, comp));
        }
    }
    return true;
}

bool TournamentRepository::saveMatches(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& m : matches) {
        lines.pushBack(m.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void TournamentRepository::addMatch(const TournamentMatch& match) {
    matches.pushBack(match);
}

TournamentMatch* TournamentRepository::findMatchById(const std::string& matchId) {
    return matches.findIf([&matchId](const TournamentMatch& m) { return m.getMatchId() == matchId; });
}

LinkedList<TournamentMatch> TournamentRepository::getMatchesByTournament(const std::string& tournamentId) const {
    LinkedList<TournamentMatch> result;
    for (const auto& m : matches) {
        if (m.getTournamentId() == tournamentId) {
            result.pushBack(m);
        }
    }
    return result;
}

bool TournamentRepository::isSlotUsedByMatch(const std::string& slotId, const std::string& date) const {
    for (const auto& m : matches) {
        if (m.getSlotId() == slotId && m.getMatchDate() == date) {
            return true;
        }
    }
    return false;
}