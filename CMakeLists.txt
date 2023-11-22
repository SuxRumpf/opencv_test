cmake_minimum_required(VERSION 3.22)
project(opencv_test)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
find_package(PkgConfig REQUIRED)
pkg_search_module(GST REQUIRED gstreamer-1.0>=1.4
        gstreamer-sdp-1.0>=1.4
        gstreamer-video-1.0>=1.4
        gstreamer-app-1.0>=1.4)


add_executable(opencv_test src/cv_undistort.cpp)
target_link_libraries( opencv_test ${OpenCV_LIBS}  )

add_executable(gstreamer_test src/gstreamer_test.cpp)
target_link_libraries( gstreamer_test ${OpenCV_LIBS}  ${GST_LIBRARIES})
target_include_directories(gstreamer_test PRIVATE ${GST_INCLUDE_DIRS})
