#pragma once
#include "User.h"
#include <iostream>

class Customer : public User {
private:
    int bookingCount;
    int cancelCount;
    int discountPercent;
    bool blacklisted;

public:
    Customer(const std::string& id, const std::string& uname, const std::string& pwd,
             const std::string& name, const std::string& ph,
             int bCount = 0, int cCount = 0, int dPercent = 0, bool isBlacklisted = false)
        : User(id, uname, pwd, name, ph, "CUSTOMER"),
          bookingCount(bCount), cancelCount(cCount), discountPercent(dPercent), blacklisted(isBlacklisted) {}

    // Getters
    int getBookingCount() const { return bookingCount; }
    int getCancelCount() const { return cancelCount; }
    int getDiscountPercent() const { return discountPercent; }
    bool isBlacklisted() const { return blacklisted; }

    // --- Nghiệp vụ cốt lõi ---
    void addBooking() {
        bookingCount++;
        updateDiscount();
    }

    // Trừ lượt đặt tích lũy nếu đơn đặt sân bị hủy trước khi hoàn tất
    void decrementBooking() {
        if (bookingCount > 0) {
            bookingCount--;
            updateDiscount();
        }
    }

    void addCancel() {
        cancelCount++;
    }

    void resetCancelCount() {
        cancelCount = 0;
    }

    void updateDiscount() {
        if (bookingCount >= 10) discountPercent = 15;
        else if (bookingCount >= 5) discountPercent = 10;
        else if (bookingCount >= 3) discountPercent = 5;
        else discountPercent = 0;
    }

    void setBlacklist(bool status) { blacklisted = status; }

    void displayInfo() const override {
        std::cout << "[CUSTOMER] " << fullName << " (ID: " << userID << ", SDT: " << phone
                  << ") - Da dat: " << bookingCount
                  << " | Da huy: " << cancelCount
                  << " | Uu dai: " << discountPercent << "%"
                  << " | Blacklist: " << (blacklisted ? "YES" : "NO") << "\n";
    }

    std::string serialize() const {
        return "CUSTOMER|" + userID + "|" + username + "|" + password + "|" + 
               fullName + "|" + phone + "|" + std::to_string(bookingCount) + "|" + 
               std::to_string(cancelCount) + "|" + std::to_string(discountPercent) + "|" + 
               std::to_string(blacklisted);
    }
};