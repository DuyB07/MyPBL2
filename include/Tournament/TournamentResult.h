#pragma once
#include <string>

class TournamentResult {
private:
    std::string tournamentId;
    std::string championTeamId;
    std::string runnerUpTeamId;
    int totalMatchesPlayed;

public:
    TournamentResult(const std::string& tourId, const std::string& champ, const std::string& runnerUp, int total)
        : tournamentId(tourId), championTeamId(champ), runnerUpTeamId(runnerUp), totalMatchesPlayed(total) {}

    std::string getTournamentId() const { return tournamentId; }
    std::string getChampionTeamId() const { return championTeamId; }
    std::string getRunnerUpTeamId() const { return runnerUpTeamId; }
    int getTotalMatchesPlayed() const { return totalMatchesPlayed; }
};