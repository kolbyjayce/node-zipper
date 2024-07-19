#include "ZipStream.h"
#include "CRC32.h"

#include <iostream>
#include <fstream>

// Utility Functions outside class
uint32_t calculateCRC32(const std::vector<uint8_t> input) {
    CRC32 crc;
    size_t numBytes = input.size();
    for (size_t  i = 0; i < numBytes; i++) {
        crc.update(input[i]);
    }
    uint32_t checksum = crc.finalize();

    return checksum;
}


// empty constructor
ZipStream::ZipStream() { }

/**
 * pathToSave - where to save the file inside of the zipped directory
 * fileLocation - path to file so that it can be read
 */
int ZipStream::Add(std::string pathToSave, std::string fileLocation) {
    // parse path to get file name and path to save to
    size_t lastSlash = pathToSave.find_last_of('/');

    std::string fileName = pathToSave.substr(lastSlash + 1);
    std::string restOfPath = pathToSave.substr(0, lastSlash);

    // If restOfPath is empty, it means the path started with a slash
    if (restOfPath.empty() && !pathToSave.empty()) {
        restOfPath = "/";
    }

    // Ensure path ends with a forward slash
    if (!restOfPath.empty() && restOfPath.back() != '/') {
        restOfPath += '/';
    }

    // remove leading slash if it was passed in
    if (restOfPath[0] == '/') {
        restOfPath = restOfPath.substr(1);
    }

    // create variable for full path without leading slash if it was provided
    std::string formattedFullPath;
    if (pathToSave[0] == '/') {
        formattedFullPath = pathToSave.substr(1);
    } else {
        formattedFullPath = pathToSave;
    }

    // if path doesn't exist, add to hash table and then create central directory and local header records
    if (!hashTablePaths.contains(restOfPath)) {
        hashTablePaths.insert(restOfPath);

        // create central directory, increment central directory count
        std::vector<uint8_t > record = createCentralDirectoryRecord(restOfPath);
        centralDirectoryRecords.push_back(record);
        setCentralDirectoryCount(centralDirectoryCount + 1);
        centralDirectoryHeaderOffset += static_cast<uint32_t>(record.size());

        // create local header here, increment local header offset
        std::vector<uint8_t> folderHeader = createLocalFileHeader(restOfPath);
        localFileHeaders.push_back(folderHeader);

        centralDirectoryHeaderOffset += static_cast<uint32_t>(folderHeader.size());
        localHeaderOffset += static_cast<uint32_t>(folderHeader.size());
    }

    // Create file records

    // read file contents
    std::vector<uint8_t> fileContents;
    std::ifstream file(fileLocation, std::ios::binary);

    if (file.is_open()) {
        char buffer[4096];
        while (file.read(buffer, sizeof(buffer))) {
            fileContents.insert(fileContents.end(), buffer, buffer + file.gcount());
        }
        fileContents.insert(fileContents.end(), buffer, buffer + file.gcount());

        if (file.bad()) {
            std::cerr << "Error: I/O error while reading file " << fileLocation << std::endl;
            return -1; // TODO: turn this into a thrown error
        }
    } else {
        std::cerr << "Error: Unable to open file " << fileLocation << std::endl;
        return -1; // TODO: turn this into a thrown error
    }

    std::streamsize fileSize = fileContents.size();

//    std::vector<uint8_t> fileContents(fileSize);
//    if (!file.read(reinterpret_cast<char*>(fileContents.data()), fileSize)) {
//        std::cerr << "Error: Unable to read file " << fileLocation << std::endl;
//        return -1; // or handle error as appropriate
//    }
    file.close();

    // Calculate CRC32
    uint32_t crc32 = calculateCRC32(fileContents);


    fileProperties props{};
    props.fileSize = fileSize;
    props.crc32 = crc32;
    props.lastModTime = reinterpret_cast<time_t>(localtime(nullptr)); // TODO: get actual last modified time of file

    // create central directory, increment central directory count
    std::vector<uint8_t> record = createCentralDirectoryRecord(formattedFullPath, props);
    centralDirectoryRecords.push_back(record);
    setCentralDirectoryCount(centralDirectoryCount + 1);
    centralDirectoryHeaderOffset += static_cast<uint32_t>(record.size());

    // create local file header, increment local header offset
    std::vector<uint8_t> fileHeader = createLocalFileHeader(formattedFullPath, props, fileContents);
    localFileHeaders.push_back(fileHeader);
    // update offsets
    localHeaderOffset += static_cast<uint32_t>(fileHeader.size());
    centralDirectoryHeaderOffset += static_cast<uint32_t>(fileHeader.size());

    return 0;
}

