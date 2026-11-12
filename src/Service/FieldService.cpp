#include "Service/FieldService.h"
#include <iostream>
#include <iomanip>
#include <sstream>

bool FieldService::createNewField(const std::string& id, const std::string& name, int type, double price) {
    if (type != 5 && type != 7 && type != 11) {
        std::cerr << "Loi: Loai san chi duoc la 5, 7 hoac 11!\n";
        return false;
    }
    Field newField(id, name, type, price);
    if (fieldRepo.addField(newField)) {
        generateDefaultTimeSlots(id); // Tự động sinh slot khi tạo sân mới
        return true;
    }
    return false;
}

bool FieldService::setFieldMaintenance(const std::string& fieldId, bool isMaintenance) {
    Field* f = fieldRepo.findFieldById(fieldId);
    if (!f) return false;
    
    f->setStatus(isMaintenance ? "MAINTENANCE" : "AVAILABLE");
    return true;
}

void FieldService::generateDefaultTimeSlots(const std::string& fieldId) {
    Field* f = fieldRepo.findFieldById(fieldId);
    if (!f) return;

    // Giả lập logic sinh slot 90 phút (06:00 -> 07:30, 07:30 -> 09:00...)
    // Trong thực tế cần dùng thư viện <chrono> hoặc DateTime.h để xử lý chuẩn hơn.
    // Ở đây dùng thuật toán đơn giản với số nguyên để minh họa.
    int startHour = std::stoi(f->getOpenTime().substr(0, 2));
    int startMin = std::stoi(f->getOpenTime().substr(3, 2));
    int endHour = std::stoi(f->getCloseTime().substr(0, 2));
    
    int currentTotalMinutes = startHour * 60 + startMin;
    int endTotalMinutes = endHour * 60;
    int slotIndex = 1;

    while (currentTotalMinutes + 90 <= endTotalMinutes) {
        int sH = currentTotalMinutes / 60;
        int sM = currentTotalMinutes % 60;
        
        currentTotalMinutes += 90;
        
        int eH = currentTotalMinutes / 60;
        int eM = currentTotalMinutes % 60;

        char sTimeBuf[6], eTimeBuf[6];
        snprintf(sTimeBuf, sizeof(sTimeBuf), "%02d:%02d", sH, sM);
        snprintf(eTimeBuf, sizeof(eTimeBuf), "%02d:%02d", eH, eM);

        std::string slotId = fieldId + "_S" + std::to_string(slotIndex++);
        double multiplier = (sH >= 17) ? 1.2 : 1.0; // Giờ vàng (sau 17h) giá x1.2

        TimeSlot ts(slotId, fieldId, sTimeBuf, eTimeBuf, multiplier);
        fieldRepo.addTimeSlot(ts);
    }
}

LinkedList<Field> FieldService::getFieldsByType(int type) const {
    LinkedList<Field> result;
    for (const auto& f : fieldRepo.getAllFields()) {
        if (f.getFieldType() == type) {
            result.pushBack(f);
        }
    }
    return result;
}