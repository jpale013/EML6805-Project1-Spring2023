cmake_minimum_required(VERSION 3.1)

project(
  lineFollower
  VERSION 1.0
  LANGUAGES CXX)

find_package(OpenCV REQUIRED)

add_executable(lineFollower main.cpp)

target_link_libraries(lineFollower ${OpenCV_LIBS})
