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
    Napi::Value ZipAsync(const Napi::CallbackInfo& info);
    Napi::Value Save(const Napi::CallbackInfo& info);

    ZipStream zipStream;
};

class ZipAsyncWorker : public Napi::AsyncWorker {
public:
    ZipAsyncWorker(Napi::Function& callback, std::string pathToSave, std::string fileLocation)
        : Napi::AsyncWorker(callback), pathToSave(pathToSave), fileLocation(fileLocation), result(0) {}

    void Execute() override {
        result = ZipStream.Add(pathToSave, fileLocation);
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        Callback().Call({Env().Null()});
    }

    void OnError(const Napi::Error& e) override {
        Napi::HandleScope scope(Env());
        Callback().Call({e.Value()});
    }

private:
    std::string pathToSave;
    std::string fileLocation;
    int result;
};