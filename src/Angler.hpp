#ifndef ANGLER
#define ANGLER
#include <math.h>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Angler {
    public: 
        float getAngle(cv::Point2f pointA, cv::Point2f pointB);
        float angleToGSR(float angle, int turn_dir);
        float constantCalculator(float angle, float GSR);
        int leftRightDeterminer(cv::Point2f pointA, cv::Point2f pointB);
};

#endif