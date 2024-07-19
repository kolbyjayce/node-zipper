import binary from '@mapbox/node-pre-gyp';
import path from 'path';
import { fileURLToPath } from 'url';
import { createRequire } from 'module';

const require = createRequire(import.meta.url);

// Convert `import.meta.url` to a file path
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const binding_path = binary.find(path.resolve(path.join(__dirname, '../../package.json')));
const { FileZip } = require(binding_path);

const exportObject = { FileZip };

export default exportObject;
export { FileZip };
