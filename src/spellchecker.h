#ifndef SpellChecker_H
#define SpellChecker_H

#include <nan.h>
#include <mutex>

#include "ZHfstOspeller.h"
#include "ospell.h"

using hfst_ol::ZHfstOspeller;
using hfst_ol::Transducer;

class SpellChecker : public Nan::ObjectWrap {
public:
  static void Init(v8::Local<v8::Object> exports);

private:
  explicit SpellChecker(ZHfstOspeller *speller);
  ~SpellChecker();

  static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);
  static void Suggestions(const Nan::FunctionCallbackInfo<v8::Value> &info);
  static void GetAlphabet(const Nan::FunctionCallbackInfo<v8::Value> &info);
  static void GetLocale(const Nan::FunctionCallbackInfo<v8::Value> &info);
  static void GetLocaleName(const Nan::FunctionCallbackInfo<v8::Value> &info);

  static Nan::Persistent<v8::Function> constructor;
  ZHfstOspeller *speller;
  std::mutex suggestionsMutex;
};

#endif
