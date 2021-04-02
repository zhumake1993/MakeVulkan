#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "Settings.h"

class VKResource : public NonCopyable
{
public:

	VKResource();
	virtual ~VKResource();

	bool InUse();
	void Use();

	virtual size_t Hash();

private:

	// 在该帧被使用
	int m_FrameIndex = -FrameResourcesCount;
};

template <class Resource>
class ResourcePoolOld
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

	Resource* Get(size_t hash)
	{
		if (m_Pool.find(hash) != m_Pool.end())
		{
			return m_Pool[hash].Get();
		}
		return nullptr;
	}

	void Add(Resource* resource)
	{
		size_t hash = resource->Hash();
		if (m_Pool.find(hash) == m_Pool.end())
		{
			m_Pool[hash] = PoolEntry();
		}
		m_Pool[hash].Add(resource);
	}

private:

	std::unordered_map<size_t, PoolEntry> m_Pool;
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