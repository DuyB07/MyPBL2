#include "Storage/FileManager.h"
#include <fstream>
#include <sstream>
#include <cctype>

namespace Storage {

    bool FileManager::readAllLines(const std::string& path, LinkedList<std::string>& outLines, std::string& outError) {
        outLines.clear();
        std::ifstream file(path);
        
        if (!file.is_open()) {
            outError = "Cannot open file for reading: " + path;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                outLines.pushBack(line);
            }
        }
        
        file.close();
        return true;
    }

    bool FileManager::writeAllLines(const std::string& path, const LinkedList<std::string>& lines, std::string& outError) {
        std::ofstream file(path, std::ios::trunc); // trunc để ghi đè
        
        if (!file.is_open()) {
            outError = "Cannot open file for writing: " + path;
            return false;
        }

        for (const auto& line : lines) {
            file << line << "\n";
        }
        
        file.close();
        return true;
    }

    bool FileManager::appendLine(const std::string& path, const std::string& line, std::string& outError) {
        std::ofstream file(path, std::ios::app); // app để ghi thêm
        
        if (!file.is_open()) {
            outError = "Cannot open file for appending: " + path;
            return false;
        }

        file << line << "\n";
        file.close();
        return true;
    }

    LinkedList<std::string> FileManager::split(const std::string& str, char delimiter) {
        LinkedList<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.pushBack(token);
        }
        
        // Xử lý trường hợp chuỗi kết thúc bằng delimiter (ví dụ: "A|B|")
        if (!str.empty() && str.back() == delimiter) {
            tokens.pushBack("");
        }
        
        return tokens;
    }

    std::string FileManager::join(const LinkedList<std::string>& tokens, char delimiter) {
        std::string result = "";
        bool first = true;
        
        for (const auto& token : tokens) {
            if (!first) {
                result += delimiter;
            }
            result += token;
            first = false;
        }
        return result;
    }

    std::string FileManager::trim(const std::string& str) {
        auto start = str.begin();
        while (start != str.end() && std::isspace(static_cast<unsigned char>(*start))) {
            ++start;
        }

        // Nếu chuỗi rỗng hoặc toàn khoảng trắng thì trả về chuỗi rỗng ngay
        if (start == str.end()) {
            return "";
        }

        auto end = str.end();
        do {
            --end;
        } while (end != start && std::isspace(static_cast<unsigned char>(*end)));

        return std::string(start, end + 1);
    }
}