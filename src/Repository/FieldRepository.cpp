#include "Repository/FieldRepository.h"

bool FieldRepository::loadFields(const std::string& path) {
    fields.clear();
    fieldIdMap.clear();
    
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        // fieldId|fieldName|fieldType|pricePerSlot|status|openTime|closeTime|rules
        if (tokens.size() >= 5) { // Tối thiểu 5 trường như bản cũ
            auto it = tokens.begin();
            std::string id = *it; ++it;
            std::string name = *it; ++it;
            int type = std::stoi(*it); ++it;
            double price = std::stod(*it); ++it;
            std::string status = *it; ++it;
            
            std::string oTime = "06:00", cTime = "22:00", rules = "";
            if (it != tokens.end()) { oTime = *it; ++it; }
            if (it != tokens.end()) { cTime = *it; ++it; }
            if (it != tokens.end()) { rules = *it; }

            Field f(id, name, type, price, status, oTime, cTime, rules);
            fields.pushBack(f);
            fieldIdMap.insert(id, id);
        }
    }
    return true;
}

bool FieldRepository::saveFields(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& f : fields) {
        lines.pushBack(f.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

bool FieldRepository::addField(const Field& field) {
    if (findFieldById(field.getFieldId()) != nullptr) return false; // Trùng ID
    fields.pushBack(field);
    fieldIdMap.insert(field.getFieldId(), field.getFieldId());
    return true;
}

Field* FieldRepository::findFieldById(const std::string& fieldId) {
    std::string* idPtr = fieldIdMap.find(fieldId);
    if (idPtr) {
        return fields.findIf([&fieldId](const Field& f) { return f.getFieldId() == fieldId; });
    }
    return nullptr;
}

bool FieldRepository::loadTimeSlots(const std::string& path) {
    timeSlots.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 5) {
            auto it = tokens.begin();
            std::string sId = *it; ++it;
            std::string fId = *it; ++it;
            std::string sTime = *it; ++it;
            std::string eTime = *it; ++it;
            double mult = std::stod(*it);
            timeSlots.pushBack(TimeSlot(sId, fId, sTime, eTime, mult));
        }
    }
    return true;
}

bool FieldRepository::saveTimeSlots(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& ts : timeSlots) {
        lines.pushBack(ts.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void FieldRepository::addTimeSlot(const TimeSlot& slot) {
    timeSlots.pushBack(slot);
}

LinkedList<TimeSlot> FieldRepository::getSlotsByField(const std::string& fieldId) const {
    LinkedList<TimeSlot> result;
    for (const auto& ts : timeSlots) {
        if (ts.getFieldId() == fieldId) {
            result.pushBack(ts);
        }
    }
    return result;
}