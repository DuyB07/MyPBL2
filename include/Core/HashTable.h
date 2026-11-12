#pragma once
#include <string>
#include <functional>
#include "Core/LinkedList.h"

template <typename K, typename V>
class HashTable {
private:
    struct Entry {
        K key;
        V value;
        Entry(const K& k, const V& v) : key(k), value(v) {}
    };

    LinkedList<Entry>* buckets;
    std::size_t numBuckets;
    std::size_t numElements;

    std::size_t hashFunction(const K& key) const {
        return std::hash<K>{}(key) % numBuckets;
    }

    void rehash() {
        std::size_t oldNumBuckets = numBuckets;
        LinkedList<Entry>* oldBuckets = buckets;

        numBuckets *= 2;
        buckets = new LinkedList<Entry>[numBuckets];
        numElements = 0;

        for (std::size_t i = 0; i < oldNumBuckets; ++i) {
            for (const auto& entry : oldBuckets[i]) {
                insert(entry.key, entry.value); // insert lại vào mảng bucket mới
            }
        }
        delete[] oldBuckets;
    }

public:
    HashTable(std::size_t initialCapacity = 11) : numBuckets(initialCapacity), numElements(0) {
        buckets = new LinkedList<Entry>[numBuckets];
    }

    ~HashTable() {
        delete[] buckets;
    }

    // Xóa Copy Constructor và Assignment Operator để tránh treo con trỏ bộ nhớ động
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    void insert(const K& key, const V& value) {
        if (loadFactor() >= 0.75f) {
            rehash();
        }

        std::size_t index = hashFunction(key);
        auto found = buckets[index].findIf([&key](const Entry& e) { return e.key == key; });
        if (found) {
            found->value = value; // Cập nhật nếu khóa đã tồn tại
        } else {
            buckets[index].pushBack(Entry(key, value)); // Thêm mới
            numElements++;
        }
    }

    V* find(const K& key) const {
        std::size_t index = hashFunction(key);
        auto found = buckets[index].findIf([&key](const Entry& e) { return e.key == key; });
        if (found) {
            return &(found->value);
        }
        return nullptr;
    }

    bool remove(const K& key) {
        std::size_t index = hashFunction(key);
        int removedCount = buckets[index].removeIf([&key](const Entry& e) { return e.key == key; });
        if (removedCount > 0) {
            numElements--;
            return true;
        }
        return false;
    }

    void clear() {
        for (std::size_t i = 0; i < numBuckets; ++i) {
            buckets[i].clear();
        }
        numElements = 0;
    }

    std::size_t size() const { return numElements; }
    float loadFactor() const { return static_cast<float>(numElements) / numBuckets; }
};