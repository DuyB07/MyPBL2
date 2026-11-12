#pragma once
#include <string>
#include "Repository/CustomerRepository.h"

class CustomerService {
private:
    CustomerRepository& customerRepo;

public:
    // Khởi tạo Service với Repository tương ứng
    CustomerService(CustomerRepository& repo) : customerRepo(repo) {}

    // --- Nghiệp vụ Khách hàng & Tài khoản ---
    bool registerNewCustomer(const std::string& id, const std::string& uname, const std::string& pwd, 
                             const std::string& name, const std::string& phone);
                             
    Customer* getCustomerByPhone(const std::string& phone);
    
    Customer* getCustomerById(const std::string& id);
    
    // --- Nghiệp vụ Đặt/Hủy sân (Tương tác với Booking) ---
    bool recordSuccessfulBooking(const std::string& customerId);
    
    // Ghi nhận hủy sân, nếu hủy 3 lần tự động đưa vào Blacklist
    bool recordCanceledBooking(const std::string& customerId, const std::string& reason, const std::string& date);

    // --- Nghiệp vụ Blacklist & Discount ---
    bool isCustomerBlacklisted(const std::string& customerId);
    bool unbanCustomer(const std::string& customerId);

    // Cập nhật mức giảm giá cho toàn bộ khách hàng dựa trên số lần đặt
    void updateAllDiscounts();
};