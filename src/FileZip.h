#pragma once

#include <napi.h>
#include "ZipStream.h"

class FileZip : public Napi::ObjectWrap<FileZip> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    FileZip(const Napi::CallbackInfo& info);

private:
    static Napi::FunctionReference constructor;

    Napi::Value Zip(const Napi::CallbackInfo& info);
    Napi::Value Save(const Napi::CallbackInfo& info);

    ZipStream zipStream;
};