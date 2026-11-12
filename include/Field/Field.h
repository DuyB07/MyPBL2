#pragma once
#include <string>

class Field {
private:
    std::string fieldId;
    std::string fieldName;
    int fieldType; // 5, 7, hoặc 11
    double pricePerSlot;
    std::string status; // "AVAILABLE" hoặc "MAINTENANCE"
    std::string openTime;
    std::string closeTime;
    std::string rules;

public:
    Field(const std::string& fId, const std::string& fName, int type, double price, 
          const std::string& stat = "AVAILABLE", const std::string& oTime = "06:00", 
          const std::string& cTime = "22:00", const std::string& rls = "")
        : fieldId(fId), fieldName(fName), fieldType(type), pricePerSlot(price), 
          status(stat), openTime(oTime), closeTime(cTime), rules(rls) {}

    std::string getFieldId() const { return fieldId; }
    std::string getFieldName() const { return fieldName; }
    int getFieldType() const { return fieldType; }
    double getPricePerSlot() const { return pricePerSlot; }
    std::string getStatus() const { return status; }
    std::string getOpenTime() const { return openTime; }
    std::string getCloseTime() const { return closeTime; }
    std::string getRules() const { return rules; }

    void setStatus(const std::string& s) { status = s; }
    void setPrice(double price) { if (price >= 0) pricePerSlot = price; }

    std::string serialize() const {
        // fieldId|fieldName|fieldType|pricePerSlot|status|openTime|closeTime|rules
        return fieldId + "|" + fieldName + "|" + std::to_string(fieldType) + "|" + 
               std::to_string(pricePerSlot) + "|" + status + "|" + openTime + "|" + closeTime + "|" + rules;
    }
};