#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "Tools.h"

struct VKBuffer;

class UniformBufferMgr : public NonCopyable
{

	struct UBCache
	{
		std::vector<VKBuffer*> buffers;
		uint32_t index = 0;

		UBCache(VkDeviceSize size) {
			buffers.resize(FrameResourcesCount);

			for (int i = 0; i < FrameResourcesCount; i++) {
				buffers[i] = GetVulkanDriver().CreateVKBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}
		}

		~UBCache() {
			for (int i = 0; i < FrameResourcesCount; i++) {
				RELEASE(buffers[i]);
			}
		}

		VKBuffer* GetBuffer() {
			return buffers[index];
		}

		void Tick() {
			index = (index + 1) % FrameResourcesCount;
		}
	};

public:

	UniformBufferMgr();
	~UniformBufferMgr();

	VKBuffer* GetUniformBuffer(std::string name, VkDeviceSize size);
	void Tick();

private:

	//

private:

	std::unordered_map<std::string, UBCache> m_NameToUBCache;
};