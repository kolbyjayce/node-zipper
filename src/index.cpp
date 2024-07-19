#include <napi.h>
#include "FileZip.h"

Napi::Object InitModule(Napi::Env env, Napi::Object exports) {
    return FileZip::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitModule)