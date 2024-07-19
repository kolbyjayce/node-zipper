//
// Created by kolby on 7/17/2024.
//

#include "HashTable.h"
#include <algorithm>

HashTable::HashTable(int size) : size(size) {
    table.resize(size);
}

int HashTable::hash(const std::string& key) const {
    unsigned long hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % size;
}

void HashTable::insert(const std::string& value) {
    int index = hash(value);
    if (std::find(table[index].begin(), table[index].end(), value) == table[index].end()) {
        table[index].push_back(value);
    }
}

bool HashTable::remove(const std::string& value) {
    int index = hash(value);
    auto& bucket = table[index];
    auto it = std::find(bucket.begin(), bucket.end(), value);
    if (it != bucket.end()) {
        bucket.erase(it);
        return true;
    }
    return false;
}

bool HashTable::contains(const std::string& value) const {
    int index = hash(value);
    const auto& bucket = table[index];
    return std::find(bucket.begin(), bucket.end(), value) != bucket.end();
}

void HashTable::clear() {
    for (auto& bucket : table) {
        bucket.clear();
    }
}

int HashTable::getSize() const {
    return size;
}