cmake_minimum_required(VERSION 3.25)
project(test_vulkan)

set(CMAKE_CXX_STANDARD 23)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(GLFW)
add_subdirectory(GLM)

find_package(Vulkan REQUIRED)
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find vulkan lib")
endif()

add_executable(test_vulkan
        ove_window.h
        ove_window.cpp
        ove_first_app.h
        ove_first_app.cpp
        main.cpp)
target_include_directories(test_vulkan PUBLIC ${Vulkan_INCLUDE_DIRS} GLFW/include GLM)
target_link_directories(test_vulkan PUBLIC GLFW/src)
target_link_libraries(test_vulkan ${Vulkan_LIBRARIES} glfw)
