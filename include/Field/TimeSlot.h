#pragma once
#include <string>

class TimeSlot {
private:
    std::string slotId;
    std::string fieldId;
    std::string startTime; // Định dạng "HH:MM"
    std::string endTime;   // Định dạng "HH:MM"
    double priceMultiplier; // Hệ số giá (mặc định 1.0)
    bool isAvailable;       // Trạng thái trống (chỉ dùng trong phiên giao dịch, không lưu file)

public:
    TimeSlot(const std::string& sId, const std::string& fId, const std::string& sTime, const std::string& eTime, double multiplier = 1.0)
        : slotId(sId), fieldId(fId), startTime(sTime), endTime(eTime), priceMultiplier(multiplier), isAvailable(true) {}

    std::string getSlotId() const { return slotId; }
    std::string getFieldId() const { return fieldId; }
    std::string getStartTime() const { return startTime; }
    std::string getEndTime() const { return endTime; }
    double getPriceMultiplier() const { return priceMultiplier; }
    bool getIsAvailable() const { return isAvailable; }

    void setPriceMultiplier(double mult) { if (mult > 0) priceMultiplier = mult; }
    void setAvailable(bool status) { isAvailable = status; }

    std::string serialize() const {
        return slotId + "|" + fieldId + "|" + startTime + "|" + endTime + "|" + std::to_string(priceMultiplier);
    }
};