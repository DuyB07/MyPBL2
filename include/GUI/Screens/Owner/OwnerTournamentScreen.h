#pragma once
#include <string>
#include "GUI/Core/IScreen.h"
#include "GUI/Core/AppContext.h"

class OwnerTournamentScreen : public IScreen {
private:
    AppContext& ctx;
    bool openMatchesModal;
    std::string selectedTourId;
    std::string selectedMatchId;
    int scoreA;
    int scoreB;

    void renderMatchesAndScoreModal();

public:
    explicit OwnerTournamentScreen(AppContext& context);
    const char* getMenuLabel() const override { return "5. Quản lý Giải đấu"; }
    void render() override;
};