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
file(GLOB common_sources common/*.h common/*.cpp common/*.hpp)
include_directories(common)
source_group("common" FILES ${common_sources})

# vulkan
file(GLOB vulkan_sources vulkan/*.h vulkan/*.cpp vulkan/*.hpp)
include_directories(vulkan)
source_group("vulkan" FILES ${vulkan_sources})

# engine
file(GLOB engine_sources engine/*.h engine/*.cpp engine/*.hpp)
include_directories(engine)
source_group("engine" FILES ${engine_sources})

# external
include_directories(external)

# ktx
include_directories(external/ktx/include)

# external/glslang
# 用于运行时编译shader，目前不用
#add_subdirectory(external/glslang)
#include_directories(external/glslang)

# external/smolv
# 用于压缩shader，目前不用
#file(GLOB smolv_sources external/smolv/*.h external/smolv/*.cpp)
#source_group("smolv" FILES ${smolv_sources})

# external/imgui
# imgui需要包含cpp文件
file(GLOB imgui_sources external/imgui/*.h external/imgui/*.cpp external/imgui/*.hpp)
source_group("imgui" FILES ${imgui_sources})

# Assimp
# 安卓上面死活编不过去。。。
#include_directories(external/assimp)

# 生成链接库
add_library (base ${common_sources} ${vulkan_sources} ${engine_sources} ${imgui_sources})

# glslang需要额外链接
#target_link_libraries(base glslang SPIRV)

# Khronos texture format (https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/)
# 这里不能直接使用相对路径
target_link_libraries(base ${PROJECT_SOURCE_DIR}/external/ktx/lib/ktx.lib)

# Assimp
#target_link_libraries(base ${PROJECT_SOURCE_DIR}/external/assimp/assimp-vc140-mt.lib)
# Assimp还需要dll，需要copy到可执行目录
#add_custom_command(TARGET base POST_BUILD        						# Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND} -E copy 									# which executes "cmake - E copy_if_different..."
#        "${PROJECT_SOURCE_DIR}/external/assimp/assimp-vc140-mt.dll"     # <--this is in-file
#        $<TARGET_FILE_DIR:base>)                 						# <--this is out-file path

# 查找所有的example
file(GLOB EXAMPLES RELATIVE ${PROJECT_SOURCE_DIR}/examples/ examples/*)

function(buildExample EXAMPLE_NAME)

	# 指定生成目标
	# WIN32指示程序的入口是WinMain
	file(GLOB example_sources examples/${EXAMPLE_NAME}/*.h examples/${EXAMPLE_NAME}/*.cpp examples/${EXAMPLE_NAME}/*.hpp)
	add_executable(${EXAMPLE_NAME} WIN32 ${example_sources})
	source_group("" FILES ${example_sources})
	
	# 链接
	target_link_libraries(${EXAMPLE_NAME} ${vulkan_lib})
	target_link_libraries(${EXAMPLE_NAME} base)
	
endfunction(buildExample)

foreach(EXAMPLE ${EXAMPLES})
	buildExample(${EXAMPLE})
endforeach(EXAMPLE)