#include "Angler.hpp"
// Include the GUI and image processing header files from OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <math.h>
#include <vector>
#include <iostream>


float Angler::getAngle(cv::Point2f pointA, cv::Point2f pointB) {
            // !! Code adapted from : https://stackoverflow.com/questions/15888180/calculating-the-angle-between-points !!

            //Find the lenfth of two vectors to each of the points.
            float len1 = sqrt(pointA.x * pointA.x + pointA.y * pointA.y);
            float len2 = sqrt(pointB.x * pointB.x + pointB.y * pointB.y);
            //Get the dot product of our two points 
            float dot = pointA.x * pointB.x + pointA.y * pointB.y;
            //Calculate the cosine. The cosine is the quotient of the product and the length of the vectors
            float angle = dot / (len1 * len2);

            //Cos angle can't be above or below 1
            if(angle > 1.0){
                return 0.0;
            }
            //Cos angle can't be above or below 1
            else if(angle < -1.0){
                return M_PI;
            }
            //Convert radians into degrees
            else{
                return std::acos(angle) * 180 / M_PI;
            }
        }

float Angler::angleToGSR(float angle, int turn_dir) {
    // Angle multiplied with some constant results in the GSR
    //const float ANGLE_TO_GSR_CONSTANT = -0.18417424228333337
    //-0.003232 -> 44.5153%
    //-0.0029088 -> 45.5471%
    //-0.002644364 -> 46.8112%
    const float ANGLE_TO_GSR_CONSTANT = -0.002237538; 
    float GSR = angle * ANGLE_TO_GSR_CONSTANT;
    //If turn_dir is 0 we are in a right turn
    
    /* !! Some print statements for angle and direction, if you want them for testing !!
    std::cout << "Direction: " << turn_dir << std::endl;
    std::cout << "Angle: " << angle << std::endl;
    // !! ***************************** !! */

    if((turn_dir == 1)){
        //If we are turning left the GSR needs to be positive, so we take the absolute value
        GSR = fabsf(GSR);
    }
    //Here we make sure we are never sending a gsr that is above the max gsr of the original recordings or less then the minimum.
    if(GSR > 0.290888) {
        return 0.290888;
    } else if (GSR < -0.290888) {
        return -0.290888;
    } else {
        return GSR;
    }
}

float Angler::constantCalculator(float angle, float GSR) {

    // GSR divided by angle results in constant needed to caluclate GSR from angle

    if(angle == 0.0f) {
        return 0;
    } 

    return (GSR / angle);
}

int Angler::leftRightDeterminer(cv::Point2f pointA, cv::Point2f pointB) {
    //If the x-value of pointA is < the x-value of pointB then it is a right turn, and GSR should be negative.
    //If the opposite is true, then it is a left tuen and GSR should be positive
    int turn_dir;

    if(pointA.x <= pointB.x) {
        //We represent a right turn with a turn_dir value of 0
        turn_dir = 0;
    }
    else{
        //We represent a left turn with a turn_dir value of 1
        turn_dir = 1;
    }

    return (turn_dir);
}