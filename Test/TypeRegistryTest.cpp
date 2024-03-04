#include "gtest/gtest.h"
#include "TypeIdentifier.h"

namespace TypeRegistryTest
{
	class ExampleType
	{
	private:
		int a = 0;

	public:
		ExampleType()
		{

		}
	};

	TEST(IdentifierTypeNameReturnsCorrect, Case1)
	{
		std::string expected = TypeIdentifier::name<ExampleType>();

		// Given
		TypeRegistry registry;
		registry.registerType<ExampleType>();

		TypeIdentifier exampleTypeIdentifier = TypeIdentifier::fromType<ExampleType>();

		// When
		std::string actual = registry.typeNameFromIdentifier(exampleTypeIdentifier);

		// Then
		EXPECT_EQ(expected, actual);
	}

	TEST(TypeNameIdentifierReturnsCorrect, Case1)
	{
		TypeIdentifier expected = TypeIdentifier::fromType<ExampleType>();

		// Given
		TypeRegistry registry;
		registry.registerType<ExampleType>();

		std::string typeName = TypeIdentifier::name<ExampleType>();

		// When
		TypeIdentifier actual = registry.identifierFromTypeName(typeName);

		// Then
		EXPECT_EQ(expected, actual);
	}
}