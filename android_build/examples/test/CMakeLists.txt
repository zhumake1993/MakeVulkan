cmake_minimum_required(VERSION 3.4.1)

# 胶水代码 native_app_glue
set(app_glue_dir ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${app_glue_dir})
add_library( app-glue STATIC ${app_glue_dir}/android_native_app_glue.c)

# =====设置预定义宏=====
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -std=c++11 -DVK_USE_PLATFORM_ANDROID_KHR")

# Vulkan loader
set(vulkan_wrapper_dir ../../../external/vulkan_wrapper)
include_directories(${vulkan_wrapper_dir})
add_library( vulkan_lib STATIC ${vulkan_wrapper_dir}/vulkan_wrapper.cpp)

set(top_dir ../../..)

# common
file(GLOB common_sources ${top_dir}/common/*.h ${top_dir}/common/*.cpp)
include_directories(${top_dir}/common)
#source_group("common" FILES ${common_sources})

# vulkan
file(GLOB vulkan_sources ${top_dir}/vulkan/*.h ${top_dir}/vulkan/*.cpp)
include_directories(${top_dir}/vulkan)
#source_group("vulkan" FILES ${vulkan_sources})

# engine
file(GLOB engine_sources ${top_dir}/engine/*.h ${top_dir}/engine/*.cpp)
include_directories(${top_dir}/engine)
#source_group("engine" FILES ${engine_sources})

# 生成链接库
add_library (base ${common_sources} ${vulkan_sources} ${engine_sources})
target_link_libraries(base vulkan_lib)

# 这里不能用test作为lib的名字，似乎跟重命名有关，原因不明
add_library(app SHARED ../../../examples/test/main.cpp)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_link_libraries(app app-glue base log android)