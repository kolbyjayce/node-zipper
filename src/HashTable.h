//
// Created by kolby on 7/17/2024.
//

#ifndef NODE_ZIPPER_HASHTABLE_H
#define NODE_ZIPPER_HASHTABLE_H


#include <string>
#include <vector>
#include <list>

class HashTable {
private:
    static const int DEFAULT_SIZE = 30;
    std::vector<std::list<std::string>> table;
    int size;

    int hash(const std::string& key) const;

public:
    HashTable(int size = DEFAULT_SIZE);

    void insert(const std::string& value);
    bool remove(const std::string& value);
    bool contains(const std::string& value) const;
    void clear();
    int getSize() const;
};


#endif //NODE_ZIPPER_HASHTABLE_H
