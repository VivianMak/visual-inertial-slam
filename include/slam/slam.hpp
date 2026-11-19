#pragma once

#include <memory>

#include "depthai/depthai.hpp"


class SLAM{

    public:
        // Constructor
        SLAM();

        // Run loop
        void run();


    private:
        std::shared_ptr<dai::Device> device;
        std::unique_ptr<dai::Pipeline> pipeline;

        std::shared_ptr<dai::MessageQueue> cam_queue;
        std::shared_ptr<dai::MessageQueue> mono_left_queue;
        std::shared_ptr<dai::MessageQueue> mono_right_queue;
        std::shared_ptr<dai::MessageQueue> depth_queue;
        std::shared_ptr<dai::MessageQueue> imu_queue;


        // Initalize luxonis nodes
        void init();


}; // class SLAM