int ZipStream::Save(std::string outputPath) {
    // construct EOCD record
    std::vector<uint8_t> eocdRecord = createEOCDRecord();

    // put all records together
    std::vector<uint8_t> zipContents;

    // Add all local file headers
    for (const auto& header : localFileHeaders) {
        zipContents.insert(zipContents.end(), header.begin(), header.end());
    }

    // Add all central directory records
    for (const auto& record : centralDirectoryRecords) {
        zipContents.insert(zipContents.end(), record.begin(), record.end());
    }

    // Add EOCD record
    zipContents.insert(zipContents.end(), eocdRecord.begin(), eocdRecord.end());

    // save to file
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        return -1; // Failed to open file
    }

    outFile.write(reinterpret_cast<const char*>(zipContents.data()), zipContents.size());

    if (!outFile) {
        return -2; // Failed to write data
    }

    outFile.close();
    if (!outFile) {
        return -3; // Failed to close file
    }

    return 0; // Success
}

// Private functions for constructing headers

// Create central directory, used for folders
std::vector<uint8_t> ZipStream::createCentralDirectoryRecord(const std::string &path) {
    std::vector<uint8_t> record;

    // Signature (4 bytes)
    uint32_t signature = 0x02014b50;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&signature), reinterpret_cast<uint8_t*>(&signature) + 4);

    // Version Made by (2 bytes) - version 2.0 for simplicity and compatibility
    uint16_t versionMadeBy = 0x0014;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&versionMadeBy), reinterpret_cast<uint8_t*>(&versionMadeBy) + 2);

    // Version needed to extract (2 bytes)
    uint16_t versionNeeded = 0x000A;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&versionNeeded), reinterpret_cast<uint8_t*>(&versionNeeded) + 2);

    // Bit flag (2 bytes) - none set
    uint16_t bitFlag = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&bitFlag), reinterpret_cast<uint8_t*>(&bitFlag) + 2);

    // Compression Method (2 bytes) - none for folders
    uint16_t compressionMethod = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&compressionMethod), reinterpret_cast<uint8_t*>(&compressionMethod) + 2);

    // File last modification (2 bytes) and date (2 bytes)
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);

    uint16_t modTime = (ltm->tm_hour << 11) | (ltm->tm_min << 5) | (ltm->tm_sec / 2);
    uint16_t modDate = ((ltm->tm_year - 80) << 9) | ((ltm->tm_mon + 1) << 5) | ltm->tm_mday;

    record.insert(record.end(), reinterpret_cast<uint8_t*>(&modTime), reinterpret_cast<uint8_t*>(&modTime) + 2);
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&modDate), reinterpret_cast<uint8_t*>(&modDate) + 2);

    // CRC-32 (4 bytes) - 0 for folders
    uint32_t crc32 = 0x00000000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&crc32), reinterpret_cast<uint8_t*>(&crc32) + 4);

    // Compressed size (4 bytes) - 0 for folders
    uint32_t compressedSize = 0x00000000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&compressedSize), reinterpret_cast<uint8_t*>(&compressedSize) + 4);

    // Uncompressed size (4 bytes) - 0 for folders
    uint32_t uncompressedSize = 0x00000000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&uncompressedSize), reinterpret_cast<uint8_t*>(&uncompressedSize) + 4);

    // File name length (2 bytes)
    uint16_t fileNameLength = path.length();
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&fileNameLength), reinterpret_cast<uint8_t*>(&fileNameLength) + 2);

    // Extra field length (2 bytes) - No extra field
    uint16_t extraFieldLength = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&extraFieldLength), reinterpret_cast<uint8_t*>(&extraFieldLength) + 2);

    // File comment length (2 bytes) - No comment
    uint16_t fileCommentLength = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&fileCommentLength), reinterpret_cast<uint8_t*>(&fileCommentLength) + 2);

    // Disk number where file starts (2 bytes) - Always first disk
    uint16_t diskNumber = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&diskNumber), reinterpret_cast<uint8_t*>(&diskNumber) + 2);

    // Internal file attributes (2 bytes)
    uint16_t internalFileAttr = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&internalFileAttr), reinterpret_cast<uint8_t*>(&internalFileAttr) + 2);

    // External file attributes (4 bytes) - Set directory bit
    uint32_t externalFileAttr = 0x10000010;  // Directory attribute for both DOS and UNIX
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&externalFileAttr), reinterpret_cast<uint8_t*>(&externalFileAttr) + 4);

    // Offset of local file header (4 bytes)
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&localHeaderOffset), reinterpret_cast<uint8_t*>(&localHeaderOffset) + 4);

    // File name
    record.insert(record.end(), path.begin(), path.end());

    // Directory records should end with forward slash
    if (!path.empty() && path.back() != '/') {
        record.push_back('/');
    }

    return record;
}

