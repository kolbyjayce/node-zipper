import { FileZip } from "../lib/binding/node-zipper.js";

const zipper = new FileZip();
zipper.Zip("/root/example/demo.txt", "C:/temp/a.txt");
zipper.Zip("/root/example.txt", "C:/temp/a.txt");

zipper.Save("C:/temp/example.zip");