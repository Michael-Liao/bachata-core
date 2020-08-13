#include "bachata.hpp"

#include <iostream>
#include <vector>

// perfect demonstration of promises
// https://github.com/nodejs/node-addon-examples/issues/85#issuecomment-583887294
class ImageAsyncProcessor : public Napi::AsyncWorker {
 public:
  ImageAsyncProcessor(Napi::Env env, ChromaRemove& cr)
      : Napi::AsyncWorker(env),
        remover_(cr),
        deferred_(Napi::Promise::Deferred::New(env)) {}
  ~ImageAsyncProcessor() {}

  Napi::Promise GetPromise() const { return deferred_.Promise(); }

 protected:
  void Execute() override {
    // long running algorithm
    remover_.apply();
  }
  void OnOK() override {
    // encode the cv::Mat into a raw buffer
    std::vector<uint8_t> buffer;
    cv::imencode(".png", remover_.result(), buffer);

    // transfer the data to a Node.js Buffer Object
    Napi::Buffer<uint8_t> js_buff =
        Napi::Buffer<uint8_t>::New(Env(), buffer.size());
    // vector will be out of scope so Napi::Buffer does not retain data
    // we need to manually copy it
    for (size_t i = 0; i < buffer.size(); ++i) {
      js_buff[i] = buffer[i];
    }

    deferred_.Resolve(js_buff);
  }
  void OnError(const Napi::Error& error) override {
    deferred_.Reject(error.Value());
  }

 private:
  ChromaRemove& remover_;
  Napi::Promise::Deferred deferred_;
};

Napi::Object Bachata::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
      DefineClass(env, "Bachata",
                  {
                      InstanceMethod("loadImage", &Bachata::LoadImage),
                      InstanceMethod("origin", &Bachata::Origin),
                      InstanceMethod("result", &Bachata::Result),
                      InstanceMethod("resultAsync", &Bachata::ResultAsync),
                      // InstanceMethod("set", &Bachata::Setter),
                      InstanceMethod("setHue", &Bachata::SetHue),
                      InstanceMethod("setSaturation", &Bachata::SetSaturation),
                      InstanceMethod("setValue", &Bachata::SetValue),
                  });

  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  // env.SetInstanceData(constructor);
  // constructor->SuppressDestruct();

  exports.Set("Bachata", func);

  return exports;
}

Bachata::Bachata(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Bachata>(info) {
  // real class constructor
  // must aggregate to parent
}

Napi::Value Bachata::LoadImage(const Napi::CallbackInfo& info) {
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(info.Env(), "expected a string")
        .ThrowAsJavaScriptException();
  }

  std::string path = info[0].As<Napi::String>();
  remover_.loadImage(path);

  return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Bachata::Origin(const Napi::CallbackInfo& info) {
  cv::Mat original_img = remover_.origin();

  std::vector<uint8_t> buffer;
  bool ok = cv::imencode(".png", original_img, buffer,
                         {cv::IMWRITE_PNG_COMPRESSION, 9});
  // std::cout<<"raw c++ buffer size: "<< buffer.size() << '\n';

  Napi::Buffer<uint8_t> js_buff =
      Napi::Buffer<uint8_t>::New(info.Env(), buffer.size());
  // vector will be out of scope so Napi::Buffer does not retain data
  // we need to manually copy it
  for (size_t i = 0; i < buffer.size(); ++i) {
    js_buff[i] = buffer[i];
  }

  return js_buff;
}

Napi::Value Bachata::Result(const Napi::CallbackInfo& info) {
  // process then get result
  remover_.apply();
  cv::Mat result_img = remover_.result();

  // convert to js type
  std::vector<uint8_t> buffer;
  bool ok = cv::imencode(".png", result_img, buffer,
                         {cv::IMWRITE_PNG_COMPRESSION, 9});
  // bool ok = cv::imencode(".png", mask, buffer);

  Napi::Buffer<uint8_t> js_buff =
      Napi::Buffer<uint8_t>::New(info.Env(), buffer.size());
  // vector will be out of scope so Napi::Buffer does not retain data
  // we need to manually copy it
  for (size_t i = 0; i < buffer.size(); ++i) {
    js_buff[i] = buffer[i];
  }

  // for debugging
  // cv::imwrite("/home/michael/Pictures/result.png", mask);

  return js_buff;
}

Napi::Value Bachata::ResultAsync(const Napi::CallbackInfo& info) {
  ImageAsyncProcessor* processor =
      new ImageAsyncProcessor(info.Env(), remover_);
  Napi::Promise promise = processor->GetPromise();

  // start the worker (fork)
  processor->Queue();

  return promise;
}

Napi::Value Bachata::SetHue(const Napi::CallbackInfo& info) {
  if (info.Length() < 2 || !(info[0].IsNumber() && info[1].IsNumber())) {
    Napi::TypeError::New(info.Env(),
                         "require 2 numbers: lowerbound, upperbound")
        .ThrowAsJavaScriptException();
  }

  int hue_lb = info[0].As<Napi::Number>().Int32Value();
  int hue_ub = info[1].As<Napi::Number>().Int32Value();

  if (hue_lb > 179 || hue_ub > 179 || hue_lb > hue_ub) {
    Napi::RangeError::New(info.Env(), "hue value out of range")
        .ThrowAsJavaScriptException();
  }

  // hue_l_ = hue_lb;
  // hue_u_ = hue_ub;
  remover_.setHue(hue_lb, hue_ub);

  // std::cout<<"hue boundaries: ["<< hue_l_ <<", "<< hue_u_ <<"]\n";

  // return dummy variable
  return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Bachata::SetSaturation(const Napi::CallbackInfo& info) {
  if (info.Length() < 2 || !(info[0].IsNumber() && info[1].IsNumber())) {
    Napi::TypeError::New(info.Env(),
                         "require 2 numbers: lowerbound, upperbound")
        .ThrowAsJavaScriptException();
  }

  int sat_lb = info[0].As<Napi::Number>().Int32Value();
  int sat_ub = info[1].As<Napi::Number>().Int32Value();

  if (sat_lb > 255 || sat_ub > 255 || sat_lb > sat_ub) {
    Napi::RangeError::New(info.Env(), "saturation out of range")
        .ThrowAsJavaScriptException();
  }

  // sat_l_ = sat_lb;
  // sat_u_ = sat_ub;
  remover_.setSaturation(sat_lb, sat_ub);

  return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Bachata::SetValue(const Napi::CallbackInfo& info) {
  if (info.Length() < 2 || !(info[0].IsNumber() && info[1].IsNumber())) {
    Napi::TypeError::New(info.Env(),
                         "require 2 numbers: lowerbound, upperbound")
        .ThrowAsJavaScriptException();
  }

  int val_lb = info[0].As<Napi::Number>().Int32Value();
  int val_ub = info[1].As<Napi::Number>().Int32Value();

  if (val_lb > 255 || val_ub > 255 || val_lb > val_ub) {
    Napi::RangeError::New(info.Env(), "value out of range")
        .ThrowAsJavaScriptException();
  }

  // val_l_ = val_lb;
  // val_u_ = val_ub;
  remover_.setValue(val_lb, val_ub);

  return Napi::Boolean::New(info.Env(), true);
}

// NODE_API_MODULE requires a single entry function
Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return Bachata::Init(env, exports);
}

NODE_API_MODULE(bachata, InitAll)
// NODE_API_MODULE(bachata, Bachata::Init)

// I guess it doesn't work with lambdas
// NODE_API_MODULE(bachata, [=](Napi::Env env, Napi::Object exports) mutable {
// return Bachata::Init(env, exports); })