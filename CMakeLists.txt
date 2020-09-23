# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (MakeVulkan)

# 设置预定义宏
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")

# 设置vulkan的lib和include
# 链接vulkan-1.lib可以省去繁琐的vulkan函数载入步骤
set(vulkan_lib C:/VulkanSDK/1.2.131.2/Lib/vulkan-1.lib)
set(vulkan_lib_include C:/VulkanSDK/1.2.131.2/Include)
include_directories(${vulkan_lib_include})

# common
file(GLOB common_sources common/*.h common/*.cpp)
include_directories(common)
source_group("common" FILES ${common_sources})

# vulkan
file(GLOB vulkan_sources vulkan/*.h vulkan/*.cpp)
include_directories(vulkan)
source_group("vulkan" FILES ${vulkan_sources})

# engine
file(GLOB engine_sources engine/*.h engine/*.cpp)
include_directories(engine)
source_group("engine" FILES ${engine_sources})

# external
include_directories(external)

# external/imgui
# imgui需要包含cpp文件
file(GLOB imgui_sources external/imgui/*.h external/imgui/*.cpp)
source_group("imgui" FILES ${imgui_sources})

# 生成链接库
add_library (base ${common_sources} ${vulkan_sources} ${engine_sources} ${imgui_sources})

# 查找所有的example
file(GLOB EXAMPLES RELATIVE ${PROJECT_SOURCE_DIR}/examples/ examples/*)

function(buildExample EXAMPLE_NAME)

	# 指定生成目标
	# WIN32指示程序的入口是WinMain
	file(GLOB example_sources examples/${EXAMPLE_NAME}/*.h examples/${EXAMPLE_NAME}/*.cpp)
	add_executable(${EXAMPLE_NAME} WIN32 ${example_sources})
	source_group("" FILES ${example_sources})
	
	# 链接
	target_link_libraries(${EXAMPLE_NAME} ${vulkan_lib})
	target_link_libraries(${EXAMPLE_NAME} base)
	
endfunction(buildExample)

foreach(EXAMPLE ${EXAMPLES})
	buildExample(${EXAMPLE})
endforeach(EXAMPLE)