#pragma once
#include "User.h"
#include <iostream>

class Owner : public User {
private:
    std::string stadiumName;
    std::string address;
    std::string openTime;
    std::string closeTime;
    int fieldCount;
    std::string fieldRules;

public:
    Owner(const std::string& id, const std::string& uname, const std::string& pwd,
          const std::string& name, const std::string& ph,
          const std::string& sName, const std::string& addr,
          const std::string& oTime, const std::string& cTime,
          int fCount, const std::string& rules)
        : User(id, uname, pwd, name, ph, "OWNER"),
          stadiumName(sName), address(addr), openTime(oTime), closeTime(cTime),
          fieldCount(fCount), fieldRules(rules) {}

    std::string getStadiumName() const { return stadiumName; }
    std::string getAddress() const { return address; }
    std::string getOpenTime() const { return openTime; }
    std::string getCloseTime() const { return closeTime; }
    int getFieldCount() const { return fieldCount; }
    std::string getFieldRules() const { return fieldRules; }

    void setStadiumName(const std::string& sName) { if (!sName.empty()) stadiumName = sName; }
    void setAddress(const std::string& addr) { if (!addr.empty()) address = addr; }
    void setFieldRules(const std::string& rules) { if (!rules.empty()) fieldRules = rules; }

    void displayInfo() const override {
        std::cout << "[OWNER] " << fullName << " - San: " << stadiumName << "\n";
    }
    
    // Dành cho FileManager
    std::string serialize() const {
        return "OWNER|" + userID + "|" + username + "|" + password + "|" +
               fullName + "|" + phone + "|" + stadiumName + "|" + address + "|" +
               openTime + "|" + closeTime + "|" + std::to_string(fieldCount) + "|" + fieldRules;
    }
};