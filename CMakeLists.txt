# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (MakeVulkan)

# 指定生成目标
# WIN32指示程序的入口是WinMain
add_executable(Test WIN32 examples/test/main.cpp)

# 设置vulkan
set(vulkan_lib C:/VulkanSDK/1.2.131.2/Lib/vulkan-1.lib)
set(vulkan_lib_include C:/VulkanSDK/1.2.131.2/Include)

include_directories(${vulkan_lib_include})
target_link_libraries(Test ${vulkan_lib})

# =====设置预定义宏=====
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")

# 添加 vulkan_base 子目录
add_subdirectory(vulkan_base)
include_directories(vulkan_base)
target_link_libraries(Test vulkan_base_lib)