#include "Reducer.hpp"
// Include the GUI and image processing header files from OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <utility>

std::pair<cv::Mat, cv::Mat> Reducer::reduceNoise(cv::Mat &img){
    //!! BLUE VALUES !!********
    // Values for blue hue
    int minHBlue{51};   
    int maxHBlue{145};
    //Values for blue saturation
    int minSBlue{93};
    int maxSBlue{199};

    int minVBlue{42};
    int maxVBlue{214};
    //*************************

    //!!YELLOW VALUES!!********
    // Values for yellow hue
    int minHYellow{15};
    int maxHYellow{42};

    // Values for yellow saturation
    int minSYellow{75};
    int maxSYellow{210};

    int minVYellow{172};
    int maxVYellow{241};
    //*************************

    //Clone the image we're passed
    //cv::Mat reduce = img.clone();
    //Focus on a specific area of interest to (mostly) limit our view to the cones and the track
    cv::Mat splitImg = img(cv::Range(253, 374), cv::Range(0, img.size().width));
    //Declare some Mat images to store some additional image transformations
    cv::Mat imgHSV, imgBlur;
    //Transform the reduced region if interest image into the HSV color space and apply the results to imgB
    cvtColor(splitImg, imgHSV, cv::COLOR_BGR2HSV);

    // Apply a small blur to the image, helps with detection and tracking
    cv::blur(imgHSV, imgBlur, cv::Size(3, 3));

    // From help by: https://stackoverflow.com/questions/23027818/search-multiple-colours-simultaneously-with-opencv-and-c
    // Create two different masks, one for detecting blue and one to detect yellow
    cv::Mat blueMask;
    cv::inRange(imgHSV, cv::Scalar(minHBlue, minSBlue, minVBlue), cv::Scalar(maxHBlue, maxSBlue, maxVBlue), blueMask);

    cv::Mat yellowMask;
    cv::inRange(imgHSV, cv::Scalar(minHYellow, minSYellow, minVYellow), cv::Scalar(maxHYellow, maxSYellow, maxVYellow), yellowMask);


    //Solution adapted from: https://stackoverflow.com/questions/42065405/remove-noise-from-threshold-image-opencv-python
    //!! REDUCE NOIUSE FOR YELLOW MASK!! Doing both separately because we want both yellow and blue color spaces to work with when finding contours,
    //but we also want the combined image to show in the end
    
    //Create a new Mat object to overlay the original for the Opening/Closing
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(4, 4));
    //Closing
    cv::morphologyEx(yellowMask, yellowMask, cv::MORPH_CLOSE, kernel);
    //and OPening, to reduce noise.
    cv::morphologyEx(yellowMask, yellowMask, cv::MORPH_OPEN, kernel);

    //!!REDUCE NOISE FOR BLUE MASK!!
    //Reduce noise for the yellowMask image
    
    //Closing
    cv::morphologyEx(blueMask, blueMask, cv::MORPH_CLOSE, kernel);
    //and Opening, to reduce noise.
    cv::morphologyEx(blueMask, blueMask, cv::MORPH_OPEN, kernel);

    //Setting the values of our masks to be our newly created morphs, which should have reduced noise
    //blueMask = blueMorph;
    //yellowMask = yellowMorph;

    std::pair <cv::Mat, cv::Mat> masks = std::make_pair(blueMask, yellowMask);

    return masks;

}
