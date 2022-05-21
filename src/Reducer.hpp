#ifndef REDUCER
#define REDUCER
#include <cstdint>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
class Reducer
{
public:
    std::pair<cv::Mat, cv::Mat> reduceNoise(cv::Mat &img);

};
#endif