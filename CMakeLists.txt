cmake_minimum_required(VERSION 3.25)
project(test_vulkan)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(GLFW)
add_subdirectory(GLM)

add_executable(test_vulkan main.cpp)
