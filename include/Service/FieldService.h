#pragma once
#include <string>
#include "Repository/FieldRepository.h"

class FieldService {
private:
    FieldRepository& fieldRepo;

public:
    FieldService(FieldRepository& repo) : fieldRepo(repo) {}

    bool createNewField(const std::string& id, const std::string& name, int type, double price);
    bool setFieldMaintenance(const std::string& fieldId, bool isMaintenance);
    
    // Tự động sinh các khung giờ 90 phút cho một sân từ openTime đến closeTime
    void generateDefaultTimeSlots(const std::string& fieldId);
    
    LinkedList<Field> getFieldsByType(int type) const;
};