#pragma once
#include <string>

class BookingDetail {
private:
    std::string detailId;
    std::string bookingId;
    std::string slotId;
    double price; // Giá tại thời điểm đặt (đã nhân hệ số)

public:
    BookingDetail(const std::string& dId, const std::string& bId, const std::string& sId, double p)
        : detailId(dId), bookingId(bId), slotId(sId), price(p) {}

    std::string getDetailId() const { return detailId; }
    std::string getBookingId() const { return bookingId; }
    std::string getSlotId() const { return slotId; }
    double getPrice() const { return price; }

    std::string serialize() const {
        return detailId + "|" + bookingId + "|" + slotId + "|" + std::to_string(price);
    }
};