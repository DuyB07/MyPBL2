#pragma once
#include "GUI/Core/AppContext.h"
#include "GUI/Core/IScreen.h"
#include "GUI/Screens/Auth/LoginScreen.h"

struct GLFWwindow;

class GuiApplication {
private:
    GLFWwindow* window;
    AppContext ctx;
    LoginScreen loginScreen;

    // Danh sách màn hình đa hình của Chủ sân & Khách thuê
    static constexpr int MAX_OWNER_SCREENS = 5;
    static constexpr int MAX_CUSTOMER_SCREENS = 4;
    IScreen* ownerScreens[MAX_OWNER_SCREENS];
    IScreen* customerScreens[MAX_CUSTOMER_SCREENS];
    int ownerScreenCount;
    int customerScreenCount;

    int activeOwnerIndex;
    int activeCustomerIndex;

    void renderTopHeaderBar();
    void renderSidebarAndWorkspace(IScreen** screens, int count, int& activeIdx);
    void renderToastOverlay();

public:
    GuiApplication();
    ~GuiApplication();

    bool init();
    void run();
    void shutdown();
};