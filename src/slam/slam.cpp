#include <atomic>
#include <csignal>
#include <iostream>
#include <map>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>

#include "depthai/depthai.hpp"

#include "slam.hpp"
#include "feature_matching_and_detection.h"

SLAM::SLAM() {}


/* Initialize nodes from camera */
void SLAM::init() {

    // Create device and pipeline
    device = std::make_shared<dai::Device>();
    pipeline = std::make_unique<dai::Pipeline>(device);

    // CAMERA (https://docs.luxonis.com/software-v3/depthai/depthai-components/nodes/camera/)
    cam = pipeline->create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_A, type=dai.ImgFrame.Type.NV12, fps=30);
    mono_left = pipeline->create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_B);
    mono_right = pipeline->create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_C);

    cam_out =  cam->requestOutput({640, 480}, dai::ImgFrame::Type::NV12, dai::ImgResizeMode::CROP, 2);
    mono_left_out = mono_left->requestOutput({640, 480});
    mono_right_out = mono_right->requestOutput({640, 480});

    // Apply some manipulation to camera stream
    //  - correct for wide lens distrotion
    //  - correct orientation (flip?)
    //  - correct color channel (rgb)
    // cam_out->setNumFramesPools(2);

    // IMU (https://docs.luxonis.com/software-v3/depthai/depthai-components/nodes/imu/)

    imu = pipeline.create<dai::node::IMU>();
    // enable ACCELEROMETER_RAW and GYROSCOPE_RAW at 100 hz rate
    imu->enableIMUSensor({dai::IMUSensor::ACCELEROMETER_RAW, dai::IMUSensor::GYROSCOPE_RAW}, 100);
    imu->setBatchReportThreshold(1);


    // STEREO DEPTH (https://docs.luxonis.com/software-v3/depthai/depthai-components/nodes/stereo_depth)
    stereo = pipeline->create<dai::node::StereoDepth>()->build(
        *monoLeftOut, *monoRightOut, dai::node::StereoDepth::PresetMode::ROBOTICS);
    
    // apply some configurations for stereo to impove accuracy
    stereo->setLeftRightCheck(true);    // Better handling for occlusions:
    stereo->setSubpixel(true);          // Better accuracy for longer distance:
    stereo->setDepthAlign(dai::CameraBoardSocket::CAM_A);  // depth pixels line up with rgb


    // Define output queues to read from
    cam_queue = cam_out->createOutputQueue(2, false);
    mono_left_queue  = mono_left_out->createOutputQueue(2, false);
    mono_right_queue = mono_right_out->createOutputQueue(2, false);
    depth_queue = stereo->depth.createOutputQueue(2, false);
    imu_queue = imu->createOutputQueue(10, false);

}


/* Run loop */
void SLAM::run() {

    // Initalize and start pipeline
    SLAM::init();
    pipeline->start();

    while(pipeline->isRunning()){

        // Read info from the node
        auto cam = cam_queue->get<dai::ImgFrame>();
        auto depth = depth_queue->get<dai::ImgFrame>();

        // actually this is wrong -- there is a lto more to this
        // will make own node and do calculations separately to convert
        // imu accelorometer and gyroscope data to transformatino
        auto imu = imu_queue->get<dai::IMU>():

        // CALL FEATURE DETCTION 

        if (frame_points.size() > 1){
            // FEATURE MATCHING
            feature_detection_and_matching_nodes();
        }

        // PLOT IN VISUALIZER




        // Exit condition
        // also want to exit if we detect loop closure
        if(cv::waitKey(1) == 'q') {
            break;
    }

    pipeline->stop();
    pipeline->wait();
}