#pragma once
#include <string>
#include "Core/Queue.h"
#include "Repository/TournamentRepository.h"
#include "Repository/FieldRepository.h"
#include "Repository/BookingRepository.h"
#include "Service/CustomerService.h"
#include "Tournament/TournamentResult.h"

class TournamentService {
private:
    TournamentRepository& tourRepo;
    FieldRepository& fieldRepo;
    BookingRepository& bookingRepo;
    CustomerService& customerService;

public:
    TournamentService(TournamentRepository& tRepo, FieldRepository& fRepo,
                      BookingRepository& bRepo, CustomerService& cService)
        : tourRepo(tRepo), fieldRepo(fRepo), bookingRepo(bRepo), customerService(cService) {}

    // Khách hàng gửi yêu cầu tạo giải đấu
    bool requestTournament(const std::string& tourId, const std::string& name,
                           const std::string& customerId, const std::string& fieldId,
                           const std::string& startDate, int maxTeams);

    // Đăng ký đội bóng vào giải
    bool registerTeam(const std::string& teamId, const std::string& tourId,
                      const std::string& teamName, const std::string& phone);

    // Chủ sân duyệt giải và tự động xếp lịch thi đấu bằng Queue<TournamentMatch> (FIFO)
    bool approveAndScheduleTournament(const std::string& tourId);

    // Chủ sân từ chối yêu cầu tạo giải đấu
    bool rejectTournament(const std::string& tourId);

    // Cập nhật tỉ số trận đấu (tự động chuyển giải sang COMPLETED nếu đã đấu hết các trận)
    bool recordMatchScore(const std::string& matchId, int scoreA, int scoreB);

    // Tính toán kết quả Vô địch & Á quân bằng lớp TournamentResult
    bool calculateTournamentResult(const std::string& tourId, TournamentResult& outResult) const;
};