#include "Service/AccountService.h"
#include <iostream>

bool AccountService::loadOwners(const std::string& path) {
    owners.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        // Format: OWNER|id|uname|pwd|name|phone|sName|addr|oTime|cTime|fCount|rules
        if (tokens.size() >= 12) {
            auto it = tokens.begin();
            if (*it != "OWNER") continue;
            ++it;
            std::string id = *it; ++it;
            std::string uname = *it; ++it;
            std::string pwd = *it; ++it;
            std::string name = *it; ++it;
            std::string phone = *it; ++it;
            std::string sName = *it; ++it;
            std::string addr = *it; ++it;
            std::string oTime = *it; ++it;
            std::string cTime = *it; ++it;
            int fCount = 0;
            try { fCount = std::stoi(*it); } catch (...) {}
            ++it;
            std::string rules = *it;

            owners.pushBack(Owner(id, uname, pwd, name, phone, sName, addr, oTime, cTime, fCount, rules));
        }
    }
    return true;
}

bool AccountService::saveOwners(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& o : owners) {
        lines.pushBack(o.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void AccountService::ensureDefaultOwner() {
    if (owners.empty()) {
        Owner defaultOwner("OWN01", "admin", "admin123", "Chu San Hoa Khanh", "0905123456",
                           "San Bong Mini Bach Khoa", "54 Nguyen Luong Bang, Da Nang",
                           "06:00", "22:00", 2, "Khong mang giay dinh sat");
        owners.pushBack(defaultOwner);
        saveOwners();
    }
}

User* AccountService::login(const std::string& username, const std::string& password) {
    // 1. Kiểm tra trong danh sách Chủ sân (Owner)
    Owner* foundOwner = owners.findIf([&username, &password](const Owner& o) {
        return o.getUsername() == username && o.getPassword() == password;
    });
    if (foundOwner) {
        currentUser = foundOwner; // Upcasting Owner* -> User*
        return currentUser;
    }

    // 2. Tra cứu O(1) trong danh sách Khách hàng (Customer) qua HashTable
    Customer* foundCust = customerRepo.findByUsername(username);
    if (foundCust && foundCust->getPassword() == password) {
        currentUser = foundCust; // Upcasting Customer* -> User*
        return currentUser;
    }

    return nullptr;
}

void AccountService::logout() {
    currentUser = nullptr;
}

Owner* AccountService::getPrimaryOwner() {
    if (owners.empty()) return nullptr;
    return &(*owners.begin());
}