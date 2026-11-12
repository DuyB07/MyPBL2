#pragma once
#include <string>
#include "Core/LinkedList.h"
#include "Tournament/Tournament.h"
#include "Tournament/Team.h"
#include "Tournament/TournamentMatch.h"
#include "Storage/FileManager.h"

class TournamentRepository {
private:
    LinkedList<Tournament> tournaments;
    LinkedList<Team> teams;
    LinkedList<TournamentMatch> matches;

public:
    TournamentRepository() = default;

    // --- Tournaments ---
    bool loadTournaments(const std::string& path = "data/tournaments.txt");
    bool saveTournaments(const std::string& path = "data/tournaments.txt") const;
    void addTournament(const Tournament& t);
    Tournament* findTournamentById(const std::string& id);
    const LinkedList<Tournament>& getAllTournaments() const { return tournaments; }

    // --- Teams ---
    bool loadTeams(const std::string& path = "data/teams.txt");
    bool saveTeams(const std::string& path = "data/teams.txt") const;
    void addTeam(const Team& team);
    LinkedList<Team> getTeamsByTournament(const std::string& tournamentId) const;

    // --- Matches ---
    bool loadMatches(const std::string& path = "data/matches.txt");
    bool saveMatches(const std::string& path = "data/matches.txt") const;
    void addMatch(const TournamentMatch& match);
    TournamentMatch* findMatchById(const std::string& matchId);
    LinkedList<TournamentMatch> getMatchesByTournament(const std::string& tournamentId) const;
    bool isSlotUsedByMatch(const std::string& slotId, const std::string& date) const;
};