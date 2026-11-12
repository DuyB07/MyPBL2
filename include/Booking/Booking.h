#pragma once
#include <string>

class Booking {
private:
    std::string bookingId;
    std::string customerId;
    std::string bookingDate; // YYYY-MM-DD
    std::string status;      // "PENDING", "CONFIRMED", "CANCELED", "COMPLETED"
    std::string createdAt;   // Thời điểm tạo đơn

public:
    Booking(const std::string& bId, const std::string& cId, const std::string& bDate, 
            const std::string& stat = "PENDING", const std::string& created = "")
        : bookingId(bId), customerId(cId), bookingDate(bDate), status(stat), createdAt(created) {}

    std::string getBookingId() const { return bookingId; }
    std::string getCustomerId() const { return customerId; }
    std::string getBookingDate() const { return bookingDate; }
    std::string getStatus() const { return status; }
    std::string getCreatedAt() const { return createdAt; }

    void setStatus(const std::string& s) { status = s; }

    std::string serialize() const {
        return bookingId + "|" + customerId + "|" + bookingDate + "|" + status + "|" + createdAt;
    }
};