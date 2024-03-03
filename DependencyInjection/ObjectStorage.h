#pragma once
#include <vector>
#include <memory>
#include <optional>

typedef void* ObjectReference;

template <typename T>
class ObjectStorage {
private:
	std::vector<std::unique_ptr<T>> objects;

	int findObject(ObjectReference object)
	{
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			if (objects[i].get() == object)
			{
				return (int)i;
			}
		}
		return -1;
	}

public:
	template <typename... Params>
	ObjectReference allocate(Params... parameters)
	{
		objects.push_back(std::make_unique<T>(parameters...));
		return (ObjectReference)objects.back().get();
	}

	void deallocate(ObjectReference object)
	{
		int objectIndex = findObject(object);
		if (objectIndex >= 0)
		{
			objects.erase(objects.begin() + objectIndex);
			return;
		}
		assert(false);
	}
};


template <typename T>
class SingletonStorage
{
private:
	std::optional<std::unique_ptr<T>> mInstance;

public:
	SingletonStorage() : mInstance(std::nullopt) {}

	template <typename... Params>
	ObjectReference allocate(Params... parameters)
	{
		if (mInstance == std::nullopt)
		{
			mInstance = std::make_unique<T>(parameters...);
		}
		return mInstance.value().get();
	}

	void deallocate()
	{
		if (mInstance != std::nullopt)
		{
			mInstance = std::nullopt;
		}
	}
};