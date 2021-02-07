#pragma once

#include "Env.h"
#include "NonCopyable.h"

enum UniformDataType
{
	kUniformDataTypeFloat1,
	kUniformDataTypeFloat2,
	kUniformDataTypeFloat3,
	kUniformDataTypeFloat4,

	kUniformDataTypeFloat4x4
};

std::string UniformDataTypeToString(UniformDataType type);

uint32_t UniformDataTypeToSize(UniformDataType type);

struct UniformBufferElement
{
	UniformBufferElement(UniformDataType t, std::string n) :type(t), name(n) {}

	UniformDataType type;
	std::string name;
	uint32_t offset;
};

struct UniformBufferLayout
{
	UniformBufferLayout(std::string n, int b, VkShaderStageFlags flags) :name(n), binding(b), stageFlags(flags) {}

	void Add(UniformBufferElement element)
	{
		element.offset = GetSize();
		elements.push_back(element);
	}

	uint32_t GetSize()
	{
		if (elements.empty())
		{
			return 0;
		}
		else
		{
			return elements.back().offset + UniformDataTypeToSize(elements.back().type);
		}
	}

	std::string name;
	int binding;
	VkShaderStageFlags stageFlags;
	std::vector<UniformBufferElement> elements;
};

struct GpuParameters
{
	std::vector<UniformBufferLayout> uniformBufferLayouts;
};

class GpuProgram : public NonCopyable
{
public:

	GpuProgram(GpuParameters& parameters);
	virtual ~GpuProgram();

	GpuParameters& GetGpuParameters();

	uint32_t GetUniformBufferSize(std::string name);

protected:

	GpuParameters m_GpuParameters;
};