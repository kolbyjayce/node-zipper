const FileZip = require('..');

const zipper = new FileZip();

console.log("Zipping file:");
const zippedPath = zipper.Zip("/path/to/file.txt");
console.log("Zipped file path:", zippedPath);

console.log("Saving file:");
const savedPath = zipper.Save("/path/to/save/file.zip");
console.log("Saved file path:", savedPath);