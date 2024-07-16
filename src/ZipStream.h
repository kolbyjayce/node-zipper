#pragma once

#include <string>
#include "PathTrie.h"
#include <iostream>

class ZipStream {
  public:
    // Empty constructor
    ZipStream();

    std::string Test() const;

    int Add(std::string path);
    int Add(std::string pathToSave, std::string fileLocation);

    // Getters and Setters
    int getCentralDirectoryCount() { return this->centralDirectoryCount; }
    void setCentralDirectoryCount(int input) { this->centralDirectoryCount = input; }

    void print() {
        PathIterator iterator = paths.getIterator();

        while (iterator.hasNext()) {
            std::cout << iterator.next() << std::endl;
        }
    }
  private:
    PathTrie paths;
    int centralDirectoryCount = 0;
    int centralDirectoryBufferOffset = 0x00;

    // methods to create sections of zip file
    // void createLocalFileHeader();
    // void createLocalFileHeader();
    // void createEOCDHeader();
};