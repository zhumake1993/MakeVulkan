cmake_minimum_required(VERSION 3.4.1)

# 胶水代码 native_app_glue
set(app_glue_dir ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${app_glue_dir})
add_library( app-glue STATIC ${app_glue_dir}/android_native_app_glue.c)

# =====设置预定义宏=====
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -std=c++11 -DVK_USE_PLATFORM_ANDROID_KHR")

# Vulkan loader
set(vulkan_wrapper_dir ../../../vulkan_wrapper)
include_directories(${vulkan_wrapper_dir})
add_library( vulkan_lib STATIC ${vulkan_wrapper_dir}/vulkan_wrapper.cpp)

# vulkan_base
set(vulkan_base_dir ../../../vulkan_base)
add_subdirectory( ${vulkan_base_dir} vulkan_base_binary_dir)
include_directories( ${vulkan_base_dir} )

# 这里不能用test作为lib的名字，似乎跟重命名有关，原因不明
add_library(app SHARED ../../../examples/test/main.cpp)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_link_libraries(app app-glue vulkan_lib vulkan_base_lib log android)