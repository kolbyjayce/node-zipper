#include "PathTrie.h"
#include <sstream>
#include <stdexcept>

// TrieNode methods
bool TrieNode::hasChild(const std::string& key) const {
    return children.find(key) != children.end();
}

TrieNode* TrieNode::getChild(const std::string& key) {
    if (hasChild(key)) {
        return children[key];
    }
    return nullptr;
}

void TrieNode::addChild(const std::string& key, TrieNode* node) {
    children[key] = node;
}

// PathIterator methods
PathIterator::PathIterator(const std::vector<std::string>& p) : paths(p), currentIndex(0) {}

bool PathIterator::hasNext() const {
    return currentIndex < paths.size();
}

std::string PathIterator::next() {
    if (!hasNext()) {
        throw std::out_of_range("No more paths");
    }
    return paths[currentIndex++];
}

void PathIterator::reset() {
    currentIndex = 0;
}

// PathTrie methods
PathTrie::PathTrie() : root(new TrieNode()) {}

PathTrie::~PathTrie() {
    // Implement proper cleanup (delete all nodes)
}

std::vector<std::string> PathTrie::splitPath(const std::string& path) const {
    std::vector<std::string> parts;
    std::istringstream ss(path);
    std::string token;
    while (std::getline(ss, token, '/')) {
        if (!token.empty()) {
            parts.push_back(token);
        }
    }
    return parts;
}

void PathTrie::addPath(const std::string& path) {
    std::vector<std::string> parts = splitPath(path);
    TrieNode* currentNode = root;

    for (const auto& part : parts) {
        if (!currentNode->hasChild(part)) {
            currentNode->addChild(part, new TrieNode());
        }
        currentNode = currentNode->getChild(part);
    }
}

bool PathTrie::pathExists(const std::string& path) const {
    std::vector<std::string> parts = splitPath(path);
    TrieNode* currentNode = root;

    for (const auto& part : parts) {
        if (!currentNode->hasChild(part)) {
            return false;
        }
        currentNode = currentNode->getChild(part);
    }
    return true;
}

std::vector<std::string> PathTrie::getAllPaths() const {
    std::vector<std::string> paths;
    getAllPathsHelper(root, "", paths);
    return paths;
}

void PathTrie::getAllPathsHelper(const TrieNode* node, std::string currentPath, std::vector<std::string>& paths) const {
    if (node->children.empty()) {
        paths.push_back(currentPath);
        return;
    }

    for (const auto& child : node->children) {
        std::string newPath = currentPath + "/" + child.first;
        getAllPathsHelper(child.second, newPath, paths);
    }
}

PathIterator PathTrie::getIterator() const {
    return PathIterator(getAllPaths());
}