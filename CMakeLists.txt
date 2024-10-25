cmake_minimum_required(VERSION 2.8)

project(hackathon_proj)

# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -g")

include_directories(./includes)

add_executable(main main.cpp ./srcs/utils.cpp)