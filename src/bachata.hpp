#if !defined(BACHATA_H)
#define BACHATA_H
// #define NAPI_EXPERIMENTAL
// #ifdef CMAKE_JS_INC
#include <napi.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "chromaremove.hpp"
// #endif

// Napi::Addon is still not out yet, we probably need to wait for a few days
class Bachata : public Napi::ObjectWrap<Bachata> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  Bachata(const Napi::CallbackInfo& exports);
  // ~Bachata();

 private:
  // data
  ChromaRemove remover_;

  // all the functions
  Napi::Value LoadImage(const Napi::CallbackInfo& info);
  Napi::Value Origin(const Napi::CallbackInfo& info);
  Napi::Value Result(const Napi::CallbackInfo& info);
  Napi::Value ResultAsync(const Napi::CallbackInfo& info);

  // New async setter
  // Napi::Value Setter(const Napi::CallbackInfo &info);
  Napi::Value SetHue(const Napi::CallbackInfo& info);
  Napi::Value SetSaturation(const Napi::CallbackInfo& info);
  Napi::Value SetValue(const Napi::CallbackInfo& info);
};

// NODE_API_ADDON(Bachata)
// NODE_API_MODULE(bachata, Bachata::Init)

#endif  // BACHATA_H