cmake_minimum_required(VERSION 2.8)

project(hackathon_proj)

# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -g")

include_directories(./includes)
aux_source_directory(./srcs SRC_LIST)

add_executable(main main.cpp ${SRC_LIST})