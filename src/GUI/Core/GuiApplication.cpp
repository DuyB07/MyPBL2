#include "GUI/Core/GuiApplication.h"
#include "GUI/Theme/OrangeLightTheme.h"
#include "GUI/Theme/FontManager.h"
#include "GUI/Components/UIComponents.h"

#include "GUI/Screens/Owner/OwnerDashboardScreen.h"
#include "GUI/Screens/Owner/OwnerFieldScreen.h"
#include "GUI/Screens/Owner/OwnerBookingScreen.h"
#include "GUI/Screens/Owner/OwnerCustomerScreen.h"
#include "GUI/Screens/Owner/OwnerTournamentScreen.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdio>

// Lớp màn hình khung mẫu tạm thời cho phần Customer (sẽ thay ở Bước GUI-3)
class OverviewPreviewScreen : public IScreen {
private:
    AppContext& ctx;
    const char* label;
public:
    OverviewPreviewScreen(AppContext& c, const char* lbl) : ctx(c), label(lbl) {}
    const char* getMenuLabel() const override { return label; }
    void render() override {
        UI::PageHeader(label, "Bo cuc Trang - Xam Sang - Cam lay cam hung tu mffms-web");

        float availW = ImGui::GetContentRegionAvail().x;
        float cardW = (availW - 36.0f) * 0.25f;

        char revBuf[64];
        std::snprintf(revBuf, sizeof(revBuf), "%.0f VND", ctx.bookingRepo.calculateTotalRevenue());

        UI::MetricCard("kpi1", "TONG DOANH THU", revBuf, "Tu cac hoa don da thanh toan", true, cardW);
        ImGui::SameLine();
        UI::MetricCard("kpi2", "SO SAN BONG", std::to_string(ctx.fieldRepo.getAllFields().size()) + " san", "San 5 / 7 / 11 nguoi", false, cardW);
        ImGui::SameLine();
        UI::MetricCard("kpi3", "PHIEU DAT SAN", std::to_string(ctx.bookingRepo.getAllBookings().size()) + " phieu", "Dong bo data/bookings.txt", false, cardW);
        ImGui::SameLine();
        UI::MetricCard("kpi4", "KHACH HANG", std::to_string(ctx.custRepo.getAllCustomers().size()) + " tai khoan", "Tra cuu O(1) qua HashTable", false, cardW);

        ImGui::Spacing();
        UI::BeginSectionCard("PreviewSection", "THONG TIN CUM SAN HIEN TAI");
        if (Owner* own = ctx.authService.getPrimaryOwner()) {
            ImGui::Text("Ten cum san : %s", own->getStadiumName().c_str());
            ImGui::Text("Dia chi     : %s", own->getAddress().c_str());
            ImGui::Text("Gio mo cua  : %s - %s", own->getOpenTime().c_str(), own->getCloseTime().c_str());
            ImGui::Text("Noi quy     : %s", own->getFieldRules().c_str());
        }
        UI::EndSectionCard();
    }
};

GuiApplication::GuiApplication()
    : window(nullptr),
      loginScreen(ctx),
      ownerScreenCount(0),
      customerScreenCount(0),
      activeOwnerIndex(0),
      activeCustomerIndex(0) {
    // Khởi tạo 5 màn hình chính thức của Chủ sân (Owner)
    ownerScreens[ownerScreenCount++] = new OwnerDashboardScreen(ctx);
    ownerScreens[ownerScreenCount++] = new OwnerFieldScreen(ctx);
    ownerScreens[ownerScreenCount++] = new OwnerBookingScreen(ctx);
    ownerScreens[ownerScreenCount++] = new OwnerCustomerScreen(ctx);
    ownerScreens[ownerScreenCount++] = new OwnerTournamentScreen(ctx);

    // Khởi tạo tạm 4 mục điều hướng của Khách hàng (Customer)
    customerScreens[customerScreenCount++] = new OverviewPreviewScreen(ctx, "1. Dat san truc quan");
    customerScreens[customerScreenCount++] = new OverviewPreviewScreen(ctx, "2. Phieu dat cua toi");
    customerScreens[customerScreenCount++] = new OverviewPreviewScreen(ctx, "3. Giai dau & Doi bong");
    customerScreens[customerScreenCount++] = new OverviewPreviewScreen(ctx, "4. Uu dai & Danh gia");
}

GuiApplication::~GuiApplication() {
    for (int i = 0; i < ownerScreenCount; ++i) delete ownerScreens[i];
    for (int i = 0; i < customerScreenCount; ++i) delete customerScreens[i];
    shutdown();
}

bool GuiApplication::init() {
    if (!glfwInit()) return false;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1320, 800, "He Thong Quan Ly San Bong Mini - PBL2", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    FontManager::loadFonts();
    Theme::applyOrangeLightTheme();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    return true;
}

