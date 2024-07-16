//
// Created by kolby on 7/16/2024.
//

#ifndef NODE_ZIPPER_UTILS_H
#define NODE_ZIPPER_UTILS_H

#include <sstream>

std::vector<std::string> delimitString(const std::string& input, char delimiter) {
    std::vector<std::string> splitArray;
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            splitArray.push_back(item);
        }
    }

    return splitArray;
}

#endif //NODE_ZIPPER_UTILS_H
