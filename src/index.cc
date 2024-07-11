#include <nan.h>
#include "FileZip.h"

NAN_MODULE_INIT(InitModule) {
    FileZip::Init(target);
}

NODE_MODULE(FileZip, InitModule);