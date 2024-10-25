cmake_minimum_required(VERSION 2.8)

project(hackathon_proj)

include_directories(./includes)

add_executable(main main.cpp ./srcs/utils.cpp)
add_executable(srcs/decompress srcs/decompress.cpp srcs/utils.cpp)
