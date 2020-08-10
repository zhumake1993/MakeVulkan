# 收集所有头文件和源文件，将结果保存到 vulkan_base_sources 变量中
file(GLOB vulkan_base_sources *.h *.cpp)

# 组织文件结构
source_group("Base" FILES ${vulkan_base_sources})

# 生成链接库
add_library (vulkan_base_lib ${vulkan_base_sources})

#IF(WIN32)
	#target_link_libraries(vulkan_base_lib ${vulkan_lib})
#ELSE(WIN32)
	#target_link_libraries(vulkan_base_lib vulkan_lib)
#ENDIF(WIN32)