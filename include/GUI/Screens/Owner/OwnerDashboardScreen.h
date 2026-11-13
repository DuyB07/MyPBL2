#pragma once
#include "GUI/Core/IScreen.h"
#include "GUI/Core/AppContext.h"

class OwnerDashboardScreen : public IScreen {
private:
    AppContext& ctx;
    bool openEditStadiumModal;

    char stadiumNameBuf[128];
    char addressBuf[128];
    char rulesBuf[256];

    void renderEditStadiumPopup();

public:
    explicit OwnerDashboardScreen(AppContext& context);
    const char* getMenuLabel() const override { return "1. Tong quan he thong"; }
    void render() override;
};