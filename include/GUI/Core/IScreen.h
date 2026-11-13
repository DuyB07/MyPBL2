#pragma once

class IScreen {
public:
    virtual ~IScreen() = default;

    // Tên hiển thị trên thanh Menu dọc (Left Sidebar)
    virtual const char* getMenuLabel() const = 0;

    // Hàm thuần ảo vẽ nội dung màn hình (Đa hình động)
    virtual void render() = 0;
};