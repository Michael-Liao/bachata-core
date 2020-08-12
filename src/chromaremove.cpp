#include "chromaremove.hpp"

void ChromaRemove::loadImage(const std::string &path)
{
    origin_ = cv::imread(path, cv::IMREAD_COLOR);
}

void ChromaRemove::updateHue(const float &hue_lb, const float &hue_hb)
{
    hue_l_ = hue_lb;
    hue_h_ = hue_hb;
}
void ChromaRemove::updateSaturation(const float &sat_lb, const float &sat_hb)
{
    sat_l_ = sat_lb;
    sat_h_ = sat_hb;
}
void ChromaRemove::updateValue(const float &val_lb, const float &val_hb)
{
    val_l_ = val_lb;
    val_h_ = val_hb;
}

cv::Mat ChromaRemove::origin() const
{
    return origin_;
}

cv::Mat ChromaRemove::result()
{
    // algorithm here
    cv::Mat mask;
    cv::Mat hsv_img;
    cv::cvtColor(origin_, hsv_img, cv::COLOR_BGR2HSV);

    int hue_lb = hue_l_ * 179;
    int hue_hb = hue_h_ * 179;
    int sat_lb = sat_l_ * 255;
    int sat_hb = sat_h_ * 255;
    int val_lb = val_l_ * 255;
    int val_hb = val_h_ * 255;

    cv::inRange(hsv_img, cv::Scalar(hue_lb, sat_lb, val_lb), cv::Scalar(hue_hb, sat_hb, val_hb), mask);
    cv::bitwise_not(mask, mask);

    // result_ = origin_.clone();
    // origin_.copyTo(result_);
    result_ = origin_;

    // split the channels and create an extra alpha channel from ourr mask
    std::vector<cv::Mat> channels;
    cv::split(result_, channels);

    channels.push_back(mask);
    cv::merge(channels, result_);

    return result_;
}