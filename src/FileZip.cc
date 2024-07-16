#include <nan.h>
#include <string>
#include "FileZip.h"

Nan::Persistent<v8::FunctionTemplate> FileZip::constructor;

// Initialize NAN module
NAN_MODULE_INIT(FileZip::Init) {
  v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(FileZip::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("FileZip").ToLocalChecked());

  Nan::SetPrototypeMethod(ctor, "Zip", Zip);
  Nan::SetPrototypeMethod(ctor, "Save", Save);

  v8::Local<v8::Function> function = Nan::GetFunction(ctor).ToLocalChecked();
  Nan::Set(target, Nan::New("FileZip").ToLocalChecked(), function);
}

NAN_METHOD(FileZip::New) {
  // check that 'New' keyword was used
  if(!info.IsConstructCall()) {
    return Nan::ThrowError(Nan::New("FileZip::New - called without new keyword").ToLocalChecked());
  }

  // no arguments given during constructor
  if (info.Length() != 0) {
    return Nan::ThrowError(Nan::New("Expected zero arguments.").ToLocalChecked());
  }

  FileZip* zipper = new FileZip();
  zipper->Wrap(info.Holder());

  info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(FileZip::Save) {
    if (info.Length() < 1 || !info[0]->IsString()) {
      return Nan::ThrowTypeError("Path argument must be a string");
    }
    
    v8::String::Utf8Value path(info.GetIsolate(), info[0]);
    std::string pathStr(*path);

    // Placeholder for actual save implementation
    // For now, we'll just return the path
    info.GetReturnValue().Set(info[0]);
}

NAN_METHOD(FileZip::Zip) {
    if (info.Length() < 1 || !info[0]->IsString()) {
      return Nan::ThrowTypeError("Path argument must be a string");
    }
    
    v8::String::Utf8Value path(info.GetIsolate(), info[0]);
    std::string pathStr(*path);

    FileZip* obj = Nan::ObjectWrap::Unwrap<FileZip>(info.Holder());

    std::string testResult = obj->zipStream.Test(); 

    info.GetReturnValue().Set(Nan::New(testResult).ToLocalChecked());
}