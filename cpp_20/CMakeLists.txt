cmake_minimum_required(VERSION 3.23)
project(cpp_20)

set(CMAKE_CXX_STANDARD 20)

find_package(CURL REQUIRED)

add_executable(cpp_20 download.cpp)
target_link_libraries(cpp_20 PRIVATE CURL::libcurl)