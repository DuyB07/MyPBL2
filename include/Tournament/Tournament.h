#pragma once
#include <string>

class Tournament {
private:
    std::string tournamentId;
    std::string name;
    std::string customerId;
    std::string fieldId;
    std::string startDate; // YYYY-MM-DD
    int maxTeams;
    std::string status;    // "PENDING", "APPROVED", "REJECTED", "COMPLETED"

public:
    Tournament(const std::string& tId, const std::string& n, const std::string& cId,
               const std::string& fId, const std::string& sDate, int mTeams = 4,
               const std::string& stat = "PENDING")
        : tournamentId(tId), name(n), customerId(cId), fieldId(fId),
          startDate(sDate), maxTeams(mTeams), status(stat) {
        if (maxTeams < 2) maxTeams = 2;
    }

    std::string getTournamentId() const { return tournamentId; }
    std::string getName() const { return name; }
    std::string getCustomerId() const { return customerId; }
    std::string getFieldId() const { return fieldId; }
    std::string getStartDate() const { return startDate; }
    int getMaxTeams() const { return maxTeams; }
    std::string getStatus() const { return status; }

    void setStatus(const std::string& s) { status = s; }

    std::string serialize() const {
        return tournamentId + "|" + name + "|" + customerId + "|" + fieldId + "|" +
               startDate + "|" + std::to_string(maxTeams) + "|" + status;
    }
};