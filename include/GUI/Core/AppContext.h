#pragma once
#include <string>
#include <cctype>
#include "Core/DateTime.h"
#include "Repository/CustomerRepository.h"
#include "Repository/FieldRepository.h"
#include "Repository/BookingRepository.h"
#include "Repository/TournamentRepository.h"
#include "Service/CustomerService.h"
#include "Service/FieldService.h"
#include "Service/BookingService.h"
#include "Service/TournamentService.h"
#include "Service/AccountService.h"

class AppContext {
public:
    CustomerRepository custRepo;
    FieldRepository fieldRepo;
    BookingRepository bookingRepo;
    TournamentRepository tourRepo;

    CustomerService custService;
    FieldService fieldService;
    BookingService bookingService;
    TournamentService tourService;
    AccountService authService;

    std::string toastMessage;
    bool toastIsError;
    double toastExpireTime;

    AppContext()
        : custService(custRepo),
          fieldService(fieldRepo),
          bookingService(bookingRepo, custService, fieldRepo, &tourRepo),
          tourService(tourRepo, fieldRepo, bookingRepo, custService),
          authService(custRepo),
          toastIsError(false),
          toastExpireTime(0.0) {
        loadAllData();
    }

    void loadAllData() {
        custRepo.loadCustomers("data/customers.txt");
        custRepo.loadBlacklists("data/blacklist.txt");
        custRepo.loadReviews("data/reviews.txt");
        custRepo.loadDiscounts("data/discounts.txt");
        custRepo.ensureDefaultDiscounts();

        fieldRepo.loadFields("data/fields.txt");
        fieldRepo.loadTimeSlots("data/timeslots.txt");

        bookingRepo.loadBookings("data/bookings.txt");
        bookingRepo.loadBookingDetails("data/booking_details.txt");
        bookingRepo.loadInvoices("data/invoices.txt");

        tourRepo.loadTournaments("data/tournaments.txt");
        tourRepo.loadTeams("data/teams.txt");
        tourRepo.loadMatches("data/matches.txt");

        authService.loadOwners("data/accounts.txt");
        authService.ensureDefaultOwner();

        if (fieldRepo.getAllFields().empty()) {
            fieldService.createNewField("F01", "Sân A (Cỏ Nhân Tạo)", 5, 200000.0);
            fieldService.createNewField("F02", "Sân B (Tiêu Chuẩn)", 7, 350000.0);
            fieldService.createNewField("F03", "Sân C (Khán Đài)", 7, 250000.0);
            fieldRepo.saveFields("data/fields.txt");
            fieldRepo.saveTimeSlots("data/timeslots.txt");
        } else {
            bool addedSlots = false;
            for (const auto& f : fieldRepo.getAllFields()) {
                if (fieldRepo.getSlotsByField(f.getFieldId()).empty()) {
                    fieldService.generateDefaultTimeSlots(f.getFieldId());
                    addedSlots = true;
                }
            }
            if (addedSlots) fieldRepo.saveTimeSlots("data/timeslots.txt");
        }

        // Tự động tạo dữ liệu mẫu ban đầu nếu hệ thống chưa có khách hàng nào
        if (custRepo.getAllCustomers().empty()) {
            seedInitialSampleData();
        }
    }

    void seedInitialSampleData() {
        std::string today = DateTime::todayDate();

        // 1. Tạo 3 khách hàng mẫu
        custService.registerNewCustomer("C01", "khachA", "123", "Nguyễn Văn An", "0905111222");
        custService.registerNewCustomer("C02", "khachB", "123", "Trần Minh Bảo", "0905333444");
        custService.registerNewCustomer("C03", "khachC", "123", "Lê Hoàng Nam", "0905666777");

        // 2. Tạo các phiếu đặt sân & thanh toán mẫu
        bookingService.createBooking("B101", "C01", "F01_S1", today);
        bookingService.checkoutBooking("B101");

        bookingService.createBooking("B102", "C02", "F01_S9", today);
        bookingService.checkoutBooking("B102");

        bookingService.createBooking("B103", "C01", "F02_S3", today); // Đang chờ thanh toán (CONFIRMED)

        // 3. Tạo đánh giá mẫu
        if (custRepo.getAllReviews().empty()) {
            custRepo.addReview(Review("RV101", "C01", "F01", 5, "Mặt cỏ mới, đèn chiếu sáng rất tốt!", today));
            custRepo.addReview(Review("RV102", "C02", "F02", 4, "Sân rộng thoáng, có chỗ để xe an toàn.", today));
        }

        // 4. Tạo 1 giải đấu mẫu đang chờ duyệt (PENDING) để Chủ sân bấm thử Queue
        if (tourRepo.getAllTournaments().empty()) {
            tourService.requestTournament("T01", "Cúp Sinh Viên Bách Khoa", "C01", "F01", today, 4);
            tourService.registerTeam("TM01", "T01", "FC Rồng Vàng", "0911222333");
            tourService.registerTeam("TM02", "T01", "FC Tia Chớp", "0911444555");
            tourService.registerTeam("TM03", "T01", "FC Đại Bàng", "0911777888");
        }

        saveAllData();
    }

    void saveAllData() {
        authService.saveOwners("data/accounts.txt");
        custRepo.saveCustomers("data/customers.txt");
        custRepo.saveBlacklists("data/blacklist.txt");
        custRepo.saveReviews("data/reviews.txt");
        custRepo.saveDiscounts("data/discounts.txt");

        fieldRepo.saveFields("data/fields.txt");
        fieldRepo.saveTimeSlots("data/timeslots.txt");

        bookingRepo.saveBookings("data/bookings.txt");
        bookingRepo.saveBookingDetails("data/booking_details.txt");
        bookingRepo.saveInvoices("data/invoices.txt");

        tourRepo.saveTournaments("data/tournaments.txt");
        tourRepo.saveTeams("data/teams.txt");
        tourRepo.saveMatches("data/matches.txt");
    }

    void notify(const std::string& msg, bool isError, double currentTime) {
        toastMessage = msg;
        toastIsError = isError;
        toastExpireTime = currentTime + 4.0;
    }

    static std::string sanitize(const char* raw) {
        if (!raw) return "";
        std::string s(raw);
        for (char& c : s) {
            if (c == '|') c = ' ';
        }
        return Storage::FileManager::trim(s);
    }

    static std::string toUpper(std::string s) {
        for (char& c : s) {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
        return s;
    }
};