// create central directory record overload, used for files
std::vector<uint8_t> ZipStream::createCentralDirectoryRecord(const std::string &path, fileProperties &props) {
    std::vector<uint8_t> record;

    // Central directory file header signature (4 bytes)
    uint32_t signature = 0x02014b50;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&signature), reinterpret_cast<uint8_t*>(&signature) + 4);

    // Version made by (2 bytes)
    uint16_t versionMadeBy = 0x0014; // Version 2.0
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&versionMadeBy), reinterpret_cast<uint8_t*>(&versionMadeBy) + 2);

    // Version needed to extract (2 bytes)
    uint16_t versionNeeded = 0x000A; // Version 1.0
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&versionNeeded), reinterpret_cast<uint8_t*>(&versionNeeded) + 2);

    // General purpose bit flag (2 bytes)
    uint16_t bitFlag = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&bitFlag), reinterpret_cast<uint8_t*>(&bitFlag) + 2);

    // Compression method (2 bytes) - No compression
    uint16_t compressionMethod = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&compressionMethod), reinterpret_cast<uint8_t*>(&compressionMethod) + 2);

    // File last modification time (2 bytes) and date (2 bytes)
    tm* ltm = localtime(&props.lastModTime);
    uint16_t modTime = (ltm->tm_hour << 11) | (ltm->tm_min << 5) | (ltm->tm_sec / 2);
    uint16_t modDate = ((ltm->tm_year - 80) << 9) | ((ltm->tm_mon + 1) << 5) | ltm->tm_mday;

    record.insert(record.end(), reinterpret_cast<uint8_t*>(&modTime), reinterpret_cast<uint8_t*>(&modTime) + 2);
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&modDate), reinterpret_cast<uint8_t*>(&modDate) + 2);

    // CRC-32 (4 bytes)
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&props.crc32), reinterpret_cast<uint8_t*>(&props.crc32) + 4);

    // Compressed size (4 bytes) - Same as uncompressed size since no compression
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&props.fileSize), reinterpret_cast<uint8_t*>(&props.fileSize) + 4);

    // Uncompressed size (4 bytes)
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&props.fileSize), reinterpret_cast<uint8_t*>(&props.fileSize) + 4);

    // File name length (2 bytes)
    uint16_t fileNameLength = path.length();
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&fileNameLength), reinterpret_cast<uint8_t*>(&fileNameLength) + 2);

    // Extra field length (2 bytes) - No extra field
    uint16_t extraFieldLength = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&extraFieldLength), reinterpret_cast<uint8_t*>(&extraFieldLength) + 2);

    // File comment length (2 bytes) - No comment
    uint16_t fileCommentLength = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&fileCommentLength), reinterpret_cast<uint8_t*>(&fileCommentLength) + 2);

    // Disk number where file starts (2 bytes)
    uint16_t diskNumber = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&diskNumber), reinterpret_cast<uint8_t*>(&diskNumber) + 2);

    // Internal file attributes (2 bytes)
    uint16_t internalFileAttr = 0x0000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&internalFileAttr), reinterpret_cast<uint8_t*>(&internalFileAttr) + 2);

    // External file attributes (4 bytes)
    uint32_t externalFileAttr = 0x00000000;
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&externalFileAttr), reinterpret_cast<uint8_t*>(&externalFileAttr) + 4);

    // Relative offset of local header (4 bytes)
    record.insert(record.end(), reinterpret_cast<uint8_t*>(&localHeaderOffset), reinterpret_cast<uint8_t*>(&localHeaderOffset) + 4);

    // File name
    record.insert(record.end(), path.begin(), path.end());

    return record;
}

