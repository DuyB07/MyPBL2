#pragma once
#include <string>
#include "Repository/CustomerRepository.h"
#include "Repository/FieldRepository.h"
#include "Repository/BookingRepository.h"
#include "Repository/TournamentRepository.h"
#include "Service/CustomerService.h"
#include "Service/FieldService.h"
#include "Service/BookingService.h"
#include "Service/TournamentService.h"
#include "Service/AccountService.h"

class ConsoleApp {
private:
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

    // Tiện ích đọc dữ liệu an toàn (chống lỗi trôi lệnh & lọc ký tự '|')
    std::string readSafeLine(const std::string& prompt);
    int readInt(const std::string& prompt);
    double readDouble(const std::string& prompt);

    // Đồng bộ toàn bộ dữ liệu với thư mục data/
    void loadAllData();
    void saveAllData();

    // Các Menu điều hướng
    void showOwnerMenu();
    void showCustomerMenu();

    // Chức năng chi tiết của Chủ sân (Owner)
    void ownerViewFieldsAndSlots();
    void ownerAddField();
    void ownerToggleMaintenance();
    void ownerManageCustomersAndBlacklist();
    void ownerManageTournaments();
    void ownerViewBookingsAndRevenue();

    // Chức năng chi tiết của Khách thuê (Customer)
    void customerViewAvailableSlots();
    void customerBookField();
    void customerManageMyBookings();
    void customerTournamentMenu();
    void customerReviewMenu();

public:
    ConsoleApp();
    void run();
};