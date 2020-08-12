#if !defined(CHROMAREMOVE_H)
#define CHROMAREMOVE_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

class ChromaRemove
{
    cv::Mat origin_;
    cv::Mat result_;

    // opencv ranges
    // hue: 0 - 179
    // saturation: 0 - 255
    // value: 0 - 255

    float hue_l_ = 0.29; // 0 - 179
    float hue_h_ = 0.38; // 0 - 179

    float sat_l_ = 0.8;
    float sat_h_ = 1;

    float val_l_ = 0.5;
    float val_h_ = 1;

public:
    ChromaRemove() = default;
    ~ChromaRemove() = default;

    void loadImage(const std::string &path);

    void updateHue(const float &hue_lb, const float &hue_hb);
    void updateSaturation(const float &sat_lb, const float &sat_hb);
    void updateValue(const float &val_lb, const float &val_hb);

    cv::Mat origin() const;
    cv::Mat result();
};

#endif