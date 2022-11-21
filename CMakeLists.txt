cmake_minimum_required(VERSION 3.23)
project(VulkanTur)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB SRC src/*.cpp src/*.chh)

add_executable(VulkanTur ${SRC})
target_link_libraries(VulkanTur glfw;Vulkan::Vulkan)