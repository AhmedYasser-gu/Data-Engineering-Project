#include "Tracker.hpp"
// Include the GUI and image processing header files from OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Contours.hpp"
#include <tuple>
#include <iostream>
#include <math.h>


std::tuple<cv::Mat, cv::Point2f, cv::Point2f> Tracker::trackObjects(cv::Mat blueMask, cv::Mat yellowMask)
{   
    //The purpose of the contours class has been altered to merely provide a more detailed debug window
    Contours contours;
    //Variable for what color we want to draw with, in this case yellow
    cv::Scalar yellow = cv::Scalar(0, 255, 255, 255);
    //Send the color scalar and the yellowMask to makeContours, it will produce an image where the outline of the yellow cones have been drawn with yellow
    cv::Mat yellowDrawing = contours.makeContours(yellowMask, yellow);
    //Scalar for the color blue, used to draw with
    cv::Scalar blue = cv::Scalar(255, 0, 0, 255);
    //Draw the outline of the blue cones 
    cv::Mat blueDrawing = contours.makeContours(blueMask, blue);
    //Iniializing the color red, for drawing on our centroids. Useful for debugging, to know what in the image we detect as centroids
    cv::Scalar red = cv::Scalar(0, 0, 255, 255);

    cv::Mat debugDrawing = yellowDrawing | blueDrawing;
    // Combine the masks
    cv::Mat mask_combined = blueMask | yellowMask;

    // !! MOMENTS !! Getting contours and filling finding centroids
    // Adapted from oopencv's tutorial/documentation of moments: https://docs.opencv.org/3.4/d0/d49/tutorial_moments.html

    //Vector to store results of findContours, when run on the blueContours Mat image
    std::vector<std::vector<cv::Point>> blueMomentsContours;
    //Vector to store the contour heirarchy. findContours() provides this output to make sense of nested contours in an image
    std::vector<cv::Vec4i> blueMomentsHierarchy;
    
    //Find the contours in our image, put the results in the vectors
    cv::findContours(blueMask, blueMomentsContours, blueMomentsHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    //Create a vector with OpenCV "moments", the size of the vector containing the contours out image
    std::vector<cv::Moments> blueMoments(blueMomentsContours.size());
    //Fill the moments vector with the moments found in each index of the vector containing the contours found in the image
    //I don't know the exact mechanics of cv::moments, but essentially what we are doing is we are unifying all the separate contour
    //"points" we have found using findContours, and unifying them into larger objects based on some logic of clusters of contours being part of the same object
    for(uint i = 0; i < blueMomentsContours.size(); i++){
        blueMoments[i] = cv::moments(blueMomentsContours[i]);
    }
    //A vector containing centroids, which are the centers of the objects we have identified in the image, based on the mean of all the points in the object
    //We want to determine the centroid of the cones because we want to use it's x-y coordinates to determine the angle between cones
    std::vector<cv::Point2f> blueCentroid(blueMomentsContours.size());
    for(uint i = 0; i < blueMomentsContours.size(); i++){
        //Calculating the x and y coordinates of each centroid
        blueCentroid[i] = cv::Point2f( blueMoments[i].m10 / blueMoments[i].m00, blueMoments[i].m01 / blueMoments[i].m00);
        cv::circle(debugDrawing, blueCentroid[i], 4, red, -1);

    }
    
    //Finding thre momemts for the yellow cones. We do this in the same way as the blue cones, so this segment of code will be less thoroughly commented to avoud repetition
    std::vector<std::vector<cv::Point>> yellowMomentsContours;
    std::vector<cv::Vec4i> yellowMomentsHierarchy;
    //Call findContours() on the image with the isolated yellow cones
    cv::findContours(yellowMask, yellowMomentsContours, yellowMomentsHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    //Vector for moments
    std::vector<cv::Moments> yellowMoments(yellowMomentsContours.size());
    //For each contour, find the monents
    for(uint i = 0; i < yellowMomentsContours.size(); i++){
        yellowMoments[i] = cv::moments(yellowMomentsContours[i]);
    }
    //Find the centroids of the yellow cones.
    std::vector<cv::Point2f> yellowCentroid(yellowMomentsContours.size());
    for(uint i = 0; i < yellowMomentsContours.size(); i++){
        yellowCentroid[i] = cv::Point2f( yellowMoments[i].m10 / yellowMoments[i].m00, yellowMoments[i].m01 / yellowMoments[i].m00);
        cv::circle(debugDrawing, yellowCentroid[i], 4, red, -1);
    }
    //Next, we ideally send the two closest individual cones on one side of the car to the Angler class to get the angle between them
    cv::Point2f pointA,
                 pointB;
    //If we have more blue centroids than yellow ones, it follows that we have detected more blue cones in the image.
    //This implies that in the frame we are currently dealing with, the blue cones are of greater importance than the yellow ones, as the course is probably turning in such a way
    //that the blue cones are taking up more of the image. If the two vectors are equal in size we default to getting the angle of the blue cones

    if(blueCentroid.size() >= yellowCentroid.size()){
    //The centroid is a set of x and y coordinates, we naively choose the centroids in the first and second index of the array.

        //At some points the vector with the centroids will not have a second or third index position, and trying to use them will cause a crash
        if(blueCentroid.size() < 2) {
            //If this is the case we just give placeholder Points with coordinate values x = 0, y = 0
            pointA = cv::Point2f(0.0f,0.0f);
            pointB = cv::Point2f(0.0f,0.0f);
        } else {
            //If the vector has something at array index 2, we use the points at index position 1 and 2
            pointA = blueCentroid[0];
            pointB = blueCentroid[2];
        } 
        
    }
    //If there are more yellow centroids the same logic holds, that we should be dealing with the yellow cones, as they exist in greater numbers in the current frame
    else if(blueCentroid.size() < yellowCentroid.size()){
        //We employ the same logic to avoid errors like with the blue centroids
        if(yellowCentroid.size() < 2) {
            pointA = cv::Point2f(0.0f,0.0f);
            pointB = cv::Point2f(0.0f,0.0f);
        } else {
            pointA = yellowCentroid[0];
            pointB = yellowCentroid[2];
        } 

    }

    //Make a tuple so we can return several values not of the same type
    std::tuple<cv::Mat, cv::Point2f, cv::Point2f> cones = std::make_tuple(debugDrawing, pointA, pointB);

    return cones;
}