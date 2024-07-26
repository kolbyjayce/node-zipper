#pragma once

#include <string>
//#include "PathTrie.h"
#include "HashTable.h"
#include <iostream>
#include <cstdint>

struct fileProperties {
    uint32_t fileSize;
    uint32_t crc32;
    time_t lastModTime;
};

class ZipStream {
public:
    // Empty constructor
    ZipStream();

    int Add(std::string pathToSave, std::string fileLocation);
    int Save(std::string outputPath);

private:
    HashTable hashTablePaths;
    uint16_t* centralDirectoryCount = 0;
    uint32_t localHeaderOffset = 0x00;
    uint32_t centralDirectoryHeaderOffset = 0x00;
    std::vector<std::vector<uint8_t>> centralDirectoryRecords;
    std::vector<std::vector<uint8_t>> localFileHeaders;

    // methods to create sections of zip file
    std::vector<uint8_t> createCentralDirectoryRecord(const std::string& path);
    std::vector<uint8_t> createCentralDirectoryRecord(const std::string& path, fileProperties& props);
    std::vector<uint8_t> createLocalFileHeader(const std::string& path); // this is for folders
    std::vector<uint8_t> createLocalFileHeader(const std::string& path, fileProperties &props, const std::vector<uint8_t>& file); // overload for files
    std::vector<uint8_t> createEOCDRecord();
};