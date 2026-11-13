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
    float cardW = 460.0f;
    float cardH = isRegisterMode ? 490.0f : 430.0f;

    // Căn khung thẻ trắng vào chính giữa màn hình
    float posX = (avail.x - cardW) * 0.5f;
    float posY = (avail.y - cardH) * 0.5f;
    if (posX > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + posX);
    if (posY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + posY);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(32.0f, 28.0f));
    ImGui::BeginChild("LoginCenteredCard", ImVec2(cardW, cardH), true, ImGuiWindowFlags_NoScrollbar);

    // Tiêu đề màu Cam nổi bật
    FontManager::pushHeading();
    ImGui::TextColored(Theme::PrimaryOrange, isRegisterMode ? "DANG KY TAI KHOAN" : "DANG NHAP HE THONG");
    FontManager::popHeading();

    if (Owner* own = ctx.authService.getPrimaryOwner()) {
        ImGui::TextColored(Theme::TextMuted, "%s", own->getStadiumName().c_str());
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (!isRegisterMode) {
        ImGui::Text("Ten dang nhap");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##login_user", loginUserBuf, sizeof(loginUserBuf));

        ImGui::Spacing();
        ImGui::Text("Mat khau");
        ImGui::SetNextItemWidth(-1);
        bool enterPressed = ImGui::InputText("##login_pass", loginPassBuf, sizeof(loginPassBuf),
                                             ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::Spacing();
        ImGui::Spacing();

        if (UI::PrimaryButton("DANG NHAP", ImVec2(-1, 42.0f)) || enterPressed) {
            std::string u = AppContext::sanitize(loginUserBuf);
            std::string p = AppContext::sanitize(loginPassBuf);
            User* logged = ctx.authService.login(u, p);
            if (logged) {
                ctx.notify("Xin chao " + logged->getFullName() + "!", false, glfwGetTime());
            } else {
                ctx.notify("Sai ten dang nhap hoac mat khau!", true, glfwGetTime());
            }
        }

        ImGui::Spacing();
        if (UI::SecondaryButton("Chua co tai khoan? Dang ky Khach hang moi", ImVec2(-1, 36.0f))) {
            isRegisterMode = true;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(Theme::TextMuted, "Dang nhap nhanh de kiem thu:");
        float halfW = (ImGui::GetContentRegionAvail().x - 10.0f) * 0.5f;
        if (UI::SecondaryButton("TK Chu San (admin)", ImVec2(halfW, 32.0f))) {
            std::snprintf(loginUserBuf, sizeof(loginUserBuf), "admin");
            std::snprintf(loginPassBuf, sizeof(loginPassBuf), "admin123");
        }
        ImGui::SameLine();
        if (UI::SecondaryButton("TK Khach (khachA)", ImVec2(halfW, 32.0f))) {
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
        ImGui::Text("Ten dang nhap moi");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##reg_user", regUserBuf, sizeof(regUserBuf));

        ImGui::Text("Mat khau");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##reg_pass", regPassBuf, sizeof(regPassBuf), ImGuiInputTextFlags_Password);

        ImGui::Text("Ho va ten khach hang");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##reg_name", regNameBuf, sizeof(regNameBuf));

        ImGui::Text("So dien thoai (9 - 11 chu so)");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##reg_phone", regPhoneBuf, sizeof(regPhoneBuf));

        ImGui::Spacing();
        if (UI::PrimaryButton("HOAN TAT DANG KY", ImVec2(-1, 42.0f))) {
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
                ctx.notify("Dang ky thanh cong! Ma KH: " + newId + ". Hay bam Dang nhap.", false, glfwGetTime());
            } else {
                ctx.notify("Dang ky that bai! Kiem tra SDT (9-11 so) hoac Username da ton tai.", true, glfwGetTime());
            }
        }

        if (UI::SecondaryButton("Quay lai man hinh Dang nhap", ImVec2(-1, 36.0f))) {
            isRegisterMode = false;
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}