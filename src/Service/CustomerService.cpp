#include "Service/CustomerService.h"
#include <iostream>

bool CustomerService::registerNewCustomer(const std::string& id, const std::string& uname, const std::string& pwd, 
                                          const std::string& name, const std::string& phone) {
    if (uname.empty() || pwd.empty() || name.empty()) {
        std::cerr << "  [-] Thong tin tai khoan khong duoc de trong!\n";
        return false;
    }
    if (phone.length() < 9 || phone.length() > 11) {
        std::cerr << "  [-] So dien thoai phai tu 9 den 11 chu so!\n";
        return false;
    }
    if (customerRepo.findByPhone(phone) != nullptr) {
        std::cerr << "  [-] So dien thoai da ton tai!\n";
        return false;
    }
    if (customerRepo.findByUsername(uname) != nullptr) {
        std::cerr << "  [-] Ten dang nhap da ton tai!\n";
        return false;
    }
    
    Customer newCust(id, uname, pwd, name, phone);
    return customerRepo.addCustomer(newCust);
}

Customer* CustomerService::getCustomerByPhone(const std::string& phone) {
    return customerRepo.findByPhone(phone);
}

Customer* CustomerService::getCustomerById(const std::string& id) {
    return customerRepo.findById(id);
}

bool CustomerService::recordSuccessfulBooking(const std::string& customerId) {
    Customer* cust = customerRepo.findById(customerId);
    if (!cust) return false;

    cust->addBooking();
    return true;
}

bool CustomerService::recordCanceledBooking(const std::string& customerId, const std::string& reason, const std::string& date) {
    Customer* cust = customerRepo.findById(customerId);
    if (!cust) return false;

    bool wasBlacklisted = cust->isBlacklisted();
    cust->decrementBooking(); // Hoàn lại bộ đếm lượt đặt để tránh trục lợi discount
    cust->addCancel();        // Tăng bộ đếm hủy

    // Nếu hủy >= 3 lần và trước đó chưa bị ban -> Đưa vào Blacklist & lưu bản ghi
    if (cust->getCancelCount() >= 3 && !wasBlacklisted) {
        cust->setBlacklist(true);
        std::string blId = "BL_" + customerId + "_" + std::to_string(cust->getCancelCount());
        Blacklist bl(blId, customerId, cust->getPhone(), cust->getCancelCount(), reason, date, true);
        customerRepo.addBlacklist(bl);
        std::cout << "  [SYSTEM] Khach hang " << cust->getFullName()
                  << " da huy " << cust->getCancelCount() << " lan -> Tu dong dua vao BLACKLIST!\n";
    }
    
    return true;
}

bool CustomerService::isCustomerBlacklisted(const std::string& customerId) {
    Customer* cust = customerRepo.findById(customerId);
    if (cust) {
        return cust->isBlacklisted();
    }
    return false;
}

bool CustomerService::unbanCustomer(const std::string& customerId) {
    Customer* cust = customerRepo.findById(customerId);
    if (!cust || !cust->isBlacklisted()) return false;

    cust->setBlacklist(false);
    cust->resetCancelCount(); // Reset số lần hủy để khách có cơ hội đặt lại
    customerRepo.deactivateBlacklist(customerId);
    std::cout << "  [SYSTEM] Da go Blacklist va reset bo dem vi pham cho " << cust->getFullName() << ".\n";
    return true;
}

void CustomerService::updateAllDiscounts() {
    std::cout << "  [SYSTEM] Da cap nhat giam gia cho khach hang.\n";
}