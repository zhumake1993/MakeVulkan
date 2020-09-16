#pragma once

struct DescriptorSetLayoutBinding
{
	VkDescriptorType descriptorType;
	uint32_t descriptorCount;
	VkShaderStageFlags stageFlags;
};

using DSLBindings = std::vector<DescriptorSetLayoutBinding>;

struct DescriptorPoolSize
{
	VkDescriptorType descriptorType;
	uint32_t count;
};

using DPSizes = std::vector<DescriptorPoolSize>;

union DescriptorInfo
{
	VkDescriptorBufferInfo buffer;
	VkDescriptorImageInfo image;
	VkBufferView texelBufferView;
};

struct DescriptorUpdateInfo {
	uint32_t binding;
	DescriptorInfo info;
};

using DesUpdateInfos = std::vector<DescriptorUpdateInfo>;