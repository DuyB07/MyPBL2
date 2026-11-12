#pragma once
#include <string>
#include "Core/LinkedList.h"
#include "Account/User.h"
#include "Account/Owner.h"
#include "Repository/CustomerRepository.h"
#include "Storage/FileManager.h"

class AccountService {
private:
    LinkedList<Owner> owners;
    CustomerRepository& customerRepo;
    User* currentUser; // Con trỏ đa hình trỏ tới Owner hoặc Customer đang đăng nhập

public:
    AccountService(CustomerRepository& cRepo) : customerRepo(cRepo), currentUser(nullptr) {}

    bool loadOwners(const std::string& path = "data/accounts.txt");
    bool saveOwners(const std::string& path = "data/accounts.txt") const;

    // Khởi tạo tài khoản Chủ sân mặc định nếu file accounts.txt chưa có
    void ensureDefaultOwner();

    // Đăng nhập trả về con trỏ lớp cơ sở User* (Upcasting)
    User* login(const std::string& username, const std::string& password);
    void logout();

    User* getCurrentUser() const { return currentUser; }
    Owner* getPrimaryOwner();
};