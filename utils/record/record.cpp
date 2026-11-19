// https://docs.luxonis.com/software-v3/depthai/examples/record_replay/holistic_record/

#include <atomic>
#include <chrono>
#include <csignal>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <opencv2/highgui.hpp>
#include <sstream>

#include "depthai/common/CameraBoardSocket.hpp"
#include "depthai/depthai.hpp"
#include "depthai/utility/RecordReplay.hpp"
#ifndef DEPTHAI_MERGED_TARGET
    #error This example needs OpenCV support, which is not available on your system
#endif

// Replaces depthai-core's examples-only utility.hpp
static std::string getDefaultRecordingPath() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream name;
    name << "recording_" << std::put_time(std::localtime(&now), "%Y%m%d_%H%M%S");
    return (std::filesystem::path(PROJECT_ROOT) / "data" / name.str()).string();
}

// Signal handling for clean shutdown
static std::atomic<bool> isRunning = true;
void signalHandler(int signum) {
    (void)signum;
    isRunning = false;
}

int main(int argc, char** argv) {
    // Register signal handler
    std::signal(SIGINT, signalHandler);

    dai::Pipeline pipeline;

    dai::RecordConfig config;
    config.outputDir = argc > 1 ? std::string(argv[1]) : getDefaultRecordingPath();
    std::filesystem::create_directories(config.outputDir);  // depthai requires it to exist
    // config.videoEncoding.enabled = true;  // Use video encoding
    // config.videoEncoding.bitrate = 0;     // Automatic
    // config.videoEncoding.profile = dai::VideoEncoderProperties::Profile::H264_MAIN;

    pipeline.enableHolisticRecord(config);

    auto camA = pipeline.create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_A);
    auto* camAOut = camA->requestFullResolutionOutput(std::nullopt, 10);
    auto camB = pipeline.create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_B);
    auto* camBOut = camB->requestFullResolutionOutput(std::nullopt, 10);
    auto camC = pipeline.create<dai::node::Camera>()->build(dai::CameraBoardSocket::CAM_C);
    auto* camCOut = camC->requestFullResolutionOutput(std::nullopt, 10);

    auto* viewFinderOut = camA->requestOutput({640, 480});

    auto imu = pipeline.create<dai::node::IMU>();

    // enable ACCELEROMETER_RAW at 400 hz rate
    imu->enableIMUSensor(dai::IMUSensor::ACCELEROMETER_RAW, 400);
    // enable GYROSCOPE_RAW at 400 hz rate
    imu->enableIMUSensor(dai::IMUSensor::GYROSCOPE_RAW, 400);
    imu->setBatchReportThreshold(100);

    // Discard frames by passing through sync
    auto sync = pipeline.create<dai::node::Sync>();
    sync->setSyncAttempts(0);  // Don't wait for frames to sync
    camAOut->link(sync->inputs["camA"]);
    camBOut->link(sync->inputs["camB"]);
    camCOut->link(sync->inputs["camC"]);

    auto viewFinderQueue = viewFinderOut->createOutputQueue();

    pipeline.start();

    while(isRunning && pipeline.isRunning()) {
        auto frame = viewFinderQueue->get<dai::ImgFrame>();
        if(!frame) continue;
        cv::imshow("Video", frame->getCvFrame());
        auto key = cv::waitKey(10);
        if(key == 'q') {
            break;
        }
    }

    pipeline.stop();
}