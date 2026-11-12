#pragma once
#include <string>

class Review {
private:
    std::string reviewId;
    std::string customerId;
    std::string fieldId;
    int rating; // 1-5
    std::string comment;
    std::string createdDate;

public:
    Review(const std::string& rId, const std::string& cId, const std::string& fId,
           int rtg, const std::string& cmt, const std::string& date)
        : reviewId(rId), customerId(cId), fieldId(fId), rating(rtg), comment(cmt), createdDate(date) {
        if (rating < 1) rating = 1;
        if (rating > 5) rating = 5;
    }

    std::string getReviewId() const { return reviewId; }
    std::string getCustomerId() const { return customerId; }
    std::string getFieldId() const { return fieldId; }
    int getRating() const { return rating; }
    std::string getComment() const { return comment; }
    std::string getCreatedDate() const { return createdDate; }

    std::string serialize() const {
        return reviewId + "|" + customerId + "|" + fieldId + "|" +
               std::to_string(rating) + "|" + comment + "|" + createdDate;
    }
};