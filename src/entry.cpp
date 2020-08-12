#include <iostream>
#include <napi.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
// #include <opencv2/videoio.hpp>

#include "chromaremove.hpp"
// #include "bachata.hpp"

/**
 * Broken, memory is not persistant.
 * might have to write a full class
 * https://github.com/nodejs/node-addon-api/blob/master/doc/addon.md
 */

static ChromaRemove cr;

void LoadImage(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "expected a string").ThrowAsJavaScriptException();
  }

  // Napi::String path = info[0].As<Napi::String>();
  cr.loadImage(info[0].As<Napi::String>());
}

void SetHue(const Napi::CallbackInfo &info)
{
  if (info.Length() != 2)
  {
    Napi::TypeError::New(info.Env(), "requires 2 parameters").ThrowAsJavaScriptException();
  }

  float hue_lb = info[0].As<Napi::Number>().FloatValue() / 179;
  float hue_ub = info[1].As<Napi::Number>().FloatValue() / 179;

  cr.updateHue(hue_lb, hue_ub);
}

void SetSaturation(const Napi::CallbackInfo &info)
{
  if (info.Length() != 2)
  {
    Napi::TypeError::New(info.Env(), "requires 2 parameters").ThrowAsJavaScriptException();
  }

  float sat_lb = info[0].As<Napi::Number>().FloatValue() / 255;
  float sat_ub = info[1].As<Napi::Number>().FloatValue() / 255;

  cr.updateHue(sat_lb, sat_ub);
}

void SetValue(const Napi::CallbackInfo &info)
{
  if (info.Length() != 2)
  {
    Napi::TypeError::New(info.Env(), "requires 2 parameters").ThrowAsJavaScriptException();
  }

  float val_lb = info[0].As<Napi::Number>().FloatValue() / 255;
  float val_ub = info[1].As<Napi::Number>().FloatValue() / 255;

  cr.updateHue(val_lb, val_ub);
}

Napi::Buffer<uint8_t> Origin(const Napi::CallbackInfo &info)
{
  cv::Mat original_img = cr.origin();

  std::vector<uint8_t> buffer;
  bool ok = cv::imencode(".png", original_img, buffer);

  return Napi::Buffer<uint8_t>::New(info.Env(), buffer.data(), buffer.size());
}

Napi::Buffer<uint8_t> Result(const Napi::CallbackInfo &info)
{
  cv::Mat result_img = cr.result();
  std::vector<uint8_t> buffer;
  // Napi::TypedArrayOf<uint8_t> buffer;
  bool ok = cv::imencode(".png", result_img, buffer);
  /// @todo imencode returns bool, use that to throw js error

  // Napi::Buffer<uint8_t> uint8_arr = Napi::Buffer<uint8_t>::New(info.Env(), buffer.data(), buffer.size());

  return Napi::Buffer<uint8_t>::New(info.Env(), buffer.data(), buffer.size());
  ;
}

static Napi::String Method(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::String::New(env, "Hello, world!");
}

static Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "hello"), Napi::Function::New(env, Method));

  /// @note Napi::Object::Set actually can take c-type strings
  /// the example is an overkill
  // register function loadImage
  exports.Set("loadImage", Napi::Function::New(env, LoadImage));
  // register value setters
  exports.Set("setHue", Napi::Function::New(env, SetHue));
  exports.Set("setSaturation", Napi::Function::New(env, SetSaturation));
  exports.Set("setValue", Napi::Function::New(env, SetValue));
  // getters
  exports.Set("origin", Napi::Function::New(env, Origin));
  exports.Set("result", Napi::Function::New(env, Result));

  return exports;
}

NODE_API_MODULE(silhouette, Init)