// create local file header for a folder
std::vector<uint8_t> ZipStream::createLocalFileHeader(const std::string &path) {
    std::vector<uint8_t> header;

    // Local file header signature (4 bytes)
    uint32_t signature = 0x04034b50;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&signature), reinterpret_cast<uint8_t*>(&signature) + 4);

    // Version needed to extract (2 bytes) - Use 1.0 for directories
    uint16_t versionNeeded = 0x000A;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&versionNeeded), reinterpret_cast<uint8_t*>(&versionNeeded) + 2);

    // bit flag (2 bytes) - No flags set
    uint16_t bitFlag = 0x0000;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&bitFlag), reinterpret_cast<uint8_t*>(&bitFlag) + 2);

    // Compression method (2 bytes) - No compression for folder
    uint16_t compressionMethod = 0x0000;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&compressionMethod), reinterpret_cast<uint8_t*>(&compressionMethod) + 2);

    // File last modification time (2 bytes) and date (2 bytes)
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);

    uint16_t modTime = (ltm->tm_hour << 11) | (ltm->tm_min << 5) | (ltm->tm_sec / 2);
    uint16_t modDate = ((ltm->tm_year - 80) << 9) | ((ltm->tm_mon + 1) << 5) | ltm->tm_mday;

    header.insert(header.end(), reinterpret_cast<uint8_t*>(&modTime), reinterpret_cast<uint8_t*>(&modTime) + 2);
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&modDate), reinterpret_cast<uint8_t*>(&modDate) + 2);

    // CRC-32 (4 bytes) - 0 for folders
    uint32_t crc32 = 0x00000000;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&crc32), reinterpret_cast<uint8_t*>(&crc32) + 4);

    // Compressed size (4 bytes) - 0 for folders
    uint32_t compressedSize = 0x00000000;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&compressedSize), reinterpret_cast<uint8_t*>(&compressedSize) + 4);

    // Uncompressed size (4 bytes) - 0 for folders
    uint32_t uncompressedSize = 0x00000000;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&uncompressedSize), reinterpret_cast<uint8_t*>(&uncompressedSize) + 4);

    // File name length (2 bytes)
    uint16_t fileNameLength = path.length();
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&fileNameLength), reinterpret_cast<uint8_t*>(&fileNameLength) + 2);

    // Extra field length (2 bytes) - No extra field
    uint16_t extraFieldLength = 0x0000;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&extraFieldLength), reinterpret_cast<uint8_t*>(&extraFieldLength) + 2);

    // File name
    header.insert(header.end(), path.begin(), path.end());

    return header;
}