void GuiApplication::run() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(vp->WorkSize);

        ImGuiWindowFlags rootFlags = ImGuiWindowFlags_NoTitleBar |
                                     ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_NoCollapse |
                                     ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::Begin("MasterAppWindow", nullptr, rootFlags);
        ImGui::PopStyleVar(2);

        renderTopHeaderBar();

        User* currentUser = ctx.authService.getCurrentUser();
        if (!currentUser) {
            ImGui::BeginChild("AuthWorkspace", ImVec2(0, 0), false);
            loginScreen.render();
            ImGui::EndChild();
        } else if (currentUser->getRole() == "OWNER") {
            renderSidebarAndWorkspace(ownerScreens, ownerScreenCount, activeOwnerIndex);
        } else {
            renderSidebarAndWorkspace(customerScreens, customerScreenCount, activeCustomerIndex);
        }

        renderToastOverlay();
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(Theme::BgWorkspace.x, Theme::BgWorkspace.y, Theme::BgWorkspace.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

void GuiApplication::renderTopHeaderBar() {
    constexpr float headerH = 56.0f;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::PrimaryOrange);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24.0f, 0.0f));
    ImGui::BeginChild("TopHeaderBar", ImVec2(0.0f, headerH), false, ImGuiWindowFlags_NoScrollbar);

    // 1. Căn giữa theo chiều dọc cho Tiêu đề Cụm sân bên trái
    FontManager::pushMedium();
    float titleH = ImGui::GetTextLineHeight();
    ImGui::SetCursorPosY((headerH - titleH) * 0.5f);
    if (Owner* own = ctx.authService.getPrimaryOwner()) {
        ImGui::TextColored(Theme::TextOnOrange, "MFFMS  |  %s", own->getStadiumName().c_str());
    } else {
        ImGui::TextColored(Theme::TextOnOrange, "MFFMS  |  HỆ THỐNG QUẢN LÝ SÂN BÓNG MINI");
    }
    FontManager::popMedium();

    // 2. Căn giữa theo chiều dọc cho Tên người dùng & Nút Đăng xuất bên phải
    User* u = ctx.authService.getCurrentUser();
    if (u) {
        std::string roleVN = (u->getRole() == "OWNER") ? "CHỦ SÂN" : "KHÁCH THUÊ";
        std::string userBadge = "[" + roleVN + "]  " + u->getFullName() + " (" + u->getUserID() + ")";
        float textW = ImGui::CalcTextSize(userBadge.c_str()).x;
        float btnW  = 104.0f;
        float btnH  = 32.0f;

        float rightStartX = ImGui::GetWindowWidth() - textW - btnW - 44.0f;
        if (rightStartX > 360.0f) {
            ImGui::SameLine(rightStartX);
            ImGui::SetCursorPosY((headerH - ImGui::GetTextLineHeight()) * 0.5f);
            ImGui::TextColored(Theme::TextOnOrange, "%s", userBadge.c_str());

            ImGui::SameLine(0.0f, 16.0f);
            ImGui::SetCursorPosY((headerH - btnH) * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Button, Theme::BgCard);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::SoftOrangeTint);
            ImGui::PushStyleColor(ImGuiCol_Text, Theme::PrimaryOrange);
            if (ImGui::Button("Đăng xuất", ImVec2(btnW, btnH))) {
                ctx.saveAllData();
                ctx.authService.logout();
                ctx.notify("Đã lưu dữ liệu và đăng xuất thành công.", false, glfwGetTime());
            }
            ImGui::PopStyleColor(3);
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor();
}

void GuiApplication::renderSidebarAndWorkspace(IScreen** screens, int count, int& activeIdx) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgSidebar);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 20.0f));
    ImGui::BeginChild("LeftSidebar", ImVec2(255.0f, 0.0f), true);

    ImGui::TextColored(Theme::TextMuted, "DANH MỤC QUẢN LÝ");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.08f, 0.5f));
    for (int i = 0; i < count; ++i) {
        bool isSelected = (activeIdx == i);
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Button, Theme::PrimaryOrange);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::OrangeHover);
            ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextOnOrange);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, Theme::BgCard);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::SoftOrangeTint);
            ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextPrimary);
        }

        if (ImGui::Button(screens[i]->getMenuLabel(), ImVec2(-1.0f, 42.0f))) {
            activeIdx = i;
        }
        ImGui::PopStyleColor(3);
        ImGui::Spacing();
    }
    ImGui::PopStyleVar();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    ImGui::SameLine(0.0f, 0.0f);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgWorkspace);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24.0f, 20.0f));
    ImGui::BeginChild("MainContentWorkspace", ImVec2(0.0f, 0.0f), false);

    if (activeIdx >= 0 && activeIdx < count && screens[activeIdx]) {
        screens[activeIdx]->render();
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void GuiApplication::renderToastOverlay() {
    double now = glfwGetTime();
    if (ctx.toastMessage.empty() || now > ctx.toastExpireTime) return;

    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImVec2 toastSize(420.0f, 54.0f);
    ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + vp->WorkSize.x - toastSize.x - 24.0f,
                                   vp->WorkPos.y + vp->WorkSize.y - toastSize.y - 24.0f));
    ImGui::SetNextWindowSize(toastSize);

    ImVec4 bg = ctx.toastIsError ? Theme::DangerRed : Theme::SuccessGreen;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);
    ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextOnOrange);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoNav |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoFocusOnAppearing;

    if (ImGui::Begin("##ToastPopup", nullptr, flags)) {
        ImGui::TextWrapped("%s  %s", ctx.toastIsError ? "[LOI]" : "[THONG BAO]", ctx.toastMessage.c_str());
    }
    ImGui::End();
    ImGui::PopStyleColor(2);
}

void GuiApplication::shutdown() {
    if (window) {
        ctx.saveAllData();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}