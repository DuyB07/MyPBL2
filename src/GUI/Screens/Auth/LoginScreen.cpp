#include "GUI/Screens/Auth/LoginScreen.h"
#include "GUI/Theme/OrangeLightTheme.h"
#include "GUI/Theme/FontManager.h"
#include "GUI/Components/UIComponents.h"
#include <GLFW/glfw3.h>
#include <cstring>
#include <cstdio>

LoginScreen::LoginScreen(AppContext& context)
    : ctx(context), isRegisterMode(false) {
    std::memset(loginUserBuf, 0, sizeof(loginUserBuf));
    std::memset(loginPassBuf, 0, sizeof(loginPassBuf));
    std::memset(regUserBuf, 0, sizeof(regUserBuf));
    std::memset(regPassBuf, 0, sizeof(regPassBuf));
    std::memset(regNameBuf, 0, sizeof(regNameBuf));
    std::memset(regPhoneBuf, 0, sizeof(regPhoneBuf));
}

void LoginScreen::render() {
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float cardW = 470.0f;
    float cardH = isRegisterMode ? 525.0f : 500.0f;

    float posX = (avail.x - cardW) * 0.5f;
    float posY = (avail.y - cardH) * 0.5f;
    if (posX > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + posX);
    if (posY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + posY);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(32.0f, 26.0f));
    ImGui::BeginChild("LoginCenteredCard", ImVec2(cardW, cardH), true, ImGuiWindowFlags_NoScrollbar);

    FontManager::pushHeading();
    ImGui::TextColored(Theme::PrimaryOrange, isRegisterMode ? "ĐĂNG KÝ TÀI KHOẢN" : "ĐĂNG NHẬP HỆ THỐNG");
    FontManager::popHeading();

    if (Owner* own = ctx.authService.getPrimaryOwner()) {
        ImGui::TextColored(Theme::TextMuted, "%s", own->getStadiumName().c_str());
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (!isRegisterMode) {
        ImGui::Text("Tên đăng nhập");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##login_user", loginUserBuf, sizeof(loginUserBuf));

        ImGui::Spacing();
        ImGui::Text("Mật khẩu");
        ImGui::SetNextItemWidth(-1);
        bool enterPressed = ImGui::InputText("##login_pass", loginPassBuf, sizeof(loginPassBuf),
                                             ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::Spacing();
        ImGui::Spacing();

        if (UI::PrimaryButton("ĐĂNG NHẬP", ImVec2(-1, 42.0f)) || enterPressed) {
            std::string u = AppContext::sanitize(loginUserBuf);
            std::string p = AppContext::sanitize(loginPassBuf);
            User* logged = ctx.authService.login(u, p);
            if (logged) {
                ctx.notify("Đăng nhập thành công! Xin chào " + logged->getFullName() + ".", false, glfwGetTime());
            } else {
                ctx.notify("Sai tên đăng nhập hoặc mật khẩu!", true, glfwGetTime());
            }
        }

        ImGui::Spacing();
        if (UI::SecondaryButton("Chưa có tài khoản? Đăng ký Khách hàng mới", ImVec2(-1, 36.0f))) {
            isRegisterMode = true;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextColored(Theme::TextMuted, "Điền nhanh tài khoản để kiểm thử:");

        float spacing = ImGui::GetStyle().ItemSpacing.x;
        float halfW = (ImGui::GetContentRegionAvail().x - spacing) * 0.5f;
        if (UI::SecondaryButton("TK Chủ Sân (admin)", ImVec2(halfW, 34.0f))) {
            std::snprintf(loginUserBuf, sizeof(loginUserBuf), "admin");
            std::snprintf(loginPassBuf, sizeof(loginPassBuf), "admin123");
        }
        ImGui::SameLine();
        if (UI::SecondaryButton("TK Khách (khachA)", ImVec2(halfW, 34.0f))) {
            if (!ctx.custRepo.getAllCustomers().empty()) {
                const Customer& c = *ctx.custRepo.getAllCustomers().begin();
                std::snprintf(loginUserBuf, sizeof(loginUserBuf), "%s", c.getUsername().c_str());
                std::snprintf(loginPassBuf, sizeof(loginPassBuf), "%s", c.getPassword().c_str());
            } else {
                std::snprintf(loginUserBuf, sizeof(loginUserBuf), "khachA");
                std::snprintf(loginPassBuf, sizeof(loginPassBuf), "123");
            }
        }
    } else {
        ImGui::Text("Tên đăng nhập mới");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##reg_user", regUserBuf, sizeof(regUserBuf));

        ImGui::Text("Mật khẩu");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##reg_pass", regPassBuf, sizeof(regPassBuf), ImGuiInputTextFlags_Password);

        ImGui::Text("Họ và tên khách hàng");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##reg_name", regNameBuf, sizeof(regNameBuf));

        ImGui::Text("Số điện thoại (9 - 11 chữ số)");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##reg_phone", regPhoneBuf, sizeof(regPhoneBuf));

        ImGui::Spacing();
        if (UI::PrimaryButton("HOÀN TẤT ĐĂNG KÝ", ImVec2(-1, 42.0f))) {
            int nextNum = static_cast<int>(ctx.custRepo.getAllCustomers().size()) + 1;
            std::string newId;
            do {
                char buf[16];
                std::snprintf(buf, sizeof(buf), "C%02d", nextNum++);
                newId = buf;
            } while (ctx.custRepo.findById(newId) != nullptr);

            std::string u  = AppContext::sanitize(regUserBuf);
            std::string p  = AppContext::sanitize(regPassBuf);
            std::string n  = AppContext::sanitize(regNameBuf);
            std::string ph = AppContext::sanitize(regPhoneBuf);

            if (ctx.custService.registerNewCustomer(newId, u, p, n, ph)) {
                ctx.saveAllData();
                std::snprintf(loginUserBuf, sizeof(loginUserBuf), "%s", u.c_str());
                std::snprintf(loginPassBuf, sizeof(loginPassBuf), "%s", p.c_str());
                isRegisterMode = false;
                ctx.notify("Đăng ký thành công! Mã KH: " + newId + ". Hãy bấm Đăng nhập.", false, glfwGetTime());
            } else {
                ctx.notify("Đăng ký thất bại! Kiểm tra SĐT (9-11 số) hoặc Username đã tồn tại.", true, glfwGetTime());
            }
        }

        if (UI::SecondaryButton("Quay lại màn hình Đăng nhập", ImVec2(-1, 36.0f))) {
            isRegisterMode = false;
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}