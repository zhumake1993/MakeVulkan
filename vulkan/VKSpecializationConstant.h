#pragma once

#include "Common.h"


class VKSpecializationConstant
{

	struct SCItem
	{
		uint32_t id;
		uint32_t size;
		uint32_t offset;
	};

public: 
	
	VKSpecializationConstant();
	~VKSpecializationConstant();

	void Add(int id, uint32_t value);
	void Set(int id, uint32_t value);

	VkSpecializationInfo* GetVkSpecializationInfoPtr();

private:

	uint32_t m_CurrOffset = 0;
	char* m_Data = nullptr;
	const uint32_t m_MaxSize = 1024;
	std::vector<SCItem> m_Items;

	std::vector<VkSpecializationMapEntry> m_Entries;
	VkSpecializationInfo m_SpecializationInfo;
};