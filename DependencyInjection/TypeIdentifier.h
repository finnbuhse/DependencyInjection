#pragma once
#include <string>
#include <unordered_map>
#include <cassert>

// Forward declaration
class TypeIdentifier;

class TypeIdentifier
{
private:
	size_t mHashCode;

public:
	template <typename Type>
	static size_t hashCode()
	{
		return typeid(Type).hash_code();
	}

	template <typename Type>
	static std::string name()
	{
		return typeid(Type).name();
	}

	template <typename T>
	static TypeIdentifier fromType()
	{
		return TypeIdentifier(hashCode<T>());
	}

	TypeIdentifier() : mHashCode(NULL)
	{

	}

	TypeIdentifier(size_t hashCode) : mHashCode(hashCode)
	{

	}

	TypeIdentifier(const TypeIdentifier& other) : mHashCode(other.mHashCode)
	{

	}

	TypeIdentifier& operator =(const TypeIdentifier& other)
	{
		mHashCode = other.mHashCode;
		return *this;
	}

	size_t hashCode() const
	{
		return mHashCode;
	}

	bool operator ==(const TypeIdentifier& other) const
	{
		return mHashCode == other.mHashCode;
	}
};

// TypeIdentifier hash function
namespace std {
	template <>
	struct hash<TypeIdentifier> {
		size_t operator()(const TypeIdentifier& typeIdentifier) const {
			return typeIdentifier.hashCode();
		}
	};
}

class TypeRegistry
{
private:
	std::unordered_map<std::string, TypeIdentifier> mNameIdentifierMap;

public:
	template <typename Type>
	void registerType()
	{
		mNameIdentifierMap.insert({ TypeIdentifier::name<Type>(), TypeIdentifier::fromType<Type>() });
	}

	TypeIdentifier identifierFromTypeName(const std::string& typeName)
	{
		return mNameIdentifierMap[typeName];
	}

	std::string typeNameFromIdentifier(TypeIdentifier typeIdentifier)
	{
		for (const std::pair<std::string, TypeIdentifier>& element : mNameIdentifierMap)
		{
			if (element.second == typeIdentifier)
			{
				return element.first;
			}
		}

		assert(("[ERROR] Attempted to get type name from TypeIdentifier which has not been registered", false));
		return "";
	}
};

