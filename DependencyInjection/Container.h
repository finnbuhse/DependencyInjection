#pragma once
#include "Dependent.h"
#include <type_traits>
#include <cassert>

template <typename T>
concept DefaultConstructible = std::is_default_constructible_v<T>;

// Forward declaration
class Container;

enum class InjectableType
{
	Transient,
	Singleton,
	N_InjectableTypes,
	Unknown
};

// Struct responsible for storing a container's data about a Transient type including a constructor function, deconstructor function, and all individual instances of the transient
class InjectableInfo
{
private:
	using AllocateFunctionType = std::function<ObjectReference(Container& container)>;
	using DeallocateFunctionType = std::function<void(Container& container, ObjectReference pObject)>;

	AllocateFunctionType mGetInstanceFunc; // Creates instance within mInstances and returns a pointer to the created object
	DeallocateFunctionType mDestroyInstanceFunc; // Destroys instance found within mInstances

public:
	const InjectableType type;

	InjectableInfo(const InjectableType _type, AllocateFunctionType allocateFunc, DeallocateFunctionType deallocateFunc) : type(_type), mGetInstanceFunc(allocateFunc), mDestroyInstanceFunc(deallocateFunc) {}

	InjectableInfo() : type(InjectableType::Unknown) {}

	ObjectReference getInstance(Container& container);

	void destroyInstance(Container& container, ObjectReference pObject);
};

/*
Manages storage and lifetime of objects, as well as enabling dependency injection.
*/
class Container
{
private:
	std::unordered_map<TypeIdentifier, InjectableInfo> mTypes;

	template <typename T>
	void registerTransientDefaultConstructible()
	{
		mTypes.insert({ TypeIdentifier::fromType<T>(), InjectableInfo(
			InjectableType::Transient,

			[](Container& container) {	// Insert lambda function
				return container.getTransientTypeStorage<T>().allocate();
			},
			[](Container& container, ObjectReference pObject) {
				return container.getTransientTypeStorage<T>().deallocate(pObject);
			}
		) });
	}

	template <typename T>
	void registerTransientDependent()
	{
		// T must inherit from Dependent<...> which ensures T implements the following functions:
		// - dependencyTypeIdentifiers()
		assert(("[ERROR] Injectable that is not default constructible must inherit from Dependent<...> ", std::is_base_of<IDependent, T>::value));

		mTypes.insert({ TypeIdentifier::fromType<T>(), InjectableInfo(
			InjectableType::Transient,

			[](Container& container) {	// Insert lambda function
				std::vector<TypeIdentifier> dependencyTypes = T::dependencyTypeIdentifiers();

				// Get dependencies
				std::vector<ObjectReference> dependencies(dependencyTypes.size());
				for (unsigned int i = 0; i < dependencyTypes.size(); i++)
				{
					dependencies[i] = container.getService(dependencyTypes[i]);
				}


				// Then construct dependent object
				return container.getTransientTypeStorage<T>().allocate(dependencies);
			},

			[](Container& container, ObjectReference pObject) {
				std::vector<TypeIdentifier> dependencyTypes = T::dependencyTypeIdentifiers();

				T* pDependent = (T*)pObject;

				std::vector<ObjectReference> dependencies = pDependent->dependencyPointers();
				for (unsigned int i = 0; i < dependencies.size(); i++)
				{
					// Deallocate transient dependencies
					if (container.isTransient<T>())
					{
						container.destroyService(dependencyTypes[i], dependencies[i]);
					}
				}

				// Then deallocate dependent object
				container.getTransientTypeStorage<T>().deallocate(pObject);
			}
		) });
	}

	template <typename T>
	void registerSingletonDefaultConstructible()
	{
		mTypes.insert({ TypeIdentifier::fromType<T>(), InjectableInfo(
			InjectableType::Singleton,

			[](Container& container) {	// Insert lambda function
				return container.getSingletonTypeStorage<T>().allocate();
			},
			[](Container& container, ObjectReference pObject) {
				return container.getSingletonTypeStorage<T>().deallocate();
			}
		) });
	}

