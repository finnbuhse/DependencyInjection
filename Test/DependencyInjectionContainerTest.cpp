#include "DependencyInjectionContainer.h"
#include "Reflection.h"
#include "gtest/gtest.h"

// Test classes

class DependencyExample1 {
public:
	DependencyExample1(/* params */) {
		// Initialization code
	}
};

class DependencyExample2 {
public:
	DependencyExample2() {
		// Initialization code
	}
};

class DependentExample {
private:

public:
	std::shared_ptr<DependencyExample1> mDependency1;
	std::shared_ptr<DependencyExample2> mDependency2;

	DependentExample(std::shared_ptr<DependencyExample1> instance1, std::shared_ptr<DependencyExample2> instance2)
		: mDependency1(instance1), mDependency2(instance2) {
		// Initialization code
	}
};

// Tests

namespace DependencyInjectionContainerTest
{
	TEST(ContainerCanRegisterType, CaseTransient)
	{
		// Given
		DependencyInjectionContainer container;

		// When
		container.registerTransient<DependencyExample1>();

		// Then 
		EXPECT_TRUE(container.isRegistered<DependencyExample1>());
		EXPECT_FALSE(container.isRegistered<DependencyExample2>());
	}

	TEST(ContainerCanRegisterType, CaseSingleton)
	{
		// Given
		DependencyInjectionContainer container;

		// When
		container.registerSingleton<DependencyExample1>();

		// Then 
		EXPECT_TRUE(container.isRegistered<DependencyExample1>());
		EXPECT_FALSE(container.isRegistered<DependencyExample2>());
	}

	TEST(ContainerCanConstructTransient, Case1)
	{
		// Given
		DependencyInjectionContainer container;
		container.registerTransient<DependencyExample1>();
		container.registerTransient<DependencyExample2>();
		container.registerTransient<DependentExample>();

		// When
		std::shared_ptr<DependentExample> dependent1 = container.resolve<DependentExample>();
		std::shared_ptr<DependentExample> dependent2 = container.resolve<DependentExample>();

		std::shared_ptr<DependencyExample1> dependent1Dependency1 = dependent1->mDependency1;
		std::shared_ptr<DependencyExample2> dependent1Dependency2 = dependent1->mDependency2;

		std::shared_ptr<DependencyExample1> dependent2Dependency1 = dependent2->mDependency1;
		std::shared_ptr<DependencyExample2> dependent2Dependency2 = dependent2->mDependency2;

		// Then 
		EXPECT_NE(dependent1.get(), dependent2.get());
		EXPECT_NE(dependent1Dependency1.get(), dependent2Dependency1.get());
		EXPECT_NE(dependent1Dependency2.get(), dependent2Dependency2.get());
	}

	TEST(ContainerCanConstructSingleton, Case1)
	{
		// Given
		DependencyInjectionContainer container;
		container.registerTransient<DependencyExample1>();
		container.registerTransient<DependencyExample2>();
		container.registerSingleton<DependentExample>();

		// When
		std::shared_ptr<DependentExample> dependent1 = container.resolve<DependentExample>();
		std::shared_ptr<DependentExample> dependent2 = container.resolve<DependentExample>();

		// Then 
		EXPECT_EQ(dependent1.get(), dependent2.get());
	}
}