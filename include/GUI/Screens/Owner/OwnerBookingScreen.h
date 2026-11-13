#pragma once
#include <string>
#include "GUI/Core/IScreen.h"
#include "GUI/Core/AppContext.h"

class OwnerBookingScreen : public IScreen {
private:
    AppContext& ctx;
    int statusFilterIdx; // 0: Tất cả, 1: CONFIRMED, 2: COMPLETED, 3: CANCELED
    char searchIdBuf[32];

public:
    explicit OwnerBookingScreen(AppContext& context);
    const char* getMenuLabel() const override { return "3. Phieu dat san & Hoa don"; }
    void render() override;
};