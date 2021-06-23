#pragma once

#include "NonCopyable.h"

class GarbageCollector;

class VKResource : public NonCopyable
{
public:

	VKResource();
	virtual ~VKResource();

	bool InUse();
	void Use();

	void Release();

private:

	// 在该帧被使用
	int m_FrameIndex = -999;

	GarbageCollector* m_GarbageCollector;
};

template <class Key, class Resource, class KeyHash>
class ResourcePool
{
	class PoolEntry
	{
	public:

		~PoolEntry()
		{
			for (auto itr = resources.begin(); itr != resources.end(); itr++)
			{
				delete *itr;
			}
			resources.clear();
		}

		Resource* Get()
		{
			if (!resources.empty() && !resources.back()->InUse())
			{
				Resource* resource = resources.back();
				resources.pop_back();
				return resource;
			}
			else
			{
				return nullptr;
			}
		}

		void Add(Resource* resource)
		{
			resources.push_front(resource);
		}

	private:

		std::list<Resource*> resources;
	};

public:

	Resource* Get(Key key)
	{
		auto itr = m_Pool.find(key);
		if (itr != m_Pool.end())
		{
			return itr->second.Get();
		}
		else
		{
			return nullptr;
		}
	}

	void Add(Key key, Resource* resource)
	{
		if (m_Pool.find(key) == m_Pool.end())
		{
			m_Pool[key] = PoolEntry();
		}
		m_Pool[key].Add(resource);
	}

private:

	std::unordered_map<Key, PoolEntry, KeyHash> m_Pool;
};