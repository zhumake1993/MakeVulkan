#pragma once

#include "Env.h"
#include "NonCopyable.h"

struct GpuParameters
{
	enum UniformDataType
	{
		kUniformDataFloat1,
		kUniformDataFloat2,
		kUniformDataFloat3,
		kUniformDataFloat4,
		kUniformDataFloat4x4
	};

	struct ValueParameter
	{
		ValueParameter(const std::string& _name, UniformDataType _type) :
			name(_name), type(_type)
		{
		}

		std::string name;
		UniformDataType type;
	};

	struct UniformParameter
	{
		UniformParameter(const std::string& _name, int _binding, VkShaderStageFlags _stageFlags) :
			name(_name), binding(_binding), stageFlags(_stageFlags)
		{
		}

		std::string name;
		int binding;
		VkShaderStageFlags stageFlags;

		std::vector<ValueParameter> valueParameters;
	};

	// 目前都是COMBINED_IMAGE_SAMPLER
	struct TextureParameter
	{
		TextureParameter(const std::string& _name, int _binding, VkShaderStageFlags _stageFlags) :
			name(_name), binding(_binding), stageFlags(_stageFlags)
		{
		}

		std::string name;
		int binding;
		VkShaderStageFlags stageFlags;
	};

	std::vector<UniformParameter> uniformParameters;
	std::vector<TextureParameter> textureParameters;

	// 为了保证Pipeline Layout Compatibility，统一设置128的PushConstant
	const uint32_t pushConstantSize = 128;
	const VkShaderStageFlags pushConstantStage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
};

class GpuProgram : public NonCopyable
{
public:

	GpuProgram(GpuParameters& parameters);
	virtual ~GpuProgram();

	GpuParameters& GetGpuParameters();

	//uint32_t GetUniformBufferSize(std::string name);

protected:

	GpuParameters m_GpuParameters;
};