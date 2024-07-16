#include "ZipStream.h"
#include "PathTrie.h"
#include "utils.h"
#include <iostream>

// empty constructor
ZipStream::ZipStream() { }

std::string ZipStream::Test() const {
  return "Hello World!";
}

int ZipStream::Add(std::string path) {
  // parse path to see if folder exists already

  // if doesn't exist, create central directory header for it

  // read contents of file
  return 0;
}

/**
 * pathToSave - where to save the file inside of the zipped directory
 * fileLocation - path to file so that it can be read
 */
int ZipStream::Add(std::string pathToSave, std::string fileLocation) {
    // parse path to get file name and path to save to
    size_t lastSlash = pathToSave.find_last_of("/\\");

    std::string fileName = pathToSave.substr(lastSlash + 1);
    std::string restOfPath = pathToSave.substr(0, lastSlash);

    // If restOfPath is empty, it means the path started with a slash
    if (restOfPath.empty() && !pathToSave.empty()) {
        restOfPath = pathToSave[0];
    }
    std::cout << fileName << restOfPath << std::endl;

    // if path doesn't exist, add to trie and then create central directory and local header records
    if (!paths.pathExists(restOfPath)) {
        paths.addPath(restOfPath);

        // create central directory here

        // create local header here

        // increment central directory count
    }
    // read file contents

    // create local file header

    // append file data

    // increment central directory record count

    return 0;
}