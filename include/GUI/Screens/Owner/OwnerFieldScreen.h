#pragma once
#include <string>
#include "GUI/Core/IScreen.h"
#include "GUI/Core/AppContext.h"

class OwnerFieldScreen : public IScreen {
private:
    AppContext& ctx;

    // Bộ lọc trên thanh công cụ
    int filterTypeIdx; // 0: Tất cả, 1: Sân 5, 2: Sân 7, 3: Sân 11
    char checkDateBuf[16];

    // Trạng thái mở Popup Modal
    bool openCreateModal;
    bool openDetailModal;
    std::string selectedFieldId;

    // Bộ đệm nhập liệu trong Popup
    char newIdBuf[32];
    char newNameBuf[128];
    int newTypeIdx;
    double newPrice;

    double editPrice;

    void renderCreateFieldModal();
    void renderFieldDetailAndScheduleModal();

public:
    explicit OwnerFieldScreen(AppContext& context);
    const char* getMenuLabel() const override { return "2. Quan ly San bong"; }
    void render() override;
};