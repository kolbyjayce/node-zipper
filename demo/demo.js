import { FileZip } from "../lib/binding/node-zipper.js";

const zipper = new FileZip();
zipper.zip("/root/example/demo.txt", "C:/temp/a.txt");
zipper.zip("/root/example.txt", "C:/temp/a.txt");

zipper.save("C:/temp/example.zip");