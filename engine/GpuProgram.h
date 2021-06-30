#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"
#include "GfxTypes.h"
#include "mkVector.h"
#include "mkString.h"

struct GpuParameters
{
	struct ValueParameter
	{
		ValueParameter(const mkString& _name, ShaderDataType _type) :
			name(_name), type(_type)
		{
		}

		mkString name;
		ShaderDataType type;
	};

	struct UniformParameter
	{
		UniformParameter() {}
		UniformParameter(const mkString& _name, int _binding, VkShaderStageFlags _stageFlags) :
			name(_name), binding(_binding), stageFlags(_stageFlags)
		{
		}

		mkString name;
		int binding;
		VkShaderStageFlags stageFlags;

		mkVector<ValueParameter> valueParameters;
	};

	// 目前都是COMBINED_IMAGE_SAMPLER
	struct TextureParameter
	{
		TextureParameter(const mkString& _name, int _binding, VkShaderStageFlags _stageFlags) :
			name(_name), binding(_binding), stageFlags(_stageFlags)
		{
		}

		mkString name;
		int binding;
		VkShaderStageFlags stageFlags;
	};

	struct InputAttachmentParameter
	{
		InputAttachmentParameter(const mkString& _name, int _binding, VkShaderStageFlags _stageFlags) :
			name(_name), binding(_binding), stageFlags(_stageFlags)
		{
		}

		mkString name;
		int binding;
		VkShaderStageFlags stageFlags;
	};

	struct SpecializationConstantParameter
	{
		SpecializationConstantParameter(int p1, ShaderDataType p2):
			id(p1), type(p2) {}
		int id;
		ShaderDataType type;
	};

	mkVector<UniformParameter> uniformParameters;
	mkVector<TextureParameter> textureParameters;
	mkVector<InputAttachmentParameter> inputAttachmentParameters;

	// PushConstant
	// 为了保证Pipeline Layout Compatibility，统一设置128的PushConstant
	const uint32_t pushConstantSize = 128;
	const VkShaderStageFlags pushConstantStage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	// SpecializationConstant
	mkVector<SpecializationConstantParameter> SCParameters;
};

class GpuProgram : public NonCopyable
{
public:

	GpuProgram(GpuParameters& parameters);
	virtual ~GpuProgram();

	GpuParameters& GetGpuParameters();

protected:

	GpuParameters m_GpuParameters;
};