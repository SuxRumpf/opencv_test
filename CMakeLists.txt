cmake_minimum_required(VERSION 3.22)
project(opencv_test)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )


add_executable(opencv_test src/cv_undistort.cpp)
target_link_libraries( opencv_test ${OpenCV_LIBS} )