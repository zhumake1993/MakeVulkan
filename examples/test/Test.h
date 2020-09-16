#pragma once

#include "Engine.h"
#include "VulkanFwd.h"
#include "EngineFwd.h"

class Test : public Engine
{

public:

	Test();
	virtual ~Test();

protected:

	void CleanUp() override;
	void Init() override;
	void Tick() override;
	void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer) override;

private:

	void PrepareModels();
	void PrepareTextures();
	void PrepareDescriptorSet();
	void CreatePipeline();

private:

	//
};