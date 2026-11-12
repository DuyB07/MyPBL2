#include "UI/ConsoleApp.h"
#include "Core/DateTime.h"
#include <iostream>
#include <cctype>

// static std::string toUpperCode(std::string s) {
//     for (char& c : s) {
//         c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
//     }
//     return s;
// }

ConsoleApp::ConsoleApp()
    : custService(custRepo),
      fieldService(fieldRepo),
      bookingService(bookingRepo, custService, fieldRepo, &tourRepo),
      tourService(tourRepo, fieldRepo, bookingRepo, custService),
      authService(custRepo) {
    loadAllData();
}

std::string ConsoleApp::readSafeLine(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    if (!std::getline(std::cin, input)) return "";
    for (char& c : input) {
        if (c == '|') c = ' ';
    }
    return Storage::FileManager::trim(input);
}

int ConsoleApp::readInt(const std::string& prompt) {
    while (true) {
        std::string line = readSafeLine(prompt);
        try {
            return std::stoi(line);
        } catch (...) {
            std::cout << "  [!] Vui long nhap mot so nguyen hop le!\n";
        }
    }
}

double ConsoleApp::readDouble(const std::string& prompt) {
    while (true) {
        std::string line = readSafeLine(prompt);
        try {
            return std::stod(line);
        } catch (...) {
            std::cout << "  [!] Vui long nhap mot so thuc hop le!\n";
        }
    }
}

void ConsoleApp::loadAllData() {
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

    // Khởi tạo sân mặc định nếu chưa có sân nào
    if (fieldRepo.getAllFields().empty()) {
        fieldService.createNewField("F01", "San A (Co Nhan Tao)", 5, 200000.0);
        fieldService.createNewField("F02", "San B (Tieu Chuan)", 7, 350000.0);
        fieldRepo.saveFields("data/fields.txt");
        fieldRepo.saveTimeSlots("data/timeslots.txt");
    } else {
        // Đảm bảo mọi sân trong fields.txt đều đã có TimeSlot trong timeslots.txt
        bool addedSlots = false;
        for (const auto& f : fieldRepo.getAllFields()) {
            if (fieldRepo.getSlotsByField(f.getFieldId()).empty()) {
                fieldService.generateDefaultTimeSlots(f.getFieldId());
                addedSlots = true;
            }
        }
        if (addedSlots) {
            fieldRepo.saveTimeSlots("data/timeslots.txt");
        }
    }
}

