#pragma once
#include <string>
#include "Core/LinkedList.h"
#include "Booking/Booking.h"
#include "Booking/BookingDetail.h"
#include "Payment/Invoice.h"
#include "Storage/FileManager.h"

class BookingRepository {
private:
    LinkedList<Booking> bookings;
    LinkedList<BookingDetail> bookingDetails;
    LinkedList<Invoice> invoices;

public:
    BookingRepository() = default;

    // --- Bookings ---
    bool loadBookings(const std::string& path = "data/bookings.txt");
    bool saveBookings(const std::string& path = "data/bookings.txt") const;
    void addBooking(const Booking& booking);
    Booking* findBookingById(const std::string& id);
    const LinkedList<Booking>& getAllBookings() const { return bookings; }
    LinkedList<Booking> getBookingsByCustomer(const std::string& customerId) const;
    
    // --- Booking Details ---
    bool loadBookingDetails(const std::string& path = "data/booking_details.txt");
    bool saveBookingDetails(const std::string& path = "data/booking_details.txt") const;
    void addBookingDetail(const BookingDetail& detail);
    LinkedList<BookingDetail> getDetailsByBookingId(const std::string& bookingId) const;

    // Kiểm tra trùng lịch
    bool isSlotBookedOnDate(const std::string& slotId, const std::string& date) const;

    // --- Invoices & Thống kê Doanh thu ---
    bool loadInvoices(const std::string& path = "data/invoices.txt");
    bool saveInvoices(const std::string& path = "data/invoices.txt") const;
    void addInvoice(const Invoice& invoice);
    const LinkedList<Invoice>& getAllInvoices() const { return invoices; }
    double calculateTotalRevenue() const;
};