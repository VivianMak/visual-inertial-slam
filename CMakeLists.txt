
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

# ORB feature matching
find_package(OpenCV REQUIRED)

add_executable(orb_video src/orb_video.cpp)
target_compile_features(orb_video PRIVATE cxx_std_17)
target_include_directories(orb_video PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(orb_video PRIVATE ${OpenCV_LIBS})
add_executable(record utils/record/record.cpp)
target_link_libraries(record PRIVATE depthai::core ${OpenCV_LIBS})
target_compile_definitions(record PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

add_executable(replay utils/record/replay.cpp)
target_link_libraries(replay PRIVATE depthai::core ${OpenCV_LIBS})
