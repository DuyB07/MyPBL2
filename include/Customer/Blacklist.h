#pragma once
#include <string>

class Blacklist {
private:
    std::string blacklistId;
    std::string customerId;
    std::string phone;
    int cancelCount;
    std::string reason;
    std::string addedDate; // Định dạng YYYY-MM-DD
    bool active;

public:
    Blacklist(const std::string& bId, const std::string& cId, const std::string& ph,
              int count, const std::string& rsn, const std::string& date, bool isActive = true)
        : blacklistId(bId), customerId(cId), phone(ph), cancelCount(count), 
          reason(rsn), addedDate(date), active(isActive) {}

    // Getters
    std::string getBlacklistId() const { return blacklistId; }
    std::string getCustomerId() const { return customerId; }
    std::string getPhone() const { return phone; }
    bool isActive() const { return active; }

    void setActive(bool status) { active = status; }

    std::string serialize() const {
        return blacklistId + "|" + customerId + "|" + phone + "|" + 
               std::to_string(cancelCount) + "|" + reason + "|" + addedDate + "|" + std::to_string(active);
    }
};