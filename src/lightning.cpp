/*
 * Copyright (C) 2020  Christian Berger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Include the single-file, header-only middleware libcluon to create high-performance microservices
#include "cluon-complete.hpp"
// Include the OpenDLV Standard Message Set that contains messages that are usually exchanged for automotive or robotic applications
#include "opendlv-standard-message-set.hpp"

// Include the GUI and image processing header files from OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ctime>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <utility>
#include <tuple>
#include <math.h>

// Include classes
#include "Tracker.hpp"
#include "Printer.hpp"
#include "Reducer.hpp"
#include "Angler.hpp"
#include "GsrTester.hpp"

int32_t main(int32_t argc, char **argv)
{
    int32_t retCode{1};
    // Parse the command line parameters as we require the user to specify some mandatory information on startup.
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ((0 == commandlineArguments.count("cid")) ||
        (0 == commandlineArguments.count("name")) ||
        (0 == commandlineArguments.count("width")) ||
        (0 == commandlineArguments.count("height")))
    {
        std::cerr << argv[0] << " attaches to a shared memory area containing an ARGB image." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --cid=<OD4 session> --name=<name of shared memory area> [--verbose]" << std::endl;
        std::cerr << "         --cid:    CID of the OD4Session to send and receive messages" << std::endl;
        std::cerr << "         --name:   name of the shared memory area to attach" << std::endl;
        std::cerr << "         --width:  width of the frame" << std::endl;
        std::cerr << "         --height: height of the frame" << std::endl;
        std::cerr << "Example: " << argv[0] << " --cid=253 --name=img --width=640 --height=480 --verbose" << std::endl;
    }
    else
    {
        // Extract the values from the command line parameters
        const std::string NAME{commandlineArguments["name"]};
        const uint32_t WIDTH{static_cast<uint32_t>(std::stoi(commandlineArguments["width"]))};
        const uint32_t HEIGHT{static_cast<uint32_t>(std::stoi(commandlineArguments["height"]))};

        const bool VERBOSE{commandlineArguments.count("verbose") != 0};
        const bool RUN_TEST{commandlineArguments.count("gsrtest") != 0};

        // Attach to the shared memory.
        std::unique_ptr<cluon::SharedMemory> sharedMemory{new cluon::SharedMemory{NAME}};
        std::unique_ptr<cluon::SharedMemory> sharedMemoryWrite{new cluon::SharedMemory{"/"}};
        if (sharedMemory && sharedMemory->valid())
        {
            std::clog << argv[0] << ": Attached to shared memory '" << sharedMemory->name() << " (" << sharedMemory->size() << " bytes)." << std::endl;

            // Interface to a running OpenDaVINCI session where network messages are exchanged.
            // The instance od4 allows you to send and receive messages.
            cluon::OD4Session od4{static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))};

            opendlv::proxy::GroundSteeringRequest gsr;
            std::mutex gsrMutex;
            auto onGroundSteeringRequest = [&gsr, &gsrMutex](cluon::data::Envelope &&env)
            {
                // The envelope data structure provide further details, such as sampleTimePoint as shown in this test case:
                // https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestEnvelopeConverter.cpp#L31-L40
                std::lock_guard<std::mutex> lck(gsrMutex);
                gsr = cluon::extractMessage<opendlv::proxy::GroundSteeringRequest>(std::move(env));
            };

            od4.dataTrigger(opendlv::proxy::GroundSteeringRequest::ID(), onGroundSteeringRequest);

            // For testing perfomance
            int frame = 0;
            int correctFrames = 0;
            GsrTester gsrTester;

            // Create printer object to store as csv
            Printer printer;
            // Path to store csv file
            std::string writePath = sharedMemoryWrite->name() + "/lightning.csv";
            // Clear the file of potential old data
            printer.clearFile(writePath);
            //Writing the first row of our csv file outside the while loop.
            std::string firstRow = "groundSteering";
            printer.writeToFile(writePath, firstRow);

            // Create new tracker object
            Tracker tracker;
            // Create new reducer object
            Reducer reducer;
            // Create new angler object
            Angler angler;

            // initialize Mat object that will be our color separated frame
            cv::Mat mask_combined;

            // Endless loop; end the program by pressing Ctrl-C.
            while (od4.isRunning())
            {
                // Create an OpenCV image header using the data in the shared memory.
                cv::Mat img;

                // Wait for frame
                sharedMemory->wait();

                sharedMemory->lock();
                {
                    cv::Mat wrapped(HEIGHT, WIDTH, CV_8UC4, sharedMemory->data());
                    img = wrapped.clone();
                }
                //WHile the shared memory is locked, get the sampleTime timestamp using the prodived cluon methods
                std::pair<bool, cluon::data::TimeStamp> timeStamp = sharedMemory->getTimeStamp();
                int64_t sampleTime = cluon::time::toMicroseconds(timeStamp.second);

                sharedMemory->unlock();
                //We send the current frame to the "reducer", which reduces the nouse in the image, and returns a pair of images,
                //one of which is the isolated yellow color space and the other the isolated blue color space
                std::pair<cv::Mat, cv::Mat> masks = reducer.reduceNoise(img);
                //Accessing the color "masks" out of the pair.
                cv::Mat blueMaskReduced = masks.first;
                cv::Mat yellowMaskReduced = masks.second;
                
                //Next we send the color space masks to the tracker, which using Opencv Moments and findContours draws a centroid on every mass of contours it detects.
                //It returns an image which is the two color-space masks combined and the Point objects for two of the centroids the tracker found. We want the Points because they contain x-y cordinates.
                std::tuple<cv::Mat, cv::Point2f, cv::Point2f> trackedCones = tracker.trackObjects(blueMaskReduced, yellowMaskReduced);
                //Accessing the returned image, and Points from the tuple.
                cv::Mat final_img = std::get<0>(trackedCones);
                cv::Point2f pointA = std::get<1>(trackedCones);
                cv::Point2f pointB = std::get<2>(trackedCones);
                //We send the Points to the Angler, which does some trigonometry to figure out the angle between the two Points, baes on their x-y coordinates
                const float conesAngle = angler.getAngle(pointA, pointB);
                //Using the x-y coordinates we also determine if we are in a left/right turn, and is GSR should be negative or positive. Somewhat unreliable, depending on if the Points were chosen appropriately.
                int turn_dir = angler.leftRightDeterminer(pointA, pointB);
                //Turn the angle of the cones, and the turn direction into an appropriate GSR.
                float ourGSR = angler.angleToGSR(conesAngle, turn_dir);

                if(isnan(ourGSR)){
                    ourGSR = 0;
                }

                //Print the gsr for the corresponding moment in time in microseconds
                std::cout << "group_11;" << sampleTime << ";" << ourGSR << std::endl;

                if(RUN_TEST) {
                    //In-terminal testing of the acceptance percentage of the GSR.
                    correctFrames = correctFrames + gsrTester.testGSR(ourGSR, gsr.groundSteering());
                    frame = frame + 1;
                    const float correctPercentage = (float)correctFrames / (float)frame;
                    
                    std::cout << "Accepted GSR: " << correctPercentage * 100 << "%" << std::endl;
                }

                // Printer writes to file
                printer.writeToFile(writePath, ourGSR);

                if (VERBOSE)
                {
                    cv::imshow("Debug Window", final_img);
                    //cv::imshow(sharedMemory->name().c_str(), img);
                    cv::waitKey(1);
                }
            }

            if(RUN_TEST) {
                std::cout << "DONE RUNNING - NOW RUNNING TEST" << std::endl;

                float correctPercentage = (float)correctFrames / (float)frame;

                std::cout << "Frames total: " << frame << std::endl;
                std::cout << "Frames correct: " << correctFrames << std::endl;

                std::cout << "Percentage correct: " << correctPercentage * 100 << "%" << std::endl;
            }

        }
        retCode = 0;
    }
    return retCode;
}


