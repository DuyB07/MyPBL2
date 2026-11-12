#pragma once
#include <stdexcept>
#include <utility>

template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
        Node(T&& val) : data(std::move(val)), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    std::size_t count;

public:
    // 1. Constructor mặc định
    Queue() : head(nullptr), tail(nullptr), count(0) {}

    // 2. Destructor
    ~Queue() {
        clear();
    }

    // 3. Copy Constructor (Deep Copy)
    Queue(const Queue& other) : head(nullptr), tail(nullptr), count(0) {
        Node* current = other.head;
        while (current != nullptr) {
            enqueue(current->data);
            current = current->next;
        }
    }

    // 4. Copy Assignment
    Queue& operator=(const Queue& other) {
        if (this != &other) {
            clear();
            Node* current = other.head;
            while (current != nullptr) {
                enqueue(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    // 5. Move Constructor
    Queue(Queue&& other) noexcept : head(other.head), tail(other.tail), count(other.count) {
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }

    // 6. Move Assignment
    Queue& operator=(Queue&& other) noexcept {
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

    // Thêm phần tử vào cuối hàng đợi - O(1)
    void enqueue(const T& item) {
        Node* newNode = new Node(item);
        if (tail == nullptr) { // Hàng đợi rỗng
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    // Lấy và xóa phần tử ở đầu hàng đợi - O(1)
    T dequeue() {
        if (empty()) {
            throw std::out_of_range("Queue is empty!");
        }
        Node* temp = head;
        T value = std::move(temp->data);
        head = head->next;
        if (head == nullptr) { // Nếu lấy xong hàng đợi trở nên rỗng
            tail = nullptr;
        }
        delete temp;
        count--;
        return value;
    }

    // Xem phần tử đầu hàng đợi mà không xóa - O(1)
    T& front() {
        if (empty()) throw std::out_of_range("Queue is empty!");
        return head->data;
    }

    const T& front() const {
        if (empty()) throw std::out_of_range("Queue is empty!");
        return head->data;
    }

    void clear() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        count = 0;
    }

    bool empty() const { return count == 0; }
    std::size_t size() const { return count; }
};