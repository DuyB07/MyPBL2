#include "Service/BookingService.h"
#include "Core/DateTime.h"
#include <iostream>

bool BookingService::createBooking(const std::string& bookingId, const std::string& customerId,
                                   const std::string& slotId, const std::string& date) {
    // 1. Kiểm tra khách hàng tồn tại & không nằm trong Blacklist
    Customer* cust = customerService.getCustomerById(customerId);
    if (!cust) {
        std::cerr << "  [-] Ma khach hang khong ton tai!\n";
        return false;
    }
    if (customerService.isCustomerBlacklisted(customerId)) {
        std::cerr << "  [-] Khach hang dang bi Blacklist, khong the dat san!\n";
        return false;
    }

    // 2. Kiểm tra định dạng ngày hợp lệ
    if (!DateTime::isValidDate(date)) {
        std::cerr << "  [-] Ngay dat san khong hop le!\n";
        return false;
    }

    // 3. Tách fieldId từ slotId (VD: "F01_S1" -> "F01") và kiểm tra Sân + Khung giờ tồn tại thực sự
    std::size_t underscorePos = slotId.find('_');
    if (underscorePos == std::string::npos) {
        std::cerr << "  [-] Ma khung gio khong dung dinh dang (VD: F01_S1)!\n";
        return false;
    }

    std::string fieldId = slotId.substr(0, underscorePos);
    Field* field = fieldRepo.findFieldById(fieldId);
    if (!field) {
        std::cerr << "  [-] San bong '" << fieldId << "' khong ton tai!\n";
        return false;
    }
    if (field->getStatus() != "AVAILABLE") {
        std::cerr << "  [-] San bong '" << fieldId << "' dang bao tri (MAINTENANCE), khong the dat!\n";
        return false;
    }

    // Kiểm tra slotId có thực sự tồn tại trong danh sách TimeSlot của sân này không
    bool slotExists = false;
    double price = 0.0;
    std::string startTime, endTime;
    LinkedList<TimeSlot> slots = fieldRepo.getSlotsByField(fieldId);
    for (const auto& s : slots) {
        if (s.getSlotId() == slotId) {
            slotExists = true;
            price = field->getPricePerSlot() * s.getPriceMultiplier();
            startTime = s.getStartTime();
            endTime = s.getEndTime();
            break;
        }
    }

    if (!slotExists) {
        std::cerr << "  [-] Khung gio '" << slotId << "' khong ton tai tren san " << fieldId << "!\n";
        return false;
    }

    // 4. Kiểm tra trùng lịch với các đơn Booking khác VÀ lịch thi đấu Giải đấu (TournamentMatch)
    if (bookingRepo.isSlotBookedOnDate(slotId, date)) {
        std::cerr << "  [-] Khung gio " << slotId << " ngay " << date << " da co nguoi dat!\n";
        return false;
    }
    if (tourRepo != nullptr && tourRepo->isSlotUsedByMatch(slotId, date)) {
        std::cerr << "  [-] Khung gio " << slotId << " ngay " << date << " da duoc xep cho Giai dau!\n";
        return false;
    }

    // 5. Tạo Booking và BookingDetail
    Booking b(bookingId, customerId, date, "CONFIRMED", DateTime::todayDate());
    BookingDetail d(bookingId + "_D1", bookingId, slotId, price);

    bookingRepo.addBooking(b);
    bookingRepo.addBookingDetail(d);

    // 6. Cập nhật thống kê khách hàng
    customerService.recordSuccessfulBooking(customerId);
    
    std::cout << "  [+] Dat san thanh cong! Ma don: " << bookingId
              << " | San: " << fieldId << " | Ca: " << slotId
              << " (" << startTime << " - " << endTime << ")"
              << " | Ngay: " << date << " | Gia: " << price << " VND\n";
    return true;
}

bool BookingService::cancelBooking(const std::string& bookingId, const std::string& reason) {
    Booking* b = bookingRepo.findBookingById(bookingId);
    if (!b) {
        std::cerr << "  [-] Khong tim thay ma don dat san!\n";
        return false;
    }
    if (b->getStatus() == "CANCELED") {
        std::cerr << "  [-] Don nay da bi huy truoc do!\n";
        return false;
    }
    if (b->getStatus() == "COMPLETED") {
        std::cerr << "  [-] Don da thanh toan hoan tat, khong the huy!\n";
        return false;
    }

    b->setStatus("CANCELED");
    customerService.recordCanceledBooking(b->getCustomerId(), reason, DateTime::todayDate());
    
    std::cout << "  [+] Da huy don dat san: " << bookingId << " (Khung gio da duoc giai phong)\n";
    return true;
}

bool BookingService::checkoutBooking(const std::string& bookingId) {
    Booking* b = bookingRepo.findBookingById(bookingId);
    if (!b || b->getStatus() != "CONFIRMED") return false;

    double totalAmt = 0;
    LinkedList<BookingDetail> details = bookingRepo.getDetailsByBookingId(bookingId);
    for (const auto& d : details) {
        totalAmt += d.getPrice();
    }

    double discountAmt = 0;
    double discountPercent = 0.0;
    Customer* cust = customerService.getCustomerById(b->getCustomerId()); 
    if (cust) {
        discountPercent = cust->getDiscountPercent() / 100.0; 
    }
    
    discountAmt = totalAmt * discountPercent;
    double finalAmt = totalAmt - discountAmt;

    b->setStatus("COMPLETED");
    Invoice inv("INV_" + bookingId, bookingId, totalAmt, discountAmt, finalAmt, DateTime::todayDate());
    bookingRepo.addInvoice(inv);

    std::cout << "\n  --- HOA DON THANH TOAN ---\n";
    std::cout << "  Ma Hoa Don     : " << inv.getInvoiceId() << "\n";
    std::cout << "  Ma Giao Dich   : " << bookingId << "\n";
    std::cout << "  Tong tien goc  : " << totalAmt << " VND\n";
    std::cout << "  Giam gia       : " << discountAmt << " VND (" << (discountPercent * 100) << "%)\n";
    std::cout << "  Can thanh toan : " << finalAmt << " VND\n";
    std::cout << "  --------------------------\n";

    return true;
}