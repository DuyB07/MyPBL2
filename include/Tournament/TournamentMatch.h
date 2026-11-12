#pragma once
#include <string>

class TournamentMatch {
private:
    std::string matchId;
    std::string tournamentId;
    std::string fieldId;
    std::string matchDate;
    std::string slotId;
    std::string teamAId;
    std::string teamBId;
    int scoreA;
    int scoreB;
    bool completed;

public:
    TournamentMatch(const std::string& mId, const std::string& tourId, const std::string& fId,
                    const std::string& mDate, const std::string& sId,
                    const std::string& tA, const std::string& tB,
                    int scA = 0, int scB = 0, bool isComp = false)
        : matchId(mId), tournamentId(tourId), fieldId(fId), matchDate(mDate), slotId(sId),
          teamAId(tA), teamBId(tB), scoreA(scA), scoreB(scB), completed(isComp) {}

    std::string getMatchId() const { return matchId; }
    std::string getTournamentId() const { return tournamentId; }
    std::string getFieldId() const { return fieldId; }
    std::string getMatchDate() const { return matchDate; }
    std::string getSlotId() const { return slotId; }
    std::string getTeamAId() const { return teamAId; }
    std::string getTeamBId() const { return teamBId; }
    int getScoreA() const { return scoreA; }
    int getScoreB() const { return scoreB; }
    bool isCompleted() const { return completed; }

    void setSchedule(const std::string& date, const std::string& sId) {
        matchDate = date;
        slotId = sId;
    }

    void setResult(int sA, int sB) {
        if (sA >= 0 && sB >= 0) {
            scoreA = sA;
            scoreB = sB;
            completed = true;
        }
    }

    std::string serialize() const {
        return matchId + "|" + tournamentId + "|" + fieldId + "|" + matchDate + "|" +
               slotId + "|" + teamAId + "|" + teamBId + "|" +
               std::to_string(scoreA) + "|" + std::to_string(scoreB) + "|" + std::to_string(completed);
    }
};