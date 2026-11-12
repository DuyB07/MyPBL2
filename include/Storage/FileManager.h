#pragma once
#include <string>
#include "Core/LinkedList.h"

namespace Storage {

class FileManager {
public:
    // Đọc toàn bộ file. Trả về true nếu thành công, false nếu lỗi (kèm thông báo trong outError).
    static bool readAllLines(const std::string& path, LinkedList<std::string>& outLines, std::string& outError);

    // Ghi toàn bộ danh sách chuỗi đè lên file.
    static bool writeAllLines(const std::string& path, const LinkedList<std::string>& lines, std::string& outError);

    // Ghi thêm một dòng vào cuối file.
    static bool appendLine(const std::string& path, const std::string& line, std::string& outError);

    // Tách chuỗi theo ký tự phân cách (ví dụ: "A|B|C" -> ["A", "B", "C"])
    static LinkedList<std::string> split(const std::string& str, char delimiter = '|');

    // Ghép danh sách chuỗi thành 1 chuỗi (ví dụ: ["A", "B", "C"] -> "A|B|C")
    static std::string join(const LinkedList<std::string>& tokens, char delimiter = '|');

    // Loại bỏ khoảng trắng thừa ở 2 đầu chuỗi
    static std::string trim(const std::string& str);
};

} // namespace Storage