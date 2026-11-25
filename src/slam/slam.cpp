#include <atomic>
#include <csignal>
#include <iostream>
#include <map>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>

#include "depthai/depthai.hpp"

#include "slam/slam.hpp"
#include "imu/imu.hpp"
#include "features/feature_matching_and_detection.h"

SLAM::SLAM() {}


/* Initialize nodes from camera */
void SLAM::init() {

    // Create device and pipeline
    device = std::make_shared<dai::Device>();
    pipeline = std::make_unique<dai::Pipeline>(device);

    // CAMERA (https://docs.luxonis.com/software-v3/depthai/depthai-components/nodes/camera/)
    cam = pipeline->create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_A);
    mono_left = pipeline->create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_B);
    mono_right = pipeline->create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_C);

    cam_out =  cam->requestOutput({640, 480}, dai::ImgFrame::Type::NV12, dai::ImgResizeMode::CROP, 30);
    mono_left_out = mono_left->requestOutput({640, 480});
    mono_right_out = mono_right->requestOutput({640, 480});

    // Apply some manipulation to camera stream
    //  - correct for wide lens distrotion
    //  - correct orientation (flip?)
    //  - correct color channel (rgb)
    // cam_out->setNumFramesPools(2);

    // IMU (https://docs.luxonis.com/software-v3/depthai/depthai-components/nodes/imu/)

    imu = pipeline->create<dai::node::IMU>();
    // enable ACCELEROMETER_RAW and GYROSCOPE_RAW at 100 hz rate
    imu->enableIMUSensor({dai::IMUSensor::ROTATION_VECTOR, dai::IMUSensor::ACCELEROMETER_CALIBRATED, dai::IMUSensor::GYROSCOPE_CALIBRATED}, 100);
    imu->setBatchReportThreshold(1);


    // STEREO DEPTH (https://docs.luxonis.com/software-v3/depthai/depthai-components/nodes/stereo_depth)
    stereo = pipeline->create<dai::node::StereoDepth>()->build(
        *mono_left_out, *mono_right_out, dai::node::StereoDepth::PresetMode::ROBOTICS);
    
    // apply some configurations for stereo to impove accuracy
    stereo->setLeftRightCheck(true);    // Better handling for occlusions:
    stereo->setSubpixel(true);          // Better accuracy for longer distance:
    stereo->setDepthAlign(dai::CameraBoardSocket::CAM_A);  // depth pixels line up with rgb


    // Define output queues to read from
    cam_queue = cam_out->createOutputQueue(2, false);
    mono_left_queue  = mono_left_out->createOutputQueue(2, false);
    mono_right_queue = mono_right_out->createOutputQueue(2, false);
    depth_queue = stereo->depth.createOutputQueue(2, false);
    imu_queue = imu->out.createOutputQueue(10, false);

}


/* Run loop */
void SLAM::run() {

    // Initalize and start pipeline
    SLAM::init();
    pipeline->start();

    while(pipeline->isRunning()){

        // Read info from the node
        auto cam_frame = cam_queue->get<dai::ImgFrame>();
        auto depth_frame = depth_queue->get<dai::ImgFrame>();

        // Get transformation via imu
        auto imu_data = imu_queue->get<dai::IMUData>();
        IMU::pushImuData(imu_data, imu_buffer);
        if(!imu_buffer.empty()) {
            IMU::Transform trans = IMU::imu2transform(imu_buffer.back());
        }

        // CALL FEATURE DETCTION 

        // TODO: frame_points doesn't exist yet
        // if (frame_points.size() > 1){
        //     // FEATURE MATCHING
        //     feature_detection_and_matching_nodes();
        // }

        // PLOT IN VISUALIZER




        // Exit condition
        // also want to exit if we detect loop closure
        if(cv::waitKey(1) == 'q') {
            break;
        }
    }

    pipeline->stop();
    pipeline->wait();
}