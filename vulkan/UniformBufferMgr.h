#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKBuffer;

class UniformBufferMgr : public NonCopyable
{

	struct UBCache
	{
		std::vector<VKBuffer*> buffers;
		uint32_t index = 0;

		UBCache();
		UBCache(VkDeviceSize size);
		~UBCache();

		VKBuffer* GetBuffer();
		void Tick();
	};

public:

	UniformBufferMgr();
	~UniformBufferMgr();

	void CreateUniformBuffer(std::string name, VkDeviceSize size);
	VKBuffer* GetUniformBuffer(std::string name);
	void Tick();

private:

	//

private:

	std::unordered_map<std::string, UBCache> m_NameToUBCache;
};