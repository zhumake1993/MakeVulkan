# 收集所有头文件和源文件，将结果保存到 vulkan_base_sources 变量中
file(GLOB vulkan_base_sources *.h *.cpp)

# 组织文件结构
source_group("Base" FILES ${vulkan_base_sources})

# 生成链接库
add_library (vulkan_base_lib ${vulkan_base_sources})

# =====设置预定义宏=====
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_ANDROID_KHR")

# =====头文件=====
# vulkan，这个测试过是不需要的
#include_directories(${VULKAN_LIB_INCLUDE})

# =====链接库=====
# 
IF(WIN32)
	target_link_libraries(vulkan_base_lib ${vulkan_lib})
ELSE(WIN32)
	target_link_libraries(vulkan_base_lib vulkan_lib)
ENDIF(WIN32)