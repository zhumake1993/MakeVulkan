#pragma once

#include "Common.h"
#include "VulkanFwd.h"

class Engine
{

public:

	Engine();
	virtual ~Engine();

	void CleanUpEngine();
	void InitEngine();
	void TickEngine();

protected:

	virtual void CleanUp() = 0;
	virtual void Init() = 0;
	virtual void Tick() = 0;

	virtual void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer) = 0;

protected:

	//

};