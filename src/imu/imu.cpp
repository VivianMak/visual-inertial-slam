#include <chrono>

#include <Eigen/Dense>
#include "depthai/depthai.hpp"

#include "imu/imu.hpp"

namespace IMU{

    // std::tuple<double, double, double> quaternionToEulerXYZ(float i, float j, float k, float real) {
    //     const double sinrCosp = 2.0 * (real * i + j * k);
    //     const double cosrCosp = 1.0 - 2.0 * (i * i + j * j);
    //     const double x = std::atan2(sinrCosp, cosrCosp) * 180.0 / kPi;

    //     double sinp = 2.0 * (real * j - k * i);
    //     sinp = std::clamp(sinp, -1.0, 1.0);
    //     const double y = std::asin(sinp) * 180.0 / kPi;

    //     const double sinyCosp = 2.0 * (real * k + i * j);
    //     const double cosyCosp = 1.0 - 2.0 * (j * j + k * k);
    //     const double z = std::atan2(sinyCosp, cosyCosp) * 180.0 / kPi;

    //     return {x, y, z};
    // }

    void pushImuData(const std::shared_ptr<dai::IMUData>& imu_data, std::deque<ImuData>& buffer){

        // Check empty imu data
        if(imu_data == nullptr) return;

        for(const auto& pkt : imu_data->packets) {

            // Get packets of imu data
            const auto& rotationVector = pkt.rotationVector;
            const auto& acceloroMeter  = pkt.acceleroMeter;
            const auto& gyroscope = pkt.gyroscope;

            // Assign imu packet data to struct
            IMU::ImuData imu;

            // Each sensor has its own timestamp; use the accelerometer's as the sample time
            imu.t = std::chrono::duration<double>(acceloroMeter.getTimestampDevice().time_since_epoch()).count();

            imu.i    = rotationVector.i;
            imu.j    = rotationVector.j;
            imu.k    = rotationVector.k;
            imu.real = rotationVector.real;

            imu.ax = acceloroMeter.x;
            imu.ay = acceloroMeter.y;
            imu.az = acceloroMeter.z;

            imu.gx = gyroscope.x;
            imu.gy = gyroscope.y;
            imu.gz = gyroscope.z;

            buffer.push_back(imu);

            // Drop the oldest readings past the limit
            while(buffer.size() > kMaxReadings) {
                buffer.pop_front();
            }
        }

        // // Print the latest IMU sample at most once per second.
        // std::cout << "Accelerometer timestamp: " << acceleroTs.time_since_epoch().count() << std::endl;
        // std::cout << "Latency [ms]: " << timeDeltaToMilliS(std::chrono::steady_clock::now() - acceleroValues.getTimestamp()) << std::endl;
        // std::cout << "Accelerometer [m/s^2]: x: " << acceleroValues.x << " y: " << acceleroValues.y << " z: " << acceleroValues.z << std::endl;

        // std::cout << "Gyroscope timestamp: " << gyroTs.time_since_epoch().count() << std::endl;
        // std::cout << "Gyroscope [rad/s]: x: " << gyroValues.x << " y: " << gyroValues.y << " z: " << gyroValues.z << std::endl;
    }

    IMU::Transform imu2transform(const ImuData& imu){

        IMU::Transform trans;
        trans.t = imu.t;

        // Get rotation matrix
        Eigen::Matrix3d R = Eigen::Quaterniond(
            imu.real, imu.i, imu.j, imu.k
        ).toRotationMatrix();

        // Place rotation in the top-left of a 4x4, translation left at zero
        trans.trans_mat = Eigen::Matrix4d::Identity();
        trans.trans_mat.block<3, 3>(0, 0) = R;

        return trans;
    }

} // namespace IMU
