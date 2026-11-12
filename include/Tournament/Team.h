#pragma once
#include <string>

class Team {
private:
    std::string teamId;
    std::string tournamentId;
    std::string teamName;
    std::string contactPhone;

public:
    Team(const std::string& tmId, const std::string& tourId, const std::string& name, const std::string& phone)
        : teamId(tmId), tournamentId(tourId), teamName(name), contactPhone(phone) {}

    std::string getTeamId() const { return teamId; }
    std::string getTournamentId() const { return tournamentId; }
    std::string getTeamName() const { return teamName; }
    std::string getContactPhone() const { return contactPhone; }

    std::string serialize() const {
        return teamId + "|" + tournamentId + "|" + teamName + "|" + contactPhone;
    }
};