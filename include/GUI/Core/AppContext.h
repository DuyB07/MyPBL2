#pragma once
#include <string>
#include <cctype>
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
    // Tầng Repository
    CustomerRepository custRepo;
    FieldRepository fieldRepo;
    BookingRepository bookingRepo;
    TournamentRepository tourRepo;

    // Tầng Service
    CustomerService custService;
    FieldService fieldService;
    BookingService bookingService;
    TournamentService tourService;
    AccountService authService;

    // Trạng thái Toast Notification (Tự động tắt sau vài giây giống Notification.js)
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
            fieldService.createNewField("F01", "San A (Co Nhan Tao)", 5, 200000.0);
            fieldService.createNewField("F02", "San B (Tieu Chuan)", 7, 350000.0);
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
        toastExpireTime = currentTime + 4.0; // Hiển thị trong 4 giây
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