#pragma once
#include <string>
#include "Repository/BookingRepository.h"
#include "Service/CustomerService.h"
#include "Repository/FieldRepository.h"
#include "Repository/TournamentRepository.h"

class BookingService {
private:
    BookingRepository& bookingRepo;
    CustomerService& customerService;
    FieldRepository& fieldRepo;
    TournamentRepository* tourRepo; // Liên kết tùy chọn để kiểm tra trùng lịch giải đấu

public:
    BookingService(BookingRepository& bRepo, CustomerService& cService,
                   FieldRepository& fRepo, TournamentRepository* tRepo = nullptr)
        : bookingRepo(bRepo), customerService(cService), fieldRepo(fRepo), tourRepo(tRepo) {}

    bool createBooking(const std::string& bookingId, const std::string& customerId,
                       const std::string& slotId, const std::string& date);
    
    bool cancelBooking(const std::string& bookingId, const std::string& reason);

    bool checkoutBooking(const std::string& bookingId);
};