#pragma once
#include <string>

class Field {
private:
    std::string fieldId;
    std::string fieldName;
    int fieldType; // 5, 7, 11
    double pricePerSlot;
    std::string status; // "AVAILABLE", "MAINTENANCE"
    std::string openTime;
    std::string closeTime;
    std::string rules;

public:
    Field(const std::string& id, const std::string& name, int type, double price,
          const std::string& stat = "AVAILABLE",
          const std::string& oTime = "06:00",
          const std::string& cTime = "21:00",
          const std::string& r = "Khong mang giay dinh sat")
        : fieldId(id), fieldName(name), fieldType(type), pricePerSlot(price),
          status(stat), openTime(oTime), closeTime(cTime), rules(r) {}

    // Getters
    std::string getFieldId() const { return fieldId; }
    std::string getFieldName() const { return fieldName; }
    int getFieldType() const { return fieldType; }
    double getPricePerSlot() const { return pricePerSlot; }
    std::string getStatus() const { return status; }
    std::string getOpenTime() const { return openTime; }
    std::string getCloseTime() const { return closeTime; }
    std::string getRules() const { return rules; }

    // Setters
    void setStatus(const std::string& newStatus) { status = newStatus; }
    void setPrice(double newPrice) { if (newPrice > 0) pricePerSlot = newPrice; }

    std::string serialize() const {
        return fieldId + "|" + fieldName + "|" + std::to_string(fieldType) + "|" +
               std::to_string(pricePerSlot) + "|" + status + "|" +
               openTime + "|" + closeTime + "|" + rules;
    }
};