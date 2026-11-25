#pragma once

#include <cstddef>
#include <deque>
#include <memory>

#include <Eigen/Dense>
#include "depthai/depthai.hpp"

namespace IMU{

    // Number of most recent readings kept in the buffer
    constexpr std::size_t kMaxReadings = 20;

    struct ImuData{
        double t;                 // seconds, device clock
        float i, j, k, real;      // quaternion rotation vector
        float ax, ay, az;         // m/s^2, accelerometer
        float gx, gy, gz;         // rad/s, gyroscope
    };

    struct Transform{
        double t;
        Eigen::Matrix4d trans_mat;
    };

    // /**
    //  * Turning quaterion rotation vector to euler roll, pitch, yaw.
    //  */
    // std::tuple<double, double, double>
    // quaternionToEulerXYZ(float i, float j, float k, float real);

    /**
     * Push every packet of an imu message onto the buffer, keeping only the last kMaxReadings
     */
    void pushImuData(const std::shared_ptr<dai::IMUData>& imu_data, std::deque<ImuData>& buffer);

    /**
     * Build a 4x4 transform from one imu reading (rotation only, translation is zero)
     */
    Transform imu2transform(const ImuData& imu);
}
