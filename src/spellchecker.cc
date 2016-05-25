#include "spellchecker.h"
#include <nan.h>

Nan::Persistent<v8::Function> SpellChecker::constructor;

SpellChecker::SpellChecker(ZHfstOspeller *speller) : speller_(speller) {}

SpellChecker::~SpellChecker() {}

void SpellChecker::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("SpellChecker").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "check", Check);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("SpellChecker").ToLocalChecked(), tpl->GetFunction());
}

void SpellChecker::New(const Nan::FunctionCallbackInfo<v8::Value> &info) {
  v8::Isolate *isolate = info.GetIsolate();
  ZHfstOspeller *speller = new ZHfstOspeller();

  if (info.IsConstructCall()) {
    // Called from JS like `new SpellChecker(filename)`
    if (info.Length() < 1) {
      Nan::ThrowTypeError("Wrong number of arguments: Expected one string");
      return;
    }
    if (!info[0]->IsString()) {
      Nan::ThrowTypeError("Argument should be a string");
      return;
    }

    v8::String::Utf8Value filename_v8(info[0]->ToString());
    std::string filename = std::string(*filename_v8);

    // Read dictionary
    try {
      speller->read_zhfst(filename);
    } catch (hfst_ol::ZHfstMetaDataParsingError zhmdpe) {
      std::stringstream error;
      error << "Cannot finish reading zhfst archive `" << filename << "`:\n"
            << zhmdpe.what() << ".\n";
      Nan::ThrowError(v8::String::NewFromUtf8(isolate, error.str().c_str()));
      return;
    } catch (hfst_ol::ZHfstZipReadingError zhzre) {
      std::stringstream error;
      error << "Cannot read zhfst archive `" << filename << "`:\n"
            << zhzre.what() << ".\n";
      Nan::ThrowError(v8::String::NewFromUtf8(isolate, error.str().c_str()));
      return;
    } catch (hfst_ol::ZHfstXmlParsingError zhxpe) {
      std::stringstream error;
      error << "Cannot finish reading index.xml from `" << filename << "`:\n"
            << zhxpe.what() << ".\n";
      Nan::ThrowError(v8::String::NewFromUtf8(isolate, error.str().c_str()));
      return;
    } catch (hfst_ol::OspellException e) {
      std::stringstream error;
      error << "Error opening `" << filename << "`.";
      Nan::ThrowError(v8::String::NewFromUtf8(isolate, error.str().c_str()));
    }

    // Set some options
    //
    // TODO: Set options via arguments (option object)
    speller->set_queue_limit(10);
    // speller.set_weight_limit(-1.0);
    // speller.set_beam(-1.0);
    // speller.set_time_cutoff(0.0);


    SpellChecker *obj = new SpellChecker(speller);


    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());

  } else {
    // Invoked as plain function `SpellChecker(...)`, turn into construct call.
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {info[0]};
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }

}

void SpellChecker::Check(const Nan::FunctionCallbackInfo<v8::Value> &info) {
  // Validate JS calling signature
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments: Expected one string");
    return;
  }
  if (!info[0]->IsString()) {
    Nan::ThrowTypeError("Argument should be a string");
    return;
  }

  v8::String::Utf8Value arg0(info[0]->ToString());
  std::string word = std::string(*arg0);

  v8::Isolate *isolate = info.GetIsolate();
  SpellChecker *obj = Nan::ObjectWrap::Unwrap<SpellChecker>(info.Holder());

  // Check spelling
  if (obj->speller_->spell(word)) {
    // That word actually spelled correctly!
    info.GetReturnValue().Set(v8::Boolean::New(isolate, false));
    return;
  }

  hfst_ol::CorrectionQueue corrections = obj->speller_->suggest(word);

  v8::Handle<v8::Array> correctionsArray =
      v8::Array::New(isolate, corrections.size());

  // Return an empty result if there was an error creating the array.
  if (correctionsArray.IsEmpty()) {
    info.GetReturnValue().Set(v8::Handle<v8::Array>());
    return;
  }

  // Populate the array
  unsigned int i = 0;
  while (corrections.size() > 0) {
    const std::string &corr = corrections.top().first;
    correctionsArray->Set(i, v8::String::NewFromUtf8(isolate, corr.c_str()));
    corrections.pop();
    i++;
  }

  info.GetReturnValue().Set(correctionsArray);
}