void ConsoleApp::saveAllData() {
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

void ConsoleApp::run() {
    while (true) {
        std::cout << "\n======================================================\n";
        std::cout << "   HE THONG QUAN LY SAN BONG MINI (PBL2 - C++17)\n";
        std::cout << "======================================================\n";
        if (Owner* own = authService.getPrimaryOwner()) {
            std::cout << " Cum san : " << own->getStadiumName() << "\n";
            std::cout << " Dia chi : " << own->getAddress() << "\n";
            std::cout << " Gio mo  : " << own->getOpenTime() << " - " << own->getCloseTime()
                      << " | Noi quy: " << own->getFieldRules() << "\n";
            std::cout << "------------------------------------------------------\n";
        }
        std::cout << " 1. Dang nhap (Chu san / Khach hang)\n";
        std::cout << " 2. Dang ky tai khoan Khach hang moi\n";
        std::cout << " 3. Xem danh sach san bong & Danh gia\n";
        std::cout << " 0. Luu du lieu & Thoat chuong trinh\n";
        std::cout << "======================================================\n";

        int choice = readInt("Chon chuc nang: ");
        if (choice == 0) {
            saveAllData();
            std::cout << "[+] Da luu toan bo du lieu xuong thu muc data/. Tam biet!\n";
            break;
        } else if (choice == 1) {
            std::string uname = readSafeLine("  Ten dang nhap: ");
            std::string pwd = readSafeLine("  Mat khau: ");
            User* user = authService.login(uname, pwd);
            if (!user) {
                std::cout << "  [-] Sai ten dang nhap hoac mat khau!\n";
            } else {
                std::cout << "  [+] Dang nhap thanh cong! Xin chao: ";
                user->displayInfo();
                if (user->getRole() == "OWNER") {
                    showOwnerMenu();
                } else {
                    showCustomerMenu();
                }
            }
        } else if (choice == 2) {
            int nextNum = static_cast<int>(custRepo.getAllCustomers().size()) + 1;
            std::string id;
            do {
                id = "C" + std::to_string(nextNum++);
            } while (custRepo.findById(id) != nullptr);

            std::string uname = readSafeLine("  Ten dang nhap moi: ");
            std::string pwd = readSafeLine("  Mat khau: ");
            std::string name = readSafeLine("  Ho va ten: ");
            std::string phone = readSafeLine("  So dien thoai (9-11 so): ");

            if (custService.registerNewCustomer(id, uname, pwd, name, phone)) {
                saveAllData();
                std::cout << "  [+] Dang ky thanh cong! Ma KH cua ban la: " << id << "\n";
            }
        } else if (choice == 3) {
            ownerViewFieldsAndSlots();
            std::cout << "\n--- DANH GIA CUA KHACH HANG ---\n";
            if (custRepo.getAllReviews().empty()) {
                std::cout << "  (Chua co danh gia nao)\n";
            }
            for (const auto& rv : custRepo.getAllReviews()) {
                std::cout << "  [" << rv.getFieldId() << "] Khach " << rv.getCustomerId()
                          << " (" << rv.getRating() << "/5 sao): " << rv.getComment()
                          << " (" << rv.getCreatedDate() << ")\n";
            }
        } else {
            std::cout << "  [!] Lua chon khong hop le!\n";
        }
    }
}

// ============================================================================
// MENU CHỦ SÂN (OWNER)
// ============================================================================
void ConsoleApp::showOwnerMenu() {
    while (authService.getCurrentUser() != nullptr) {
        std::cout << "\n---------------- MENU CHU SAN (OWNER) ----------------\n";
        std::cout << " 1. Xem danh sach san bong & Tinh trang lich theo ngay\n";
        std::cout << " 2. Them san bong moi (Tu dong sinh TimeSlot)\n";
        std::cout << " 3. Chuyen trang thai van hanh San (MO CUA / BAO TRI)\n";
        std::cout << " 4. Quan ly Danh ba Khach hang & Blacklist (HashTable O(1))\n";
        std::cout << " 5. Quan ly Giai dau & Xep lich (Queue<TournamentMatch>)\n";
        std::cout << " 6. Quan ly Don dat san, Thanh toan & Thong ke Doanh thu\n";
        std::cout << " 0. Dang xuat\n";
        std::cout << "------------------------------------------------------\n";

        int choice = readInt("Chon chuc nang: ");
        switch (choice) {
            case 1: ownerViewFieldsAndSlots(); break;
            case 2: ownerAddField(); break;
            case 3: ownerToggleMaintenance(); break;
            case 4: ownerManageCustomersAndBlacklist(); break;
            case 5: ownerManageTournaments(); break;
            case 6: ownerViewBookingsAndRevenue(); break;
            case 0:
                saveAllData();
                authService.logout();
                std::cout << "[+] Da dang xuat.\n";
                break;
            default:
                std::cout << "[!] Lua chon khong hop le!\n";
        }
    }
}

void ConsoleApp::ownerViewFieldsAndSlots() {
    std::cout << "\n--- DANH SACH SAN BONG HIEN CO ---\n";
    for (const auto& f : fieldRepo.getAllFields()) {
        LinkedList<TimeSlot> slots = fieldRepo.getSlotsByField(f.getFieldId());
        std::string opStatus = (f.getStatus() == "AVAILABLE")
                               ? "DANG MO CUA (AVAILABLE)"
                               : "DANG BAO TRI (MAINTENANCE)";
        std::cout << " * Ma san: " << f.getFieldId()
                  << " | Ten: " << f.getFieldName()
                  << " | Loai: San " << f.getFieldType() << " nguoi"
                  << " | Gia goc/ca: " << f.getPricePerSlot() << " VND"
                  << " | So ca/ngay: " << slots.size()
                  << " | Tinh trang san: " << opStatus << "\n";
    }
}

void ConsoleApp::ownerAddField() {
    std::string id = readSafeLine("  Nhap ma san moi (VD: F03): ");
    std::string name = readSafeLine("  Nhap ten san: ");
    int type = readInt("  Nhap loai san (5, 7 hoac 11): ");
    double price = readDouble("  Nhap gia thue moi ca (VND): ");

    if (fieldService.createNewField(id, name, type, price)) {
        saveAllData();
        std::cout << "  [+] Tao san moi va sinh 10 khung gio tu dong thanh cong!\n";
    } else {
        std::cout << "  [-] Tao san that bai (Ma san da ton tai hoac sai loai san 5/7/11)!\n";
    }
}

void ConsoleApp::ownerToggleMaintenance() {
    ownerViewFieldsAndSlots();
    std::string id = readSafeLine("  Nhap ma san can doi trang thai: ");
    int mode = readInt("  Chon trang thai (1 = DANG MO CUA, 2 = DANG BAO TRI): ");
    if (fieldService.setFieldMaintenance(id, mode == 2)) {
        saveAllData();
        std::cout << "  [+] Cap nhat trang thai van hanh san thanh cong!\n";
    } else {
        std::cout << "  [-] Khong tim thay ma san!\n";
    }
}

void ConsoleApp::ownerManageCustomersAndBlacklist() {
    std::cout << "\n--- DANH BA KHACH HANG ---\n";
    for (const auto& c : custRepo.getAllCustomers()) {
        std::cout << " * ID: " << c.getUserID()
                  << " | User: " << c.getUsername()
                  << " | Ten: " << c.getFullName()
                  << " | SDT: " << c.getPhone()
                  << " | Dat: " << c.getBookingCount()
                  << " | Huy: " << c.getCancelCount()
                  << " | Uu dai: " << c.getDiscountPercent() << "%"
                  << " | Blacklist: " << (c.isBlacklisted() ? "BI CAM" : "KHONG") << "\n";
    }

    if (!custRepo.getAllBlacklists().empty()) {
        std::cout << "\n--- LICH SU BAN GHI BLACKLIST ---\n";
        for (const auto& bl : custRepo.getAllBlacklists()) {
            std::cout << " * Ma BL: " << bl.getBlacklistId()
                      << " | Khach: " << bl.getCustomerId()
                      << " | SDT: " << bl.getPhone()
                      << " | Hieu luc: " << (bl.isActive() ? "DANG CAM" : "DA GO") << "\n";
        }
    }

    std::cout << "\n 1. Tra cuu nhanh Khach hang theo SDT (HashTable O(1))\n";
    std::cout << " 2. Go phong toa (Unban) Khach hang trong Blacklist\n";
    std::cout << " 0. Quay lai\n";
    int sub = readInt("Chon: ");
    if (sub == 1) {
        std::string phone = readSafeLine("  Nhap SDT can tim: ");
        Customer* found = custService.getCustomerByPhone(phone);
        if (found) {
            std::cout << "  [+] Tim thay: ";
            found->displayInfo();
        } else {
            std::cout << "  [-] Khong tim thay khach hang co SDT nay!\n";
        }
    } else if (sub == 2) {
        std::string cid = readSafeLine("  Nhap Ma KH can go Blacklist (VD: C01): ");
        if (custService.unbanCustomer(cid)) {
            saveAllData();
        } else {
            std::cout << "  [-] Khach hang khong ton tai hoac khong nam trong Blacklist!\n";
        }
    }
}

void ConsoleApp::ownerManageTournaments() {
    std::cout << "\n--- DANH SACH GIAI DAU ---\n";
    if (tourRepo.getAllTournaments().empty()) {
        std::cout << "  (Chua co giai dau nao)\n";
    }
    for (const auto& t : tourRepo.getAllTournaments()) {
        LinkedList<Team> tTeams = tourRepo.getTeamsByTournament(t.getTournamentId());
        std::cout << " * Ma giai: " << t.getTournamentId()
                  << " | Ten: " << t.getName()
                  << " | Khach: " << t.getCustomerId()
                  << " | San: " << t.getFieldId()
                  << " | Ngay: " << t.getStartDate()
                  << " | So doi: " << tTeams.size() << "/" << t.getMaxTeams()
                  << " | Trang thai: " << t.getStatus() << "\n";

        LinkedList<TournamentMatch> matches = tourRepo.getMatchesByTournament(t.getTournamentId());
        for (const auto& m : matches) {
            std::cout << "    -> Tran " << m.getMatchId() << " [" << m.getSlotId() << "]: "
                      << m.getTeamAId() << " vs " << m.getTeamBId();
            if (m.isCompleted()) {
                std::cout << " | Ti so: " << m.getScoreA() << " - " << m.getScoreB() << " (Hoan tat)\n";
            } else {
                std::cout << " (Chua cap nhat ti so)\n";
            }
        }

        TournamentResult res("", "", "", 0);
        if (tourService.calculateTournamentResult(t.getTournamentId(), res)) {
            std::cout << "    => [KET QUA] Vo dich: " << res.getChampionTeamId()
                      << " | A quan: " << res.getRunnerUpTeamId()
                      << " (Da dau: " << res.getTotalMatchesPlayed() << " tran)\n";
        }
    }

    std::cout << "\n 1. Duyet giai dau & Xep lich tu dong qua Queue<TournamentMatch>\n";
    std::cout << " 2. Cap nhat ti so tran dau\n";
    std::cout << " 0. Quay lai\n";
    int sub = readInt("Chon: ");
    if (sub == 1) {
        std::string tid = readSafeLine("  Nhap Ma giai can duyet: ");
        if (tourService.approveAndScheduleTournament(tid)) {
            saveAllData();
        }
    } else if (sub == 2) {
        std::string mid = readSafeLine("  Nhap Ma tran dau (VD: T01_M1): ");
        int sA = readInt("  Ban thang Doi A: ");
        int sB = readInt("  Ban thang Doi B: ");
        if (tourService.recordMatchScore(mid, sA, sB)) {
            saveAllData();
        } else {
            std::cout << "  [-] Khong tim thay ma tran dau!\n";
        }
    }
}

void ConsoleApp::ownerViewBookingsAndRevenue() {
    std::cout << "\n--- DANH SACH DON DAT SAN ---\n";
    if (bookingRepo.getAllBookings().empty()) {
        std::cout << "  (Chua co don dat san nao)\n";
    }
    for (const auto& b : bookingRepo.getAllBookings()) {
        std::cout << " * Ma Don: " << b.getBookingId()
                  << " | Khach: " << b.getCustomerId()
                  << " | Ngay: " << b.getBookingDate()
                  << " | Trang thai: " << b.getStatus() << "\n";
        for (const auto& d : bookingRepo.getDetailsByBookingId(b.getBookingId())) {
            std::cout << "    -> Slot: " << d.getSlotId() << " | Gia: " << d.getPrice() << " VND\n";
        }
    }

    std::cout << "\n--- THONG KE HOA DON & DOANH THU ---\n";
    for (const auto& inv : bookingRepo.getAllInvoices()) {
        std::cout << " * Hoa don: " << inv.getInvoiceId()
                  << " | Don: " << inv.getBookingId()
                  << " | Goc: " << inv.getTotalAmount()
                  << " | Giam: " << inv.getDiscountAmount()
                  << " | Thuc thu: " << inv.getFinalAmount() << " VND\n";
    }
    std::cout << " ==> TONG DOANH THU THUC THU: " << bookingRepo.calculateTotalRevenue() << " VND\n";

    std::cout << "\n 1. Xac nhan Thanh toan & Xuat hoa don cho Don dat san\n";
    std::cout << " 0. Quay lai\n";
    if (readInt("Chon: ") == 1) {
        std::string bid = readSafeLine("  Nhap Ma don can thanh toan: ");
        if (bookingService.checkoutBooking(bid)) {
            saveAllData();
        } else {
            std::cout << "  [-] Don khong ton tai hoac khong o trang thai CONFIRMED!\n";
        }
    }
}

// ============================================================================
// MENU KHÁCH THUÊ SÂN (CUSTOMER)
// ============================================================================
void ConsoleApp::showCustomerMenu() {
    while (authService.getCurrentUser() != nullptr) {
        User* u = authService.getCurrentUser();
        Customer* cust = custService.getCustomerById(u->getUserID());

        std::cout << "\n-------------- MENU KHACH HANG (CUSTOMER) -------------\n";
        if (cust) {
            std::cout << " Khach hang: " << cust->getFullName() << " (" << cust->getUserID() << ")"
                      << " | Luot dat: " << cust->getBookingCount()
                      << " | Luot huy: " << cust->getCancelCount()
                      << " | Uu dai: " << cust->getDiscountPercent() << "%"
                      << " | Trang thai: " << (cust->isBlacklisted() ? "BI CAM (BLACKLIST)" : "HOAT DONG") << "\n";
            std::cout << "-------------------------------------------------------\n";
        }
        std::cout << " 1. Tra cuu Tinh trang San & Khung gio theo ngay\n";
        std::cout << " 2. Dat san bong\n";
        std::cout << " 3. Lich su dat san cua toi (Thanh toan / Huy san)\n";
        std::cout << " 4. Dang ky To chuc Giai dau & Them doi bong\n";
        std::cout << " 5. Gui danh gia (Review) chat luong san bong\n";
        std::cout << " 0. Dang xuat\n";
        std::cout << "-------------------------------------------------------\n";

        int choice = readInt("Chon chuc nang: ");
        switch (choice) {
            case 1: customerViewAvailableSlots(); break;
            case 2: customerBookField(); break;
            case 3: customerManageMyBookings(); break;
            case 4: customerTournamentMenu(); break;
            case 5: customerReviewMenu(); break;
            case 0:
                saveAllData();
                authService.logout();
                std::cout << "[+] Da dang xuat.\n";
                break;
            default:
                std::cout << "[!] Lua chon khong hop le!\n";
        }
    }
}

void ConsoleApp::customerViewAvailableSlots() {
    std::string date = readSafeLine("  Nhap ngay muon kiem tra (YYYY-MM-DD, Enter de chon hom nay " + DateTime::todayDate() + "): ");
    if (date.empty()) date = DateTime::todayDate();
    if (!DateTime::isValidDate(date)) {
        std::cout << "  [-] Dinh dang ngay khong hop le!\n";
        return;
    }

    std::cout << "\n--- TONG QUAN TINH TRANG CAC SAN NGAY " << date << " ---\n";
    for (const auto& f : fieldRepo.getAllFields()) {
        LinkedList<TimeSlot> slots = fieldRepo.getSlotsByField(f.getFieldId());
        int totalSlots = 0, freeSlots = 0;
        std::string bookedList = "";

        for (const auto& s : slots) {
            totalSlots++;
            bool b = bookingRepo.isSlotBookedOnDate(s.getSlotId(), date);
            bool m = tourRepo.isSlotUsedByMatch(s.getSlotId(), date);
            if (!b && !m) {
                freeSlots++;
            } else {
                if (!bookedList.empty()) bookedList += ", ";
                bookedList += s.getSlotId() + "(" + s.getStartTime() + ")";
            }
        }

        std::cout << " * [" << f.getFieldId() << "] " << f.getFieldName()
                  << " (San " << f.getFieldType() << " nguoi)"
                  << " | Van hanh: " << (f.getStatus() == "AVAILABLE" ? "MO CUA" : "BAO TRI")
                  << " | Con trong: " << freeSlots << "/" << totalSlots << " ca";
        if (!bookedList.empty()) {
            std::cout << " | Ca da kin: [" << bookedList << "]";
        }
        std::cout << "\n";
    }

    std::string fieldId = readSafeLine("\n  Nhap Ma san muon xem chi tiet tung ca (VD: F01): ");
    Field* f = fieldRepo.findFieldById(fieldId);
    if (!f) {
        std::cout << "  [-] Khong tim thay ma san!\n";
        return;
    }

    std::cout << "\n--- CHI TIET KHUNG GIO SAN " << fieldId << " (" << f->getFieldName() << ") NGAY " << date << " ---\n";
    for (const auto& s : fieldRepo.getSlotsByField(fieldId)) {
        bool booked = bookingRepo.isSlotBookedOnDate(s.getSlotId(), date);
        bool matchUsed = tourRepo.isSlotUsedByMatch(s.getSlotId(), date);
        double price = f->getPricePerSlot() * s.getPriceMultiplier();

        std::string slotState = "CON TRONG";
        if (f->getStatus() != "AVAILABLE") slotState = "SAN DANG BAO TRI";
        else if (booked) slotState = "DA CO KHACH DAT";
        else if (matchUsed) slotState = "CO LICH GIAI DAU";

        std::cout << " * Slot: " << s.getSlotId()
                  << " [" << s.getStartTime() << " - " << s.getEndTime() << "]"
                  << " | Gia: " << price << " VND"
                  << " | Trang thai: " << slotState << "\n";
    }
}

void ConsoleApp::customerBookField() {
    User* u = authService.getCurrentUser();
    if (custService.isCustomerBlacklisted(u->getUserID())) {
        std::cout << "  [-] Tai khoan cua ban dang bi cam (BLACKLIST), khong the dat san!\n";
        return;
    }

    std::string date = readSafeLine("  Nhap ngay muon dat (YYYY-MM-DD, Enter de chon " + DateTime::todayDate() + "): ");
    if (date.empty()) date = DateTime::todayDate();
    if (!DateTime::isValidDate(date)) {
        std::cout << "  [-] Ngay khong hop le (Dinh dang dung: YYYY-MM-DD)!\n";
        return;
    }

    std::cout << "\n--- DANH SACH SAN & SO CA TRONG NGAY " << date << " ---\n";
    for (const auto& f : fieldRepo.getAllFields()) {
        LinkedList<TimeSlot> slots = fieldRepo.getSlotsByField(f.getFieldId());
        int totalSlots = 0, freeSlots = 0;
        std::string bookedList = "";
        for (const auto& s : slots) {
            totalSlots++;
            if (!bookingRepo.isSlotBookedOnDate(s.getSlotId(), date) &&
                !tourRepo.isSlotUsedByMatch(s.getSlotId(), date)) {
                freeSlots++;
            } else {
                if (!bookedList.empty()) bookedList += ", ";
                bookedList += s.getSlotId();
            }
        }
        std::cout << " * Ma san: " << f.getFieldId() << " | " << f.getFieldName()
                  << " (San " << f.getFieldType() << ") | Gia goc: " << f.getPricePerSlot() << " VND"
                  << " | Van hanh: " << (f.getStatus() == "AVAILABLE" ? "MO CUA" : "BAO TRI")
                  << " | Con trong: " << freeSlots << "/" << totalSlots << " ca";
        if (!bookedList.empty()) {
            std::cout << " (Da dat: " << bookedList << ")";
        }
        std::cout << "\n";
    }

    std::string fieldId = readSafeLine("\n  Nhap Ma san muon dat (VD: F01): ");
    Field* f = fieldRepo.findFieldById(fieldId);
    if (!f || f->getStatus() != "AVAILABLE") {
        std::cout << "  [-] San khong ton tai hoac dang bao tri!\n";
        return;
    }

    std::cout << "\n--- CAC KHUNG GIO CUA SAN " << fieldId << " NGAY " << date << " ---\n";
    for (const auto& s : fieldRepo.getSlotsByField(fieldId)) {
        bool booked = bookingRepo.isSlotBookedOnDate(s.getSlotId(), date);
        bool matchUsed = tourRepo.isSlotUsedByMatch(s.getSlotId(), date);
        double price = f->getPricePerSlot() * s.getPriceMultiplier();
        std::string state = booked ? "DA DAT" : (matchUsed ? "LICH GIAI DAU" : "CON TRONG");

        std::cout << "   [" << s.getSlotId() << "] " << s.getStartTime() << " - " << s.getEndTime()
                  << " | Gia: " << price << " VND | -> " << state << "\n";
    }

    std::string rawSlot = readSafeLine("  Nhap Ma khung gio (VD: " + fieldId + "_S1 hoac S1 hoac 1): ");
    if (rawSlot.empty()) return;

    // Chuẩn hóa thông minh: nếu người dùng nhập "1" -> "F01_S1", nếu nhập "S1" hoặc "s1" -> "F01_S1"
    std::string slotId = rawSlot;
    bool allDigits = true;
    for (char c : rawSlot) {
        if (!std::isdigit(static_cast<unsigned char>(c))) { allDigits = false; break; }
    }
    if (allDigits) {
        slotId = fieldId + "_S" + rawSlot;
    } else if (rawSlot.find('_') == std::string::npos && (rawSlot[0] == 'S' || rawSlot[0] == 's')) {
        slotId = fieldId + "_S" + rawSlot.substr(1);
    }

    // Đảm bảo khách không nhập mã khung giờ của sân khác
    if (slotId.substr(0, slotId.find('_')) != fieldId) {
        std::cout << "  [-] Ma khung gio '" << slotId << "' khong thuoc san " << fieldId << "!\n";
        return;
    }

    // Sinh mã đơn Booking tự động không bao giờ trùng
    int nextNum = static_cast<int>(bookingRepo.getAllBookings().size()) + 101;
    std::string bookingId;
    do {
        bookingId = "B" + std::to_string(nextNum++);
    } while (bookingRepo.findBookingById(bookingId) != nullptr);

    if (bookingService.createBooking(bookingId, u->getUserID(), slotId, date)) {
        saveAllData();
    }
}

void ConsoleApp::customerManageMyBookings() {
    User* u = authService.getCurrentUser();
    LinkedList<Booking> myBookings = bookingRepo.getBookingsByCustomer(u->getUserID());
    std::cout << "\n--- LICH SU DAT SAN CUA TOI ---\n";
    if (myBookings.empty()) {
        std::cout << "  (Ban chua co don dat san nao)\n";
        return;
    }

    for (const auto& b : myBookings) {
        std::cout << " * Ma Don: " << b.getBookingId()
                  << " | Ngay dat: " << b.getBookingDate()
                  << " | Trang thai: " << b.getStatus() << "\n";
        for (const auto& d : bookingRepo.getDetailsByBookingId(b.getBookingId())) {
            std::cout << "    -> Khung gio: " << d.getSlotId() << " | Gia: " << d.getPrice() << " VND\n";
        }
    }

    std::cout << "\n 1. Thanh toan don dat san (Ap dung ma giam gia tich luy)\n";
    std::cout << " 2. Huy don dat san (Luu y: Huy >= 3 lan se bi dua vao Blacklist)\n";
    std::cout << " 0. Quay lai\n";
    int sub = readInt("Chon: ");
    if (sub == 1) {
        std::string bid = readSafeLine("  Nhap Ma don muon thanh toan: ");
        Booking* b = bookingRepo.findBookingById(bid);
        if (!b || b->getCustomerId() != u->getUserID()) {
            std::cout << "  [-] Ma don khong ton tai hoac khong thuoc tai khoan cua ban!\n";
            return;
        }
        if (bookingService.checkoutBooking(bid)) {
            saveAllData();
        } else {
            std::cout << "  [-] Don nay da thanh toan hoac da bi huy!\n";
        }
    } else if (sub == 2) {
        std::string bid = readSafeLine("  Nhap Ma don muon huy: ");
        Booking* b = bookingRepo.findBookingById(bid);
        if (!b || b->getCustomerId() != u->getUserID()) {
            std::cout << "  [-] Ma don khong ton tai hoac khong thuoc tai khoan cua ban!\n";
            return;
        }
        std::string reason = readSafeLine("  Nhap ly do huy: ");
        if (bookingService.cancelBooking(bid, reason)) {
            saveAllData();
        }
    }
}

void ConsoleApp::customerTournamentMenu() {
    User* u = authService.getCurrentUser();
    std::cout << "\n--- DANH SACH GIAI DAU HIEN CO ---\n";
    for (const auto& t : tourRepo.getAllTournaments()) {
        LinkedList<Team> tTeams = tourRepo.getTeamsByTournament(t.getTournamentId());
        std::cout << " * [" << t.getTournamentId() << "] " << t.getName()
                  << " | San: " << t.getFieldId()
                  << " | Ngay: " << t.getStartDate()
                  << " | So doi: " << tTeams.size() << "/" << t.getMaxTeams()
                  << " | Trang thai: " << t.getStatus() << "\n";

        TournamentResult res("", "", "", 0);
        if (tourService.calculateTournamentResult(t.getTournamentId(), res)) {
            std::cout << "    => [KET QUA] Vo dich: " << res.getChampionTeamId()
                      << " | A quan: " << res.getRunnerUpTeamId()
                      << " (Da dau: " << res.getTotalMatchesPlayed() << " tran)\n";
        }
    }

    std::cout << "\n 1. Gui yeu cau To chuc Giai dau moi\n";
    std::cout << " 2. Dang ky Doi bong vao Giai dau\n";
    std::cout << " 0. Quay lai\n";
    int sub = readInt("Chon: ");
    if (sub == 1) {
        int nextNum = static_cast<int>(tourRepo.getAllTournaments().size()) + 1;
        std::string tid;
        do {
            tid = "T0" + std::to_string(nextNum++);
        } while (tourRepo.findTournamentById(tid) != nullptr);

        std::string name = readSafeLine("  Nhap ten giai dau: ");
        std::string fid = readSafeLine("  Nhap ma san to chuc (VD: F01): ");
        std::string date = readSafeLine("  Nhap ngay to chuc (YYYY-MM-DD): ");
        int maxTeams = readInt("  So doi toi da (VD: 4): ");

        if (!DateTime::isValidDate(date)) {
            std::cout << "  [-] Ngay to chuc khong hop le!\n";
            return;
        }
        if (tourService.requestTournament(tid, name, u->getUserID(), fid, date, maxTeams)) {
            saveAllData();
            std::cout << "  [+] Ma giai dau cua ban la: " << tid << "\n";
        }
    } else if (sub == 2) {
        std::string tid = readSafeLine("  Nhap Ma giai dau (VD: T01): ");
        std::string tmid = readSafeLine("  Nhap Ma doi bong (VD: TM04): ");
        std::string tmName = readSafeLine("  Nhap Ten doi bong: ");
        std::string phone = readSafeLine("  SDT doi truong: ");
        if (tourService.registerTeam(tmid, tid, tmName, phone)) {
            saveAllData();
        }
    }
}

void ConsoleApp::customerReviewMenu() {
    User* u = authService.getCurrentUser();
    ownerViewFieldsAndSlots();
    std::string fid = readSafeLine("  Nhap Ma san muon danh gia (VD: F01): ");
    if (!fieldRepo.findFieldById(fid)) {
        std::cout << "  [-] Ma san khong ton tai!\n";
        return;
    }
    int rating = readInt("  Cham diem (1 - 5 sao): ");
    std::string comment = readSafeLine("  Nhan xet cua ban: ");
    std::string rvid = "RV" + std::to_string(custRepo.getAllReviews().size() + 101);

    custRepo.addReview(Review(rvid, u->getUserID(), fid, rating, comment, DateTime::todayDate()));
    saveAllData();
    std::cout << "  [+] Cam on ban da gui danh gia!\n";
}