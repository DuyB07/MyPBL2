#pragma once
#include <string>
#include <cctype>

class DateTime {
public:
    static bool isLeapYear(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    // Kiểm tra định dạng YYYY-MM-DD hợp lệ
    static bool isValidDate(const std::string& dateStr) {
        if (dateStr.length() != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
            return false;
        }
        for (std::size_t i = 0; i < dateStr.length(); ++i) {
            if (i == 4 || i == 7) continue;
            if (!std::isdigit(static_cast<unsigned char>(dateStr[i]))) return false;
        }

        int year = std::stoi(dateStr.substr(0, 4));
        int month = std::stoi(dateStr.substr(5, 2));
        int day = std::stoi(dateStr.substr(8, 2));

        if (year < 2020 || year > 2100) return false;
        if (month < 1 || month > 12) return false;

        int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (isLeapYear(year)) daysInMonth[2] = 29;

        return day >= 1 && day <= daysInMonth[month];
    }

    // Kiểm tra định dạng HH:MM hợp lệ
    static bool isValidTime(const std::string& timeStr) {
        if (timeStr.length() != 5 || timeStr[2] != ':') return false;
        if (!std::isdigit(static_cast<unsigned char>(timeStr[0])) ||
            !std::isdigit(static_cast<unsigned char>(timeStr[1])) ||
            !std::isdigit(static_cast<unsigned char>(timeStr[3])) ||
            !std::isdigit(static_cast<unsigned char>(timeStr[4]))) {
            return false;
        }
        int hour = std::stoi(timeStr.substr(0, 2));
        int minute = std::stoi(timeStr.substr(3, 2));
        return (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
    }

    static std::string todayDate() {
        return "2026-11-07";
    }
};