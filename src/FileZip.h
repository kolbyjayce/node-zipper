#pragma once

#include <nan.h>
#include "ZipStream.h"
class FileZip : public Nan::ObjectWrap {
  private:
    ZipStream zipStream;

  public:
    static NAN_MODULE_INIT(Init);
    static NAN_METHOD(New);
    static NAN_METHOD(Zip);
    static NAN_METHOD(Save);

    static Nan::Persistent<v8::FunctionTemplate> constructor;
};