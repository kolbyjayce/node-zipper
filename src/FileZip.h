#pragma once

#include <napi.h>
#include "ZipStream.h"

class FileZip : public Napi::ObjectWrap<FileZip> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    FileZip(const Napi::CallbackInfo& info);

private:
    static Napi::FunctionReference constructor;

    Napi::Value zip(const Napi::CallbackInfo& info);
    Napi::Value zipAsync(const Napi::CallbackInfo& info);
    Napi::Value save(const Napi::CallbackInfo& info);

    ZipStream zipStream;
};

class ZipAsyncWorker : public Napi::AsyncWorker {
public:
    ZipAsyncWorker(Napi::Function& callback, ZipStream* zipStream, std::string pathToSave, std::string fileLocation)
        : Napi::AsyncWorker(callback), zipStream(zipStream), pathToSave(pathToSave), fileLocation(fileLocation), result(0) {}

    void Execute() override {
        result = zipStream->Add(pathToSave, fileLocation);
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        if (result != 0) {
            std::string errorMsg = "Failed to add file to zip. Error code: " + std::to_string(result);
            Callback().Call({Napi::Error::New(Env(), errorMsg).Value()});
        } else {
            Callback().Call({Env().Null(), Napi::Number::New(Env(), result)});
        }
    }

private:
    ZipStream* zipStream;
    std::string pathToSave;
    std::string fileLocation;
    int result;
};