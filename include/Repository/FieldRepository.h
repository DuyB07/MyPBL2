#pragma once
#include <string>
#include "Core/LinkedList.h"
#include "Core/HashTable.h"
#include "Field/Field.h"
#include "Field/TimeSlot.h"
#include "Storage/FileManager.h"

class FieldRepository {
private:
    LinkedList<Field> fields;
    HashTable<std::string, std::string> fieldIdMap; // Key: fieldId, Value: fieldId
    LinkedList<TimeSlot> timeSlots;

public:
    FieldRepository() = default;

    bool loadFields(const std::string& path = "data/fields.txt");
    bool saveFields(const std::string& path = "data/fields.txt") const;
    bool addField(const Field& field);
    Field* findFieldById(const std::string& fieldId);
    
    // Trả về danh sách copy để an toàn khi thao tác UI
    LinkedList<Field> getAllFields() const { return fields; }

    bool loadTimeSlots(const std::string& path = "data/timeslots.txt");
    bool saveTimeSlots(const std::string& path = "data/timeslots.txt") const;
    void addTimeSlot(const TimeSlot& slot);
    
    // Lọc ra các slot thuộc về một sân cụ thể
    LinkedList<TimeSlot> getSlotsByField(const std::string& fieldId) const;
};