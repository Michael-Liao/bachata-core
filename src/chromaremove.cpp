#include "chromaremove.hpp"

void ChromaRemove::loadImage(const std::string& path) {
  origin_ = cv::imread(path, cv::IMREAD_COLOR);
}

void ChromaRemove::setHue(const int& hue_lb, const int& hue_ub) {
  hue_l_ = hue_lb;
  hue_u_ = hue_ub;
}
void ChromaRemove::setSaturation(const int& sat_lb, const int& sat_ub) {
  sat_l_ = sat_lb;
  sat_u_ = sat_ub;
}
void ChromaRemove::setValue(const int& val_lb, const int& val_ub) {
  val_l_ = val_lb;
  val_u_ = val_ub;
}

cv::Mat ChromaRemove::origin() const {
  return origin_;
}

cv::Mat ChromaRemove::result() const {
  return result_;
}

void ChromaRemove::apply() {
  // algorithm here
  cv::Mat mask;
  cv::Mat hsv_img;
  cv::cvtColor(origin_, hsv_img, cv::COLOR_BGR2HSV);

  cv::inRange(hsv_img, cv::Scalar(hue_l_, sat_l_, val_l_),
              cv::Scalar(hue_u_, sat_u_, val_u_), mask);
  cv::bitwise_not(mask, mask);

  // result_ = origin_.clone();
  // origin_.copyTo(result_);
  // result_ = origin_;

  // split the channels and create an extra alpha channel from our mask
  std::vector<cv::Mat> channels;
  cv::split(origin_, channels);

  channels.push_back(mask);
  cv::merge(channels, result_);
}