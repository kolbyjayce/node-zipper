#include "FileZip.h"
#include <string>

Napi::FunctionReference FileZip::constructor;

Napi::Object FileZip::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "FileZip", {
        InstanceMethod("Zip", &FileZip::Zip),
        InstanceMethod("Save", &FileZip::Save)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("FileZip", func);
    return exports;
}

FileZip::FileZip(const Napi::CallbackInfo& info) : Napi::ObjectWrap<FileZip>(info) {
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Expected zero arguments").ThrowAsJavaScriptException();
        return;
    }
}

Napi::Value FileZip::Zip(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "Two string arguments required: pathToSave and fileLocation").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string pathToSaveStr = info[0].As<Napi::String>().Utf8Value();
    std::string fileLocationStr = info[1].As<Napi::String>().Utf8Value();

    int result = zipStream.Add(pathToSaveStr, fileLocationStr);

    if (result != 0) {
        std::string errorMsg = "Failed to add file to zip. Error code: " + std::to_string(result);
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Undefined();
}

Napi::Value FileZip::ZipAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 3 || !info[0].IsString() || !info[1].IsString() || !info[2].IsFunction()) {
        Napi::TypeError::New(env, "Two string arguments and one callback function required").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string pathToSaveStr = info[0].As<Napi::String>().Utf8Value();
    std::string fileLocationStr = info[1].As<Napi::String>().Utf8Value();
    Napi::Function callback = info[2].As<Napi::Function>();

    ZipAsyncWorker* worker = new ZipAsyncWorker(callback, pathToSaveStr, fileLocationStr);
    worker->Queue();

    return env.Undefined();
}

Napi::Value FileZip::Save(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Path argument must be a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string outputPath = info[0].As<Napi::String>().Utf8Value();

    int result = zipStream.Save(outputPath);

    if (result != 0) {
        std::string errorMsg = "Failed to save zip file. Error code: " + std::to_string(result);
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Undefined();
}