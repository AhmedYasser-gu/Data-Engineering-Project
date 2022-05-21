#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../Angler.hpp"
#include "../Printer.hpp"
#include "../Reducer.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <utility>



TEST_CASE("Test angle math"){
    Angler angler;

    cv::Point2f pointA;
    cv::Point2f pointB;

    pointA.x = 20.0f;
    pointA.y = 40.0f;

    pointB.x = 40.0f;
    pointB.y = 70.0f;

    float result = angler.getAngle(pointA, pointB);


    REQUIRE(result != 0.0f);
}

TEST_CASE("Test, not impossible angle"){
    Angler angler;

    cv::Point2f pointA;
    cv::Point2f pointB;

    pointA.x = 20.0f;
    pointA.y = 40.0f;

    pointB.x = 40.0f;
    pointB.y = 70.0f;

    float result = angler.getAngle(pointA, pointB);

    REQUIRE(result < 200.0f);
}

TEST_CASE("Correct direction"){
    Angler angler;

    cv::Point2f pointA;
    cv::Point2f pointB;

    pointA.x = 20.0f;
    pointA.y = 40.0f;

    pointB.x = 40.0f;
    pointB.y = 70.0f;

    REQUIRE(angler.leftRightDeterminer(pointA, pointB) == 0);
}

TEST_CASE("Run through reducer with 'zeroed' Mat object"){
    cv::Mat zero_img = cv::Mat::zeros(480, 640, CV_8UC4);
    bool pass;

    Reducer reducer;

    std::pair<cv::Mat, cv::Mat> ret = reducer.reduceNoise(zero_img);

    if(cv::countNonZero(ret.first) == 0){
        pass = true;
    }
    else{
        pass = false;
    }

    REQUIRE(pass == true);
}
