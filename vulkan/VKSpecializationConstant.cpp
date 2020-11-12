#include "VKSpecializationConstant.h"
#include "Tools.h"

VKSpecializationConstant::VKSpecializationConstant()
{
	m_Data = new char[m_MaxSize];
}

VKSpecializationConstant::~VKSpecializationConstant()
{
}

void VKSpecializationConstant::Add(int id, uint32_t value)
{
	if (m_CurrOffset + sizeof(uint32_t) > m_MaxSize) {
		LOG("too much specialization constants!");
		assert(false);
	}

	for (auto& item : m_Items) {
		if (item.id == id) {
			LOG("same specialization constant id!");
			assert(false);
		}
	}

	SCItem item;

	item.id = id;
	item.size = sizeof(uint32_t);
	item.offset = m_CurrOffset;

	uint32_t* addr = reinterpret_cast<uint32_t*>(m_Data + item.offset);
	*addr = value;

	m_CurrOffset += item.size;

	m_Items.push_back(item);
}

void VKSpecializationConstant::Set(int id, uint32_t value)
{
	for (auto& item : m_Items) {
		if (item.id == id) {
			uint32_t* addr = reinterpret_cast<uint32_t*>(m_Data + item.offset);
			*addr = value;

			return;
		}
	}

	LOG("specialization constant id not found!");
	assert(false);
	return;
}

VkSpecializationInfo * VKSpecializationConstant::GetVkSpecializationInfoPtr()
{
	uint32_t num = static_cast<uint32_t>(m_Items.size());

	m_Entries.resize(num);

	for (uint32_t i = 0; i < num; i++) {
		m_Entries[i].constantID = m_Items[i].id;
		m_Entries[i].size = m_Items[i].size;
		m_Entries[i].offset = m_Items[i].offset;
	}

	m_SpecializationInfo = {};
	m_SpecializationInfo.dataSize = m_CurrOffset;
	m_SpecializationInfo.mapEntryCount = num;
	m_SpecializationInfo.pMapEntries = m_Entries.data();
	m_SpecializationInfo.pData = m_Data;

	return &m_SpecializationInfo;
}

