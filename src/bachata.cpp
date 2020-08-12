#include "bachata.hpp"
#include <vector>
#include <iostream>

Napi::Object Bachata::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(env, "Bachata", {
        InstanceMethod("loadImage", &Bachata::LoadImage),
        InstanceMethod("origin", &Bachata::Origin),
        InstanceMethod("result", &Bachata::Result),
        InstanceMethod("set", &Bachata::Setter),
        InstanceMethod("setHue", &Bachata::SetHue),
        InstanceMethod("setSaturation", &Bachata::SetSaturation),
        InstanceMethod("setValue", &Bachata::SetValue),
    });

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    // env.SetInstanceData(constructor);
    // constructor->SuppressDestruct();

    exports.Set("Bachata", func);

    return exports;
}

Bachata::Bachata(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Bachata>(info)
{
    // real class constructor
    // must aggregat to parent
    // my class will be lazy loading so no members will be initialized
}

Napi::Value Bachata::LoadImage(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(info.Env(), "expected a string").ThrowAsJavaScriptException();
    }

    std::string path = info[0].As<Napi::String>();
    origin_ = cv::imread(path, cv::IMREAD_COLOR);

    return Napi::Boolean::New(info.Env(), origin_.empty());
}

Napi::Value Bachata::Origin(const Napi::CallbackInfo &info)
{
    std::vector<uint8_t> buffer;
    bool ok = cv::imencode(".png", origin_, buffer, {cv::IMWRITE_PNG_COMPRESSION, 9});
    // std::cout<<"raw c++ buffer size: "<< buffer.size() << '\n';

    Napi::Buffer<uint8_t> js_buff = Napi::Buffer<uint8_t>::New(info.Env(), buffer.size());
    // vector will be out of scope so Napi::Buffer does not retain data
    // we need to manually copy it
    for (size_t i = 0; i < buffer.size(); ++i)
    {
        js_buff[i] = buffer[i];
    }

    return js_buff;
}

Napi::Value Bachata::Result(const Napi::CallbackInfo &info)
{
    // process
    cv::Mat hsv_img;
    cv::cvtColor(origin_, hsv_img, cv::COLOR_BGR2HSV);
    cv::Mat mask;
    cv::inRange(hsv_img, cv::Scalar(hue_l_, sat_l_, val_l_), cv::Scalar(hue_u_, sat_u_, val_u_), mask);
    cv::bitwise_not(mask, mask);
    
    // copy so we have the same sizes
    result_ = origin_;

    // split the channels and create an extra alpha channel from ourr mask
    std::vector<cv::Mat> channels;
    cv::split(result_, channels);
    cv::bitwise_and(channels[1], channels[1], channels[1], mask);
    channels.push_back(mask);
    cv::merge(channels, result_);

    // convert to js type
    std::vector<uint8_t> buffer;
    bool ok = cv::imencode(".png", result_, buffer);
    // bool ok = cv::imencode(".png", mask, buffer);

    Napi::Buffer<uint8_t> js_buff = Napi::Buffer<uint8_t>::New(info.Env(), buffer.size());
    // vector will be out of scope so Napi::Buffer does not retain data
    // we need to manually copy it
    for (size_t i = 0; i < buffer.size(); ++i)
    {
        js_buff[i] = buffer[i];
    }

    // for debugging
    cv::imwrite("/home/michael/Pictures/result.png", mask);

    return js_buff;
}

Napi::Value Bachata::Setter(const Napi::CallbackInfo &info)
{
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    if (info.Length() != 3)
    {
        deferred.Reject(
            Napi::TypeError::New(info.Env(), "require exactly 3 arguments").Value()
        );
    }
    if (!info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber())
    {
        deferred.Reject(
            Napi::TypeError::New(info.Env(), "unmatched types").Value()
        );
    }

    std::string property_name = info[0].As<Napi::String>();
    int lower_bound = info[1].As<Napi::Number>().Int32Value();
    int upper_bound = info[2].As<Napi::Number>().Int32Value();

    if (property_name.compare("hue"))
    {
        hue_l_ = lower_bound;
        hue_u_ = upper_bound;
    }
    else if (property_name.compare("saturation"))
    {
        sat_l_ = lower_bound;
        sat_u_ = upper_bound;
    }
    else if (property_name.compare("value"))
    {
        val_l_ = lower_bound;
        val_u_ = upper_bound;
    }

    Napi::Array arr = Napi::Array::New(info.Env(), 2);
    arr[0u] = lower_bound;
    arr[1u] = upper_bound;
    deferred.Resolve(arr);
    
    return deferred.Promise();
}

Napi::Value Bachata::SetHue(const Napi::CallbackInfo& info)
{
    if (info.Length() < 2 || !(info[0].IsNumber() && info[1].IsNumber()))
    {
        Napi::TypeError::New(info.Env(), "require 2 numbers: lowerbound, upperbound").ThrowAsJavaScriptException();
    }

    int hue_lb = info[0].As<Napi::Number>().Int32Value();
    int hue_ub = info[0].As<Napi::Number>().Int32Value();

    if (hue_lb > 179 || hue_ub > 179 || hue_lb > hue_ub)
    {
        Napi::RangeError::New(info.Env(), "hue value out of range").ThrowAsJavaScriptException();
    }

    hue_l_ = hue_lb;
    hue_u_ = hue_ub;

    // return dummy variable
    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Bachata::SetSaturation(const Napi::CallbackInfo& info)
{
    if (info.Length() < 2 || !(info[0].IsNumber() && info[1].IsNumber()))
    {
        Napi::TypeError::New(info.Env(), "require 2 numbers: lowerbound, upperbound").ThrowAsJavaScriptException();
    }

    int sat_lb = info[0].As<Napi::Number>().Int32Value();
    int sat_ub = info[0].As<Napi::Number>().Int32Value();

    if (sat_lb > 255 || sat_ub > 255 || sat_lb > sat_ub)
    {
        Napi::RangeError::New(info.Env(), "saturation out of range").ThrowAsJavaScriptException();
    }

    sat_l_ = sat_lb;
    sat_u_ = sat_ub;

    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Bachata::SetValue(const Napi::CallbackInfo& info)
{
    if (info.Length() < 2 || !(info[0].IsNumber() && info[1].IsNumber()))
    {
        Napi::TypeError::New(info.Env(), "require 2 numbers: lowerbound, upperbound").ThrowAsJavaScriptException();
    }

    int val_lb = info[0].As<Napi::Number>().Int32Value();
    int val_ub = info[0].As<Napi::Number>().Int32Value();

    if (val_lb > 255 || val_ub > 255 || val_lb > val_ub)
    {
        Napi::RangeError::New(info.Env(), "value out of range").ThrowAsJavaScriptException();
    }

    val_l_ = val_lb;
    val_u_ = val_ub;

    return Napi::Boolean::New(info.Env(), true);
}

// NODE_API_MODULE requires a single entry function
Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
    return Bachata::Init(env, exports);
}

NODE_API_MODULE(bachata, InitAll)
// NODE_API_MODULE(bachata, Bachata::Init)

// I guess it doesn't work with lambdas
// NODE_API_MODULE(bachata, [=](Napi::Env env, Napi::Object exports) mutable { return Bachata::Init(env, exports); })