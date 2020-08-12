#if !defined(CHROMAREMOVE_H)
#define CHROMAREMOVE_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

class ChromaRemove
{
public:
    ChromaRemove() = default;
    ~ChromaRemove() = default;

    void loadImage(const std::string &path);

    void setHue(const int &hue_lb, const int &hue_hb);
    void setSaturation(const int &sat_lb, const int &sat_hb);
    void setValue(const int &val_lb, const int &val_hb);

    cv::Mat origin() const;
    cv::Mat result();
    // shit we might need to return an array (or tuple but tuple is ugly)
    // const int hue() const;
    // const int saturation() const;
    // const int value() const;

private:
    cv::Mat origin_;
    cv::Mat result_;

    // opencv ranges
    // hue: 0 - 179
    // saturation: 0 - 255
    // value: 0 - 255

    int hue_l_ = 52;
    int hue_u_ = 68;

    int sat_l_ = 200;
    int sat_u_ = 255;

    int val_l_ = 127;
    int val_u_ = 255;
};

#endif