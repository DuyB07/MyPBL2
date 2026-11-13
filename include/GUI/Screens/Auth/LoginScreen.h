#pragma once
#include "GUI/Core/IScreen.h"
#include "GUI/Core/AppContext.h"

class LoginScreen : public IScreen {
private:
    AppContext& ctx;
    bool isRegisterMode;

    char loginUserBuf[64];
    char loginPassBuf[64];

    char regUserBuf[64];
    char regPassBuf[64];
    char regNameBuf[128];
    char regPhoneBuf[32];

public:
    explicit LoginScreen(AppContext& context);
    const char* getMenuLabel() const override { return "Dang nhap"; }
    void render() override;
};