// create local file header for a file, overload original method
std::vector<uint8_t> ZipStream::createLocalFileHeader(const std::string &path, fileProperties &props, const std::vector<uint8_t>& file) {
    std::vector<uint8_t> header;

    // Local file header signature (4 bytes)
    uint32_t signature = 0x04034b50;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&signature), reinterpret_cast<uint8_t*>(&signature) + 4);

    // Version needed to extract (2 bytes)
    uint16_t versionNeeded = 0x000A; // Version 1.0
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&versionNeeded), reinterpret_cast<uint8_t*>(&versionNeeded) + 2);

    // General purpose bit flag (2 bytes)
    uint16_t bitFlag = 0x0000;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&bitFlag), reinterpret_cast<uint8_t*>(&bitFlag) + 2);

    // Compression method (2 bytes) - No compression
    uint16_t compressionMethod = 0x0000;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&compressionMethod), reinterpret_cast<uint8_t*>(&compressionMethod) + 2);

    // File last modification time (2 bytes) and date (2 bytes)
    tm* ltm = localtime(&props.lastModTime);
    uint16_t modTime = (ltm->tm_hour << 11) | (ltm->tm_min << 5) | (ltm->tm_sec / 2);
    uint16_t modDate = ((ltm->tm_year - 80) << 9) | ((ltm->tm_mon + 1) << 5) | ltm->tm_mday;

    header.insert(header.end(), reinterpret_cast<uint8_t*>(&modTime), reinterpret_cast<uint8_t*>(&modTime) + 2);
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&modDate), reinterpret_cast<uint8_t*>(&modDate) + 2);

    // CRC-32 (4 bytes)
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&props.crc32), reinterpret_cast<uint8_t*>(&props.crc32) + 4);

    // Compressed size (4 bytes) - Same as uncompressed size since no compression
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&props.fileSize), reinterpret_cast<uint8_t*>(&props.fileSize) + 4);

    // Uncompressed size (4 bytes)
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&props.fileSize), reinterpret_cast<uint8_t*>(&props.fileSize) + 4);

    // File name length (2 bytes)
    uint16_t fileNameLength = path.length();
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&fileNameLength), reinterpret_cast<uint8_t*>(&fileNameLength) + 2);

    // Extra field length (2 bytes) - No extra field
    uint16_t extraFieldLength = 0x0000;
    header.insert(header.end(), reinterpret_cast<uint8_t*>(&extraFieldLength), reinterpret_cast<uint8_t*>(&extraFieldLength) + 2);

    // File name
    header.insert(header.end(), path.begin(), path.end());

    // Extra field would go here, but we're not using one

    // Append file contents
    header.insert(header.end(), file.begin(), file.end());

    return header;
}

// create EOCD Record when ready to save
std::vector<uint8_t> ZipStream::createEOCDRecord() {
    std::vector<uint8_t> eocdRecord;

    // EOCD signature (4 bytes)
    uint32_t signature = 0x06054b50;
    eocdRecord.insert(eocdRecord.end(), reinterpret_cast<uint8_t*>(&signature), reinterpret_cast<uint8_t*>(&signature) + 4);

    // Number of this disk (0) (2 bytes)
    uint16_t diskNumber = 0x0000;
    eocdRecord.insert(eocdRecord.end(), reinterpret_cast<uint8_t*>(&diskNumber), reinterpret_cast<uint8_t*>(&diskNumber) + 2);

    // Disk where central directory starts (0) (2 bytes)
    uint16_t centralDirDisk = 0x0000;
    eocdRecord.insert(eocdRecord.end(), reinterpret_cast<uint8_t*>(&centralDirDisk), reinterpret_cast<uint8_t*>(&centralDirDisk) + 2);

    // Number of central directory records on this disk (2 bytes)
    // and total number central directory records (2 bytes)
    uint16_t centralDirCount = static_cast<uint16_t>(centralDirectoryCount);
    eocdRecord.insert(eocdRecord.end(), reinterpret_cast<uint8_t*>(&centralDirCount), reinterpret_cast<uint8_t*>(&centralDirCount) + 2);
    eocdRecord.insert(eocdRecord.end(), reinterpret_cast<uint8_t*>(&centralDirCount), reinterpret_cast<uint8_t*>(&centralDirCount) + 2);

    // Size of central directory (4 bytes)
    uint32_t centralDirectorySize = centralDirectoryHeaderOffset - localHeaderOffset;
    eocdRecord.insert(eocdRecord.end(), reinterpret_cast<uint8_t*>(&centralDirectorySize), reinterpret_cast<uint8_t*>(&centralDirectorySize) + 4);

    // Offset of start of central directory
    eocdRecord.insert(eocdRecord.end(), reinterpret_cast<uint8_t*>(&localHeaderOffset), reinterpret_cast<uint8_t*>(&localHeaderOffset) + 4);

    // Comment length (0)
    uint16_t commentLength = 0;
    eocdRecord.insert(eocdRecord.end(), reinterpret_cast<uint8_t*>(&commentLength), reinterpret_cast<uint8_t*>(&commentLength) + 2);

    return eocdRecord;
}