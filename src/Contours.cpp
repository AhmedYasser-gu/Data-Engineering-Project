#include "Contours.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>



/**
 * It takes an image, a color, and a threshold value, and returns an image with the contours of the
 * blobs in the original image drawn in the specified color
 * 
 * @param img The image to be processed
 * @param color The color of the contours.
 * @param thresh The threshold value for the Canny edge detector.
 * 
 * @return A cv::Mat object.
 */
cv::Mat Contours::makeContours (cv::Mat &img, cv::Scalar color){
    cv::Mat output;
    const int thresh = 100;


    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    
    cv::Canny(img, output, thresh, thresh*2);
    cv::findContours(output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    cv::Mat drawing = cv::Mat::zeros(img.size(), CV_8UC3);
    for(uint i = 0; i < contours.size(); i++){
        cv::drawContours(drawing, contours, (int)i, color, 3);
    }
    return drawing;
}
