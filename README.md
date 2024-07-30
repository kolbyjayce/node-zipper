# Zipper JS

Concise documentation on learning zip file specs found [here](https://medium.com/@felixstridsberg/the-zip-file-format-6c8a160d1c34)

## Using this library

This library was created to be a quick, efficient solution to adding files into a zip archive. It is a very primative implementation and should be thouroughly tested before using in a production environment.

As of version 0.1.1, asynchronous calls are not supported. Adding files into an archive will block the node event cycle while running.

To use this library:
```javascript
import { FileZip } from "node-zipper"; // import syntax
const { FileZip } = require("node-zipper"); // commonJS syntax

const zipper = new FileZip();

// first argument is location to save to inside zip folder, second argument is path to file (must be local path to file)
zipper.zip("path/in/zip/file.txt", "/path/to/file.txt");
// ... repeat

zipper.save("/path/to/save/example.zip");
```

### Current Limitations
- The path to fetch a file must be a local file system path. Currently, Buffers and other data types are not supported
- Explicit file names are required when zipping and saving content

## Contributing

To start a development environment, follow the steps below.

### Download python (if needed)

Python 3.x.x is required for this project

### Compile package
```bash
npm install
```

This will install all needed dependencies along with building the module. Once built, javascript demo can be written to import library from "lib/binding/Release"

Subsequent compilations after code changes can be compiled with
```bash
npm build
```

This will recompile c++ source code to Javascript to be tested again.

Alternatively, by creating a CMakeLists.txt file with ZipStream, HashTable, and CRC32, a fully functional c++ project can be created for testing independent of the Javascript ecosystem and FileZip class declarations. This allows for fast development focusing on C++ code.

### Repository Structure

- `lib`: contains all binaries to be disributed with npm
- `demo`: simple javascript demo of how to use package and to ensure it is working correctly
- `test`: mocha tests to ensure functionality
- `src`: c++ source code
  - In this folder, FileZip.h/cpp and index.cpp are the only file interacting with the nodeV8 engine. This was designed intentionally in order to write standard C++ logic that is modular to the node file systenm
  - By copying out CRC32, ZipStream, and HashTable, a fully functional zip library can be reused for C/C++ development
