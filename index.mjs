import { createRequire } from "node:module";

const require = createRequire(import.meta.url);
const FileZip = require('./index.js');

export default FileZip;