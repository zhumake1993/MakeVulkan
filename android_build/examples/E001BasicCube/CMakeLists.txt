cmake_minimum_required(VERSION 3.4.1)

# 胶水代码 native_app_glue
set(app_glue_dir ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${app_glue_dir})
add_library( app-glue STATIC ${app_glue_dir}/android_native_app_glue.c)

# =====设置预定义宏=====
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -std=c++11 -DVK_USE_PLATFORM_ANDROID_KHR")

set(name E001BasicCube)
set(top_dir ../../..)

# Vulkan loader
set(vulkan_wrapper_dir ${top_dir}/external/vulkan_wrapper)
include_directories(${vulkan_wrapper_dir})
add_library( vulkan_lib STATIC ${vulkan_wrapper_dir}/vulkan_wrapper.cpp)

# common
file(GLOB common_sources ${top_dir}/common/*.h ${top_dir}/common/*.cpp)
include_directories(${top_dir}/common)

# vulkan
file(GLOB vulkan_sources ${top_dir}/vulkan/*.h ${top_dir}/vulkan/*.cpp)
include_directories(${top_dir}/vulkan)

# engine
file(GLOB engine_sources ${top_dir}/engine/*.h ${top_dir}/engine/*.cpp)
include_directories(${top_dir}/engine)

# external
include_directories(${top_dir}/external)

# ktx
include_directories(${top_dir}/external/ktx/include)

# external/imgui
# imgui需要包含cpp文件
file(GLOB imgui_sources ${top_dir}/external/imgui/*.h ${top_dir}/external/imgui/*.cpp)

# 生成链接库
add_library (base STATIC ${common_sources} ${vulkan_sources} ${engine_sources} ${imgui_sources})
target_link_libraries(base vulkan_lib)

# Khronos texture format (https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/)
# CMAKE_CURRENT_SOURCE_DIR是CMakeList.txt所在的目录
target_link_libraries(base ${CMAKE_CURRENT_SOURCE_DIR}/../../../external/ktx/lib/liblibktx.a)

# 这里不能用test作为lib的名字，似乎跟重命名有关，原因不明
file(GLOB main_sources ${top_dir}/examples/${name}/*.h ${top_dir}/examples/${name}/*.cpp)
add_library(app SHARED ${main_sources})

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_link_libraries(app app-glue base log android)