	template <typename T>
	void registerSingletonDependent()
	{
		// T must inherit from Dependent<...> which ensures T implements the following functions:
		// - dependencyTypeIdentifiers()
		assert(("[ERROR] Injectable that is not default constructible must inherit from Dependent<...> ", std::is_base_of<IDependent, T>::value));

		mTypes.insert({ TypeIdentifier::fromType<T>(), InjectableInfo(
			InjectableType::Singleton,

			[](Container& container) {	// Insert lambda function
				std::vector<TypeIdentifier> dependencyTypes = T::dependencyTypeIdentifiers();

				// Get dependencies
				std::vector<ObjectReference> dependencies(dependencyTypes.size());
				for (unsigned int i = 0; i < dependencyTypes.size(); i++)
				{
					dependencies[i] = container.getService(dependencyTypes[i]);
				}


				// Then construct dependent object
				return container.getSingletonTypeStorage<T>().allocate(dependencies);
			},

			[](Container& container, ObjectReference pObject) {
				std::vector<TypeIdentifier> dependencyTypes = T::dependencyTypeIdentifiers();

				T* pDependent = (T*)pObject;

				std::vector<ObjectReference> dependencies = pDependent->dependencyPointers();
				for (unsigned int i = 0; i < dependencies.size(); i++)
				{
					// Deallocate transient dependencies
					if (container.isTransient<T>())
					{
						container.destroyService(dependencyTypes[i], dependencies[i]);
					}
				}

				// Then deallocate dependent object
				container.getSingletonTypeStorage<T>().deallocate();
			}
		) });
	}

	void assertRegistered(const TypeIdentifier& typeIdentifier)
	{
		assert(("[ERROR] Attempted to get service which has not been registered", isRegistered(typeIdentifier)));
	}

public:
	// Templated

	/*
	One ObjectStorage instance is present for each transient type
	*/
	template <typename T>
	ObjectStorage<T>& getTransientTypeStorage()
	{
		static ObjectStorage<T> objectStorage; // TODO: Improve
		return objectStorage;
	}

	/*
	One SingletonStorage instance is present for each singleton type
	*/
	template <typename T>
	SingletonStorage<T>& getSingletonTypeStorage()
	{
		static SingletonStorage<T> singletonStorage; // TODO: Improve
		return singletonStorage;
	}

	template <DefaultConstructible T>
	void addTransient()
	{
		registerTransientDefaultConstructible<T>();
	}

	template <typename T>
	requires (!DefaultConstructible<T>)
	void addTransient()
	{
		registerTransientDependent<T>();
	}

	template <DefaultConstructible T>
	void addSingleton()
	{
		registerSingletonDefaultConstructible<T>();
	}

	template <typename T>
	requires (!DefaultConstructible<T>)
	void addSingleton()
	{
		registerSingletonDependent<T>();
	}

	template <typename T>
	bool isRegistered()
	{
		return isRegistered(TypeIdentifier::fromType<T>());
	}

	template <typename T>
	bool isTransient()
	{
		return isTransient(TypeIdentifier::fromType<T>());
	}

	template <typename T>
	bool isSingleton()
	{
		return isSingleton(TypeIdentifier::fromType<T>());
	}

	template <typename T>
	T* getService()
	{
		return (T*)getService(TypeIdentifier::fromType<T>());
	}

	template <typename T>
	void destroyService(ObjectReference service)
	{
		destroyService(TypeIdentifier::fromType<T>(), service);
	}

	template <typename T>
	void destroyService(T* service)
	{
		destroyService(TypeIdentifier::fromType<T>(), (ObjectReference)service);
	}

	// Non-templated

	bool isRegistered(const TypeIdentifier typeIdentifier)
	{
		return mTypes.find(typeIdentifier) != mTypes.end();
	}

	bool isTransient(const TypeIdentifier typeIdentifier)
	{
		if (isRegistered(typeIdentifier))
		{
			return mTypes.at(typeIdentifier).type == InjectableType::Transient;
		}
		return false;
	}

	bool isSingleton(const TypeIdentifier typeIdentifier)
	{
		if (isRegistered(typeIdentifier))
		{
			return mTypes.at(typeIdentifier).type == InjectableType::Singleton;
		}
		return false;
	}

	ObjectReference getService(const TypeIdentifier& typeIdentifier)
	{
		assertRegistered(typeIdentifier);

		return mTypes.at(typeIdentifier).getInstance(*this);
	}

	void destroyService(const TypeIdentifier typeIdentifier, ObjectReference service)
	{
		assertRegistered(typeIdentifier);

		mTypes.at(typeIdentifier).destroyInstance(*this, service);
	}
};