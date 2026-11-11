cmake_minimum_required(VERSION 3.20)
project(visual_inertial_slam CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(depthai CONFIG REQUIRED)
find_package(OpenCV REQUIRED)


# Testing connection to camear
add_executable(camera utils/camera.cpp)
target_link_libraries(camera PRIVATE depthai::core ${OpenCV_LIBS})

add_executable(camera_raw utils/camera_raw.cpp)
target_link_libraries(camera_raw PRIVATE depthai::core ${OpenCV_LIBS})