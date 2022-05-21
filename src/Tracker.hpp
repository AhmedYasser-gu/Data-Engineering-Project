#ifndef TRACKER
#define TRACKER
#include <cstdint>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
class Tracker {
public:
    std::tuple<cv::Mat, cv::Point2f, cv::Point2f> trackObjects(cv::Mat blueMask, cv::Mat yellowMask);
};
#endif
