#include "Repository/BookingRepository.h"
#include <iostream>

bool BookingRepository::loadBookings(const std::string& path) {
    bookings.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 5) {
            auto it = tokens.begin();
            std::string bId = *it; ++it;
            std::string cId = *it; ++it;
            std::string bDate = *it; ++it;
            std::string stat = *it; ++it;
            std::string created = *it;
            bookings.pushBack(Booking(bId, cId, bDate, stat, created));
        }
    }
    return true;
}

bool BookingRepository::saveBookings(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& b : bookings) {
        lines.pushBack(b.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void BookingRepository::addBooking(const Booking& booking) {
    bookings.pushBack(booking);
}

Booking* BookingRepository::findBookingById(const std::string& id) {
    return bookings.findIf([&id](const Booking& b) { return b.getBookingId() == id; });
}

bool BookingRepository::loadBookingDetails(const std::string& path) {
    bookingDetails.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 4) {
            auto it = tokens.begin();
            std::string dId = *it; ++it;
            std::string bId = *it; ++it;
            std::string sId = *it; ++it;
            double price = 0;
            try { price = std::stod(*it); } catch (...) {}
            bookingDetails.pushBack(BookingDetail(dId, bId, sId, price));
        }
    }
    return true;
}

bool BookingRepository::saveBookingDetails(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& d : bookingDetails) {
        lines.pushBack(d.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void BookingRepository::addBookingDetail(const BookingDetail& detail) {
    bookingDetails.pushBack(detail);
}

LinkedList<BookingDetail> BookingRepository::getDetailsByBookingId(const std::string& bookingId) const {
    LinkedList<BookingDetail> result;
    for (const auto& d : bookingDetails) {
        if (d.getBookingId() == bookingId) result.pushBack(d);
    }
    return result;
}

bool BookingRepository::isSlotBookedOnDate(const std::string& slotId, const std::string& date) const {
    // 1. Tìm các booking diễn ra vào ngày 'date' và chưa bị hủy
    LinkedList<std::string> activeBookingIds;
    for (const auto& b : bookings) {
        if (b.getBookingDate() == date && b.getStatus() != "CANCELED") {
            activeBookingIds.pushBack(b.getBookingId());
        }
    }

    // 2. Kiểm tra xem slotId có nằm trong chi tiết của các booking này không
    for (const auto& d : bookingDetails) {
        if (d.getSlotId() == slotId) {
            bool isActive = false;
            for (const auto& actId : activeBookingIds) {
                if (actId == d.getBookingId()) {
                    isActive = true;
                    break;
                }
            }
            if (isActive) return true; // Trùng lịch!
        }
    }
    return false;
}

bool BookingRepository::loadInvoices(const std::string& path) {
    invoices.clear();
    LinkedList<std::string> lines;
    std::string error;
    if (!Storage::FileManager::readAllLines(path, lines, error)) return false;

    for (const auto& line : lines) {
        LinkedList<std::string> tokens = Storage::FileManager::split(line, '|');
        if (tokens.size() >= 6) {
            auto it = tokens.begin();
            std::string iId = *it; ++it;
            std::string bId = *it; ++it;
            double tAmt = 0, dAmt = 0, fAmt = 0;
            try {
                tAmt = std::stod(*it); ++it;
                dAmt = std::stod(*it); ++it;
                fAmt = std::stod(*it); ++it;
            } catch (...) {}
            std::string pDate = *it;
            invoices.pushBack(Invoice(iId, bId, tAmt, dAmt, fAmt, pDate));
        }
    }
    return true;
}

bool BookingRepository::saveInvoices(const std::string& path) const {
    LinkedList<std::string> lines;
    for (const auto& i : invoices) {
        lines.pushBack(i.serialize());
    }
    std::string error;
    return Storage::FileManager::writeAllLines(path, lines, error);
}

void BookingRepository::addInvoice(const Invoice& invoice) {
    invoices.pushBack(invoice);
}

LinkedList<Booking> BookingRepository::getBookingsByCustomer(const std::string& customerId) const {
    LinkedList<Booking> result;
    for (const auto& b : bookings) {
        if (b.getCustomerId() == customerId) {
            result.pushBack(b);
        }
    }
    return result;
}

double BookingRepository::calculateTotalRevenue() const {
    double total = 0.0;
    for (const auto& inv : invoices) {
        total += inv.getFinalAmount();
    }
    return total;
}