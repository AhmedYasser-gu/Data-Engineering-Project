#ifndef CONTOURS
#define CONTOURS
#include <cstdint>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class Contours
{
public:
    cv::Mat makeContours(cv::Mat &img, cv::Scalar color);
};

#endif