#include <atomic>
#include <csignal>
#include <iostream>
#include <map>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>

#include "depthai/depthai.hpp"

#include "slam.hpp"

SLAM::SLAM() {}


/* Initialize nodes from camera */
void SLAM::init() {

    // Create device and pipeline
    device = std::make_shared<dai::Device>();
    pipeline = std::make_unique<dai::Pipeline>(device);

    // Create camera nodes and output queue
    cam = pipeline->create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_A);
    mono_left = pipeline->create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_B);
    mono_right = pipeline->create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_C);

    cam_out =  cam->requestOutput({640, 480}, dai::ImgFrame::Type::NV12, dai::ImgResizeMode::CROP, 2);
    mono_left_out = mono_left->requestOutput({640, 480});
    mono_right_out = mono_right->requestOutput({640, 480});

    // Apply some manipulation to camera stream
    //  - correct for wide lens distrotion
    //  - correct orientation (flip?)
    //  - correct color channel (rgb)


    // Stereo depth from the two monos
    // https://docs.luxonis.com/software-v3/depthai/depthai-components/nodes/stereo_depth
    auto stereo = pipeline->create<dai::node::StereoDepth>()->build(
        *monoLeftOut, *monoRightOut, dai::node::StereoDepth::PresetMode::ROBOTICS);
    
    // apply some configurations for stereo to impove accuracy
    stereo->setLeftRightCheck(true);    // Better handling for occlusions:
    stereo->setSubpixel(true);          // Better accuracy for longer distance:
    stereo->setDepthAlign(dai::CameraBoardSocket::CAM_A);  // depth pixels line up with rgb



    // Define output queues to read from
    cam_queue = cam_out->createOutputQueue();
    mono_left_queue  = mono_left_out->createOutputQueue();
    mono_right_queue = mono_right_out->createOutputQueue();
    depth_queue = stereo->depth.createOutputQueue();

}


/* Run loop */
void SLAM::run() {
    // read from node
    // feature extract and mathc
    // depth
    // plot
}