#include "ZHfstOspeller.h"
#include "ospell.h"
#include <mutex>
#include <nan.h>

class SuggestionsWorker : public Nan::AsyncWorker {
public:
  SuggestionsWorker(Nan::Callback *callback, ZHfstOspeller *speller,
                    std::string word, std::mutex *suggestionsMutex)
      : Nan::AsyncWorker(callback), word(word), speller(speller),
        suggestionsMutex(suggestionsMutex) {}
  ~SuggestionsWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures here, so everything we
  // need for input and output should go on `this`.
  void Execute() {
    std::lock_guard<std::mutex> lock(*suggestionsMutex);

    // Check spelling
    correct_word = speller->spell(word);

    if (!correct_word) {
      // Save suggestions
      corrections = speller->suggest(word);
    }
  }

  // Executed when the async work is complete this function will be run inside
  // the main event loop so it is safe to use V8 again
  void HandleOKCallback() {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();

    if (correct_word) {
      v8::Local<v8::Value> argv[] = {Nan::Null(),
                                     v8::Boolean::New(isolate, false)};

      callback->Call(2, argv);
      return;
    }

    v8::Handle<v8::Array> correctionsArray =
        v8::Array::New(isolate, corrections.size());

    // Return an empty result if there was an error creating the array.
    if (correctionsArray.IsEmpty()) {
      v8::Local<v8::Value> argv[] = {Nan::Error("Error creating the array"),
                                     Nan::Null()};

      callback->Call(2, argv);
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

    v8::Local<v8::Value> argv[] = {Nan::Null(), correctionsArray};

    callback->Call(2, argv);
  }

private:
  std::string word;
  ZHfstOspeller *speller;
  hfst_ol::CorrectionQueue corrections;
  bool correct_word;
  std::mutex *suggestionsMutex;
};
