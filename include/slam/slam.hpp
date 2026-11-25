#pragma once

#include <deque>
#include <memory>

#include "depthai/depthai.hpp"

#include "imu/imu.hpp"


class SLAM{

    public:
        // Constructor
        SLAM();

        // Run loop
        void run();


    private:
        std::shared_ptr<dai::Device> device;
        std::unique_ptr<dai::Pipeline> pipeline;

        // Nodes
        std::shared_ptr<dai::node::Camera> cam;
        std::shared_ptr<dai::node::Camera> mono_left;
        std::shared_ptr<dai::node::Camera> mono_right;
        std::shared_ptr<dai::node::IMU> imu;
        std::shared_ptr<dai::node::StereoDepth> stereo;

        // Camera outputs (owned by the camera nodes)
        dai::Node::Output* cam_out = nullptr;
        dai::Node::Output* mono_left_out = nullptr;
        dai::Node::Output* mono_right_out = nullptr;

        std::shared_ptr<dai::MessageQueue> cam_queue;
        std::shared_ptr<dai::MessageQueue> mono_left_queue;
        std::shared_ptr<dai::MessageQueue> mono_right_queue;
        std::shared_ptr<dai::MessageQueue> depth_queue;
        std::shared_ptr<dai::MessageQueue> imu_queue;

        // Last IMU::kMaxReadings imu readings
        std::deque<IMU::ImuData> imu_buffer;


        // Initalize luxonis nodes
        void init();


}; // class SLAM