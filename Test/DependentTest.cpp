#include "gtest/gtest.h"
#include "Dependent.h"

namespace DependentTest
{
	class DependencyExample
	{
	private:
		int a = 0;

	public:
		DependencyExample() {}
	};

	class DependentExample : public Dependent<DependencyExample>
	{
	public:
		DependentExample(const std::vector<ObjectReference>& dependencies) : Dependent<DependencyExample>(dependencies) {} // Must be implemented

		DependentExample(DependencyExample& dependency) : Dependent<DependencyExample>(dependency) {}
	};

	TEST(DependencyTypeIdentifiersReturnsCorrect, Case1)
	{
		std::vector<TypeIdentifier> expected = { TypeIdentifier::fromType<DependencyExample>() };

		// Given

		// When
		std::vector<TypeIdentifier> actual = DependentExample::dependencyTypeIdentifiers();

		// Then
		EXPECT_EQ(expected[0], actual[0]);
	}

	TEST(GetDependencyReturnsCorrect, StandardConstructorTemplatedCall)
	{
		// Given
		DependencyExample dependency;
		ObjectReference expected = (ObjectReference)&dependency;

		DependentExample dependent(dependency);

		// When
		ObjectReference actual = (ObjectReference)&dependent.dependency<0>();

		// Then
		EXPECT_EQ(expected, actual);
	}

	TEST(GetDependencyReturnsCorrect, StandardConstructorParameterisedCall)
	{
		// Given
		DependencyExample dependency;
		ObjectReference expected = (ObjectReference)&dependency;

		DependentExample dependent(dependency);

		// When
		ObjectReference actual = dependent.dependency(0);

		// Then
		EXPECT_EQ(expected, actual);
	}

	TEST(GetDependencyReturnsCorrect, DependencyVectorConstructorTemplatedCall)
	{
		// Given
		DependencyExample dependency;
		ObjectReference expected = (ObjectReference)&dependency;

		DependentExample dependent({ (ObjectReference)&dependency });

		// When
		ObjectReference actual = (ObjectReference)&dependent.dependency<0>();

		// Then
		EXPECT_EQ(expected, actual);
	}

	TEST(GetDependencyReturnsCorrect, DependencyVectorConstructorParameterisedCall)
	{
		// Given
		DependencyExample dependency;
		ObjectReference expected = (ObjectReference)&dependency;

		DependentExample dependent({ (ObjectReference)&dependency });

		// When
		ObjectReference actual = dependent.dependency(0);

		// Then
		EXPECT_EQ(expected, actual);
	}
}