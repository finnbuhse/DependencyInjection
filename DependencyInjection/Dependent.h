#pragma once
#include "TypeIdentifier.h"
#include "ObjectStorage.h"
#include <tuple>
#include <vector>
#include <functional>

class IDependencySet
{
public:

};

template <typename... DEPENDS>
class DependencySet : public IDependencySet
{
public:
	static constexpr unsigned int count = sizeof...(DEPENDS);

protected:
	std::tuple<DEPENDS*...> mDependencies;

	template <size_t DEPEND_INDEX>
	using DependencyPointerType = std::tuple_element<DEPEND_INDEX, std::tuple<DEPENDS*...>>::type;

	#pragma region STATIC Get type identifiers
	template <size_t DEPEND_INDEX>
	static void getTypeIdentifiers(std::vector<TypeIdentifier>& outputIdentifiers)
	{
		if constexpr (DEPEND_INDEX < count)
		{
			outputIdentifiers[DEPEND_INDEX] = TypeIdentifier::fromType<DependencyType<DEPEND_INDEX>>();

			getTypeIdentifiers<DEPEND_INDEX + 1>(outputIdentifiers);
		}
	}
	#pragma endregion

	#pragma region Set dependency references from parameters
	template <size_t DEPEND_INDEX, typename T>
	void setDependencyReferences(T& dependency)
	{
		T* dependencyPointer = &dependency;
		std::get<DEPEND_INDEX>(mDependencies) = (DependencyPointerType<DEPEND_INDEX>)dependencyPointer;
	}

	template <size_t DEPEND_INDEX, typename T, typename... Ts>
	void setDependencyReferences(T& dependency, Ts&... dependencies)
	{
		if constexpr (DEPEND_INDEX < count)
		{
			T* dependencyPointer = &dependency;
			std::get<DEPEND_INDEX>(mDependencies) = (DependencyPointerType<DEPEND_INDEX>)dependencyPointer;

			setDependencyReferences<DEPEND_INDEX + 1>(dependencies...);
		}
	}

	template <size_t DEPEND_INDEX, typename... Ts>
	void setDependencyReferences(Ts&... dependencies)
	{
		setDependencyReferences<DEPEND_INDEX>(dependencies...);
	}
	#pragma endregion

	#pragma region Set dependency references from vector
	template <size_t DEPEND_INDEX>
	void setDependencyReferencesFromVector(const std::vector<ObjectReference>& dependencies)
	{
		if constexpr (DEPEND_INDEX < count)
		{
			std::get<DEPEND_INDEX>(mDependencies) = (DependencyPointerType<DEPEND_INDEX>)dependencies[DEPEND_INDEX];

			setDependencyReferencesFromVector<DEPEND_INDEX + 1>(dependencies);
		}
	}
	#pragma endregion

	#pragma region Set dependency references from other
	template <size_t DEPEND_INDEX>
	void setDependencyReferencesFromOther(DependencySet<DEPENDS...>& other)
	{
		if constexpr (DEPEND_INDEX < count)
		{
			std::get<DEPEND_INDEX>(mDependencies) = other.get<DEPEND_INDEX>();

			setDependencyReferencesFromOther<DEPEND_INDEX + 1>(other);
		}
	}
	#pragma endregion

	#pragma region Get dependency pointers
	template <size_t DEPEND_INDEX>
	void getDependencyPointers(std::vector<ObjectReference>& outputPointers) const
	{
		if constexpr (DEPEND_INDEX < count)
		{
			outputPointers[DEPEND_INDEX] = (ObjectReference)std::get<DEPEND_INDEX>(mDependencies);

			getDependencyPointers<DEPEND_INDEX + 1>(outputPointers);
		}
	}
	#pragma endregion

public:
	template <size_t DEPEND_INDEX>
	using DependencyType = std::tuple_element<DEPEND_INDEX, std::tuple<DEPENDS...>>::type;

	static std::vector<TypeIdentifier> typeIdentifiers()
	{
		std::vector<TypeIdentifier> result(count);
		getTypeIdentifiers<0>(result);
		return result;
	}

	DependencySet(DEPENDS&... dependencies)
	{
		setDependencyReferences<0>(dependencies...);
	}

	DependencySet(const std::vector<ObjectReference>& dependencies)
	{
		setDependencyReferencesFromVector<0>(dependencies);
	}

	DependencySet(DependencySet<DEPENDS...>& other)
	{
		setDependencyReferencesFromOther<0>(other);
	}

	std::vector<ObjectReference> dependencyPointers() const
	{
		std::vector<ObjectReference> result(count);
		getDependencyPointers<0>(result);
		return result;
	}

	template <size_t DEPEND_INDEX>
	DependencyType<DEPEND_INDEX>& get() const
	{
		return *std::get<DEPEND_INDEX>(mDependencies);
	}

	ObjectReference get(const unsigned int i)
	{
		return dependencyPointers()[i];
	}
};

class IDependent
{

};

template <typename... DEPENDS>
class Dependent : public IDependent
{
private:
	

public:
	using DependenciesType = DependencySet<DEPENDS...>;

	DependenciesType dependencies;

	static std::vector<TypeIdentifier> dependencyTypeIdentifiers()
	{
		return DependenciesType::typeIdentifiers();
	}

	Dependent(DEPENDS&... _dependencies) : dependencies(_dependencies...)
	{

	}

	Dependent(DependencySet<DEPENDS...>& _dependencies) : dependencies(_dependencies)
	{

	}

	Dependent(const std::vector<ObjectReference>& _dependencies) : dependencies(_dependencies)
	{
		
	}

	std::vector<ObjectReference> dependencyPointers() const
	{
		return dependencies.dependencyPointers();
	}

	template <size_t DEPEND_INDEX>
	DependencySet<DEPENDS...>::DependencyType<DEPEND_INDEX>& dependency() const
	{
		return dependencies.get<DEPEND_INDEX>();
	}

	ObjectReference dependency(const unsigned int i)
	{
		return dependencies.get(i);
	}
};