const {FileZip} = require('../build/Release/FileZip');

const zipper = new FileZip();
zipper.Zip("/root/example/demo.txt", "C:/temp/a.txt");
// zipper.Zip("/root/example.txt", "C:\\temp\\a.txt");
// zipper.Zip("/root/csv/Property Export.csv", "C:\\Users\\kolby\\Documents\\Property Export Formatted.csv");
// zipper.Zip("/root/word/Project Outline.docx", "C:\\Users\\kolby\\Documents\\Project Outline.docx");
// zipper.Zip("/root/pdf/ResumeEx.pdf", "C:\\Users\\kolby\\Documents\\Resumes\\ResumeKolbyC.pdf")

zipper.Save("C:/temp/example.zip");

// console.log("Zipping file:");
// const zippedPath = zipper.Zip("/path/to/file.txt");
// console.log("Zipped file path:", zippedPath);

// console.log("Saving file:");
// const savedPath = zipper.Save("/path/to/save/file.zip");
// console.log("Saved file path:", savedPath);