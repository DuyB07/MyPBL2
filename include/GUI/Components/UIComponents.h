#pragma once
#include <string>
#include "imgui.h"

namespace UI {
    // Khung thẻ trắng có tiêu đề & đường gạch chân màu Cam
    void BeginSectionCard(const char* id, const char* title, float height = 0.0f, ImGuiWindowFlags extraFlags = ImGuiWindowFlags_None);
    void EndSectionCard();

    // Tiêu đề đầu trang + Nút hành động chính căn sát lề phải chuẩn xác
    bool PageHeader(const char* pageTitle, const char* subtitle = nullptr, const char* rightActionBtnLabel = nullptr);

    // Căn giữa hộp thoại Modal ở chính giữa màn hình
    void CenterNextModal(float width, float height = 0.0f);

    // Bảng 2 cột ẩn viền giúp căn thẳng hàng Nhãn và Ô nhập liệu trong Form
    bool BeginFormTable(const char* id, float labelColWidth = 155.0f);
    void FormRowLabel(const char* label);
    void EndFormTable();

    // Các loại nút bấm theo ngữ nghĩa
    bool PrimaryButton(const char* label, const ImVec2& size = ImVec2(0, 0));
    bool SecondaryButton(const char* label, const ImVec2& size = ImVec2(0, 0));
    bool DangerButton(const char* label, const ImVec2& size = ImVec2(0, 0));
    bool SuccessButton(const char* label, const ImVec2& size = ImVec2(0, 0));

    // Nhãn trạng thái (Badge) tiếng Việt dễ hiểu
    void StatusBadge(const std::string& statusText);

    // Định dạng số tiền có dấu phẩy hàng nghìn (VD: 240000 -> "240,000 VNĐ")
    std::string FormatMoney(double amount);

    // Hiển thị thông báo lịch sự khi bảng trống
    void RenderEmptyState(const char* message);

    // Thẻ KPI thống kê
    void MetricCard(const char* id, const char* title, const std::string& value, const char* caption, bool highlightOrange = false, float width = 0.0f);
}