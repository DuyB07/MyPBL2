#pragma once
#include <string>

class Invoice {
private:
    std::string invoiceId;
    std::string bookingId;
    double totalAmount;
    double discountAmount;
    double finalAmount;
    std::string paymentDate;

public:
    Invoice(const std::string& iId, const std::string& bId, double total, double discount, double finalAmt, const std::string& pDate)
        : invoiceId(iId), bookingId(bId), totalAmount(total), discountAmount(discount), finalAmount(finalAmt), paymentDate(pDate) {}

    std::string getInvoiceId() const { return invoiceId; }
    std::string getBookingId() const { return bookingId; }
    double getTotalAmount() const { return totalAmount; }
    double getDiscountAmount() const { return discountAmount; }
    double getFinalAmount() const { return finalAmount; }

    std::string serialize() const {
        return invoiceId + "|" + bookingId + "|" + std::to_string(totalAmount) + "|" + 
               std::to_string(discountAmount) + "|" + std::to_string(finalAmount) + "|" + paymentDate;
    }
};