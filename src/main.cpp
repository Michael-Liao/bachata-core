#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <iostream>

#include "chromaremove.hpp"

int main(int argc, char const *argv[])
{
    ChromaRemove cr;
    cr.loadImage("/home/michael/Pictures/green-screen-backgrounds-1024x687.png");
    // cv::namedWindow("show image");
    // cv::imshow("show image", cr.origin());
    cv::Mat result = cr.result();
    cv::imwrite("/home/michael/Pictures/mask.png", result);
    std::vector<uint8_t> buffer;
    bool ok = cv::imencode(".png", result, buffer);
    if (ok)
    {
        std::cout<< "decode success" <<'\n';
    }

    return 0;
}
