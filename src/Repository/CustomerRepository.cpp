#include "Repository/CustomerRepository.h"

bool CustomerRepository::loadCustomers(const std::string& path) {
    customers.clear();
    phoneToIdMap.clear();
    usernameToIdMap.clear();
    
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 10) {
            auto it = tokens.begin();
            ++it; // Bỏ qua chữ "CUSTOMER"
            std::string id = *it; ++it;
            std::string uname = *it; ++it;
            std::string pwd = *it; ++it;
            std::string name = *it; ++it;
            std::string phone = *it; ++it;
            int bCount = 0, cCount = 0, dPercent = 0;
            bool isBl = false;
            try {
                bCount = std::stoi(*it); ++it;
                cCount = std::stoi(*it); ++it;
                dPercent = std::stoi(*it); ++it;
                isBl = (std::stoi(*it) != 0);
            } catch (...) {}

            Customer cust(id, uname, pwd, name, phone, bCount, cCount, dPercent, isBl);
            customers.pushBack(cust);
            phoneToIdMap.insert(phone, id);
            usernameToIdMap.insert(uname, id);
        }
    }
    return true;
}

bool CustomerRepository::saveCustomers(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& cust : customers) {
        lines.pushBack(cust.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

bool CustomerRepository::addCustomer(const Customer& customer) {
    if (findByPhone(customer.getPhone()) != nullptr) return false;
    if (findById(customer.getUserID()) != nullptr) return false;
    if (findByUsername(customer.getUsername()) != nullptr) return false;
    
    customers.pushBack(customer);
    phoneToIdMap.insert(customer.getPhone(), customer.getUserID());
    usernameToIdMap.insert(customer.getUsername(), customer.getUserID());
    return true;
}

Customer* CustomerRepository::findById(const std::string& id) {
    return customers.findIf([&id](const Customer& c) { return c.getUserID() == id; });
}

Customer* CustomerRepository::findByPhone(const std::string& phone) {
    std::string* idPtr = phoneToIdMap.find(phone);
    if (idPtr) return findById(*idPtr);
    return nullptr;
}

Customer* CustomerRepository::findByUsername(const std::string& username) {
    std::string* idPtr = usernameToIdMap.find(username);
    if (idPtr) return findById(*idPtr);
    return nullptr;
}

bool CustomerRepository::loadBlacklists(const std::string& path) {
    blacklists.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 7) {
            auto it = tokens.begin();
            std::string bId = *it; ++it;
            std::string cId = *it; ++it;
            std::string phone = *it; ++it;
            int cCount = 0;
            try { cCount = std::stoi(*it); } catch (...) {}
            ++it;
            std::string reason = *it; ++it;
            std::string date = *it; ++it;
            bool active = true;
            try { active = (std::stoi(*it) != 0); } catch (...) {}
            
            blacklists.pushBack(Blacklist(bId, cId, phone, cCount, reason, date, active));
        }
    }
    return true;
}

bool CustomerRepository::saveBlacklists(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& bl : blacklists) {
        lines.pushBack(bl.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void CustomerRepository::addBlacklist(const Blacklist& bl) {
    blacklists.pushBack(bl);
}

bool CustomerRepository::loadReviews(const std::string& path) {
    reviews.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 6) {
            auto it = tokens.begin();
            std::string rId = *it; ++it;
            std::string cId = *it; ++it;
            std::string fId = *it; ++it;
            int rtg = 5;
            try { rtg = std::stoi(*it); } catch (...) {}
            ++it;
            std::string cmt = *it; ++it;
            std::string date = *it;
            reviews.pushBack(Review(rId, cId, fId, rtg, cmt, date));
        }
    }
    return true;
}

bool CustomerRepository::saveReviews(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& rv : reviews) {
        lines.pushBack(rv.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void CustomerRepository::addReview(const Review& rv) {
    reviews.pushBack(rv);
}

void CustomerRepository::deactivateBlacklist(const std::string& customerId) {
    for (auto& bl : blacklists) {
        if (bl.getCustomerId() == customerId && bl.isActive()) {
            bl.setActive(false);
        }
    }
}

bool CustomerRepository::loadDiscounts(const std::string& path) {
    discounts.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 6) {
            auto it = tokens.begin();
            std::string dId = *it; ++it;
            std::string name = *it; ++it;
            int minB = 0, pct = 0;
            try {
                minB = std::stoi(*it); ++it;
                pct = std::stoi(*it); ++it;
            } catch (...) {}
            std::string desc = *it; ++it;
            bool act = true;
            try { act = (std::stoi(*it) != 0); } catch (...) {}
            discounts.pushBack(Discount(dId, name, minB, pct, desc, act));
        }
    }
    return true;
}

bool CustomerRepository::saveDiscounts(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& d : discounts) {
        lines.pushBack(d.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void CustomerRepository::ensureDefaultDiscounts() {
    if (discounts.empty()) {
        discounts.pushBack(Discount("DC01", "Hang Dong", 3, 5, "Giam 5% khi dat tu 3 lan", true));
        discounts.pushBack(Discount("DC02", "Hang Bac", 5, 10, "Giam 10% khi dat tu 5 lan", true));
        discounts.pushBack(Discount("DC03", "Hang Vang", 10, 15, "Giam 15% khi dat tu 10 lan", true));
        saveDiscounts("data/discounts.txt");
    }
}