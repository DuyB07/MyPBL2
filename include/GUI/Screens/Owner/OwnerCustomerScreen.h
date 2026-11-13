#pragma once
#include <string>
#include "GUI/Core/IScreen.h"
#include "GUI/Core/AppContext.h"

class OwnerCustomerScreen : public IScreen {
private:
    AppContext& ctx;
    char phoneSearchBuf[32];
    std::string foundCustomerBanner;
    bool foundSuccess;
    bool openBlacklistModal;

    void renderBlacklistHistoryModal();

public:
    explicit OwnerCustomerScreen(AppContext& context);
    const char* getMenuLabel() const override { return "4. Danh bạ Khách hàng"; }
    void render() override;
};