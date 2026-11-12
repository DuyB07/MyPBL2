#pragma once
#include <string>

class Discount {
private:
    std::string discountId;
    std::string name;
    int minBookings;
    int discountPercent;
    std::string description;
    bool active;

public:
    Discount(const std::string& dId, const std::string& n, int minB, int pct, const std::string& desc, bool isActive = true)
        : discountId(dId), name(n), minBookings(minB), discountPercent(pct), description(desc), active(isActive) {}

    bool isEligible(int customerBookingCount) const {
        return active && (customerBookingCount >= minBookings);
    }

    std::string serialize() const {
        return discountId + "|" + name + "|" + std::to_string(minBookings) + "|" + 
               std::to_string(discountPercent) + "|" + description + "|" + std::to_string(active);
    }
};