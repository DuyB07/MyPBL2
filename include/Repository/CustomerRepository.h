#pragma once
#include <string>
#include "Core/LinkedList.h"
#include "Core/HashTable.h"
#include "Account/Customer.h"
#include "Customer/Blacklist.h"
#include "Customer/Review.h"
#include "Storage/FileManager.h"

class CustomerRepository {
private:
    LinkedList<Customer> customers;
    HashTable<std::string, std::string> phoneToIdMap;    // Key: Phone, Value: CustomerID
    HashTable<std::string, std::string> usernameToIdMap; // Key: Username, Value: CustomerID
    LinkedList<Blacklist> blacklists;
    LinkedList<Review> reviews;

public:
    CustomerRepository() = default;
    
    // --- Quản lý Customer ---
    bool loadCustomers(const std::string& path = "data/customers.txt");
    bool saveCustomers(const std::string& path = "data/customers.txt") const;
    bool addCustomer(const Customer& customer);
    Customer* findById(const std::string& id);
    Customer* findByPhone(const std::string& phone);
    Customer* findByUsername(const std::string& username);
    const LinkedList<Customer>& getAllCustomers() const { return customers; }

    // --- Quản lý Blacklist ---
    bool loadBlacklists(const std::string& path = "data/blacklist.txt");
    bool saveBlacklists(const std::string& path = "data/blacklist.txt") const;
    void addBlacklist(const Blacklist& bl);
    void deactivateBlacklist(const std::string& customerId);
    const LinkedList<Blacklist>& getAllBlacklists() const { return blacklists; }

    // --- Quản lý Review ---
    bool loadReviews(const std::string& path = "data/reviews.txt");
    bool saveReviews(const std::string& path = "data/reviews.txt") const;
    void addReview(const Review& rv);
    const LinkedList<Review>& getAllReviews() const { return reviews; }
};