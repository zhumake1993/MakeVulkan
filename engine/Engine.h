#pragma once

#include "Common.h"

class VulkanCommandBuffer;
class VulkanFramebuffer;

class Engine
{

public:

	Engine();
	virtual ~Engine();

	void CleanUpEngine();
	void InitEngine();
	void TickEngine();

protected:

	virtual void CleanUp() {}
	virtual void Init() {}
	virtual void Tick() {}
	virtual void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer, VulkanFramebuffer* vulkanFramebuffer) {}

protected:

	//

};