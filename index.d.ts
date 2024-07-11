declare module 'zipper-js' {
  class FileZip {
    Zip(path: string): string;
    Save(path: string): string;
  }
  export = FileZip;
}