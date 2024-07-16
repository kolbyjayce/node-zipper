#ifndef PATH_TRIE_H
#define PATH_TRIE_H

#include <string>
#include <vector>
#include <map>

class TrieNode {
public:
    std::map<std::string, TrieNode*> children;

    bool hasChild(const std::string& key) const;
    TrieNode* getChild(const std::string& key);
    void addChild(const std::string& key, TrieNode* node);
};

class PathIterator {
private:
    std::vector<std::string> paths;
    size_t currentIndex;

public:
    PathIterator(const std::vector<std::string>& p);
    bool hasNext() const;
    std::string next();
    void reset();
};

class PathTrie {
private:
    TrieNode* root;
    std::vector<std::string> splitPath(const std::string& path) const;
    void getAllPathsHelper(const TrieNode* node, std::string currentPath, std::vector<std::string>& paths) const;

public:
    PathTrie();
    ~PathTrie();
    void addPath(const std::string& path);
    bool pathExists(const std::string& path) const;
    std::vector<std::string> getAllPaths() const;
    PathIterator getIterator() const;
};

#endif // PATH_TRIE_H