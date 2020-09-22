#pragma once

#include "Engine.h"
#include "VulkanFwd.h"
#include "EngineFwd.h"

class Startup : public Engine
{

public:

	Startup();
	virtual ~Startup();

protected:

	void CleanUp() override;
	void Init() override;
	void Tick() override;
	void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer) override;

private:

	//

private:

	//
};