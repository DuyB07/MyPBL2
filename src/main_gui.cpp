#include "GUI/Core/GuiApplication.h"
#include <filesystem>

int main() {
    if (!std::filesystem::exists("data") && std::filesystem::exists("../data")) {
        std::filesystem::current_path("..");
    }

    GuiApplication app;
    if (!app.init()) {
        return 1;
    }
    app.run();
    return 0;
}