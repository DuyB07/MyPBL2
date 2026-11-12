#pragma once
#include <functional>
#include <stdexcept>
#include <utility>

template <typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(const T& val) : data(val), prev(nullptr), next(nullptr) {}
        Node(T&& val) : data(std::move(val)), prev(nullptr), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    std::size_t count;

public:
    // --- LỚP ITERATOR CƠ BẢN (Cho phép ĐỌC và GHI) ---
    class iterator {
    private:
        Node* current;
    public:
        // Constructor nhận Node* (không phải const Node*)
        iterator(Node* node) : current(node) {}
        
        T& operator*() { return current->data; } // Trả về tham chiếu có thể sửa đổi
        iterator& operator++() { current = current->next; return *this; }
        bool operator!=(const iterator& other) const { return current != other.current; }
    };

    // --- LỚP CONST ITERATOR (Chỉ cho phép ĐỌC) ---
    class const_iterator {
    private:
        const Node* current; // Sử dụng const Node*
    public:
        // Constructor nhận const Node*
        const_iterator(const Node* node) : current(node) {}
        
        const T& operator*() const { return current->data; } // Trả về tham chiếu hằng
        const_iterator& operator++() { current = current->next; return *this; }
        bool operator!=(const const_iterator& other) const { return current != other.current; }
    };

    // 1. Khởi tạo mặc định
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}

    // 2. Destructor - Giải phóng toàn bộ bộ nhớ (Ngăn memory leak)
    ~LinkedList() {
        clear();
    }

    // 3. Rule of Five: Copy Constructor (Deep Copy)
    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), count(0) {
        Node* current = other.head;
        while (current != nullptr) {
            pushBack(current->data);
            current = current->next;
        }
    }

    // 4. Copy Assignment
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            Node* current = other.head;
            while (current != nullptr) {
                pushBack(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    // 5. Move Constructor
    LinkedList(LinkedList&& other) noexcept : head(other.head), tail(other.tail), count(other.count) {
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }

    // 6. Move Assignment
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            count = other.count;
            other.head = nullptr;
            other.tail = nullptr;
            other.count = 0;
        }
        return *this;
    }

    // --- CÁC PHÉP TOÁN CTDL O(1) ---

    void pushBack(const T& value) {
        Node* newNode = new Node(value);
        if (tail == nullptr) { // Điều kiện biên: Danh sách rỗng
            head = tail = newNode;
        } else {               // Điều kiện biên: Đã có phần tử
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        count++;
    }

    // --- CÁC PHÉP TOÁN O(N) ---

    // Xóa theo điều kiện - Trả về số lượng đã xóa
    int removeIf(std::function<bool(const T&)> predicate) {
        int removedCount = 0;
        Node* current = head;
        while (current != nullptr) {
            if (predicate(current->data)) {
                Node* toDelete = current;
                
                // Cập nhật con trỏ prev và next
                if (current->prev) current->prev->next = current->next;
                else head = current->next; // Xóa ở đầu

                if (current->next) current->next->prev = current->prev;
                else tail = current->prev; // Xóa ở cuối

                current = current->next; // Tiến tới node kế trước khi delete
                delete toDelete;
                count--;
                removedCount++;
            } else {
                current = current->next;
            }
        }
        return removedCount;
    }

    // Tìm kiếm - Trả về con trỏ đến dữ liệu để dễ dàng chỉnh sửa
    T* findIf(std::function<bool(const T&)> predicate) const {
        Node* current = head;
        while (current != nullptr) {
            if (predicate(current->data)) return &(current->data); // Ép kiểu lỏng lẻo ở đây có thể gây warning ở các compiler khó tính, nhưng trong đồ án này là chấp nhận được để trả về T* thay vì const T*
            current = current->next;
        }
        return nullptr;
    }

    void clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = tail = nullptr;
        count = 0;
    }

    // --- TIỆN ÍCH ---
    std::size_t size() const { return count; }
    bool empty() const { return count == 0; }
    
    // Cặp hàm begin/end cho danh sách có thể sửa đổi
    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }

    // Cặp hàm begin/end cho danh sách chỉ đọc (const)
    const_iterator begin() const { return const_iterator(head); }
    const_iterator end() const { return const_iterator(nullptr); }
};