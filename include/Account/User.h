#pragma once
#include <string>

class User {
protected:
    std::string userID;
    std::string username;
    std::string password;
    std::string fullName; // Thêm fullName cho rõ ràng
    std::string phone;
    std::string role; // "OWNER" hoặc "CUSTOMER"

public:
    User(const std::string& id, const std::string& uname, const std::string& pwd, 
         const std::string& name, const std::string& ph, const std::string& r)
        : userID(id), username(uname), password(pwd), fullName(name), phone(ph), role(r) {}

    virtual ~User() = default; // Destructor ảo để hủy đối tượng con an toàn

    // --- Getters ---
    std::string getUserID() const { return userID; }
    std::string getUsername() const { return username; }
    std::string getPassword() const { return password; }
    std::string getFullName() const { return fullName; }
    std::string getPhone() const { return phone; }
    std::string getRole() const { return role; }

    // --- Setters (Kèm kiểm tra dữ liệu hợp lệ) ---
    void setFullName(const std::string& name) { fullName = name; }
    void setPassword(const std::string& pwd) { password = pwd; }
    bool setPhone(const std::string& newPhone) {
        if (newPhone.length() >= 9 && newPhone.length() <= 11) {
            phone = newPhone;
            return true;
        }
        return false;
    }

    // --- Phương thức ảo thuần túy ---
    virtual void displayInfo() const = 0; 
};