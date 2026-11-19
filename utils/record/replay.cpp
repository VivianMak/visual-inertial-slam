// https://docs.luxonis.com/software-v3/depthai/examples/record_replay/holistic_replay/

#include <opencv2/highgui.hpp>

#include "depthai/depthai.hpp"

#ifndef DEPTHAI_MERGED_TARGET
    #error This example needs OpenCV support, which is not available on your system
#endif

#include <iostream>

int main(int argc, char** argv) {

    // Check recording path
    if (argc < 2){
        std::cerr << "Error: Please provide a file path.\n";
        std::cerr << "Usage: " << argv[0] << " <path/to/recording.tar>\n";
        return 1;
    }

    dai::Pipeline pipeline;

    // Path to the .tar written by record (or a folder of its extracted files)
    pipeline.enableHolisticReplay(argv[1]);

    auto camRgb = pipeline.create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_A);
    auto* camOut = camRgb->requestOutput({600, 400});

    auto imu = pipeline.create<dai::node::IMU>();

    // enable ACCELEROMETER_RAW at 500 hz rate
    imu->enableIMUSensor(dai::IMUSensor::ACCELEROMETER_RAW, 500);
    // enable GYROSCOPE_RAW at 400 hz rate
    imu->enableIMUSensor(dai::IMUSensor::GYROSCOPE_RAW, 400);
    imu->setBatchReportThreshold(100);

    auto videoQueue = camOut->createOutputQueue();
    auto q = imu->out.createOutputQueue();

    pipeline.start();

    while(pipeline.isRunning()) {
        auto videoIn = videoQueue->get<dai::ImgFrame>();
        auto imuData = q->get<dai::IMUData>();

        cv::imshow("video", videoIn->getCvFrame());

        for(auto& imuPacket : imuData->packets) {
            auto& acceleroValues = imuPacket.acceleroMeter;
            auto& gyroValues = imuPacket.gyroscope;

            printf("Accelerometer [m/s^2]: x: %.3f y: %.3f z: %.3f \n", acceleroValues.x, acceleroValues.y, acceleroValues.z);
            printf("Gyroscope [rad/s]: x: %.3f y: %.3f z: %.3f \n", gyroValues.x, gyroValues.y, gyroValues.z);
        }

        if(cv::waitKey(1) == 'q') {
            break;
        }
    }

    pipeline.stop();
}
