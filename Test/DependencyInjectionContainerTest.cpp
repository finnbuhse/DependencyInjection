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

class DependentExample1 {
private:

public:
	std::shared_ptr<DependencyExample1> mDependency1;
	std::shared_ptr<DependencyExample2> mDependency2;

	DependentExample1(std::shared_ptr<DependencyExample1> instance1, std::shared_ptr<DependencyExample2> instance2)
		: mDependency1(instance1), mDependency2(instance2) {
		// Initialization code
	}
};

struct Config
{
	int x;
	float y;
};

class DependentExample2 {
private:

public:
	std::shared_ptr<DependencyExample1> mDependency;
	const Config mConfig;

	DependentExample2(Config config, std::shared_ptr<DependencyExample1> dependency) : mConfig(config), mDependency(dependency)
	{
		// Initialization code
	}
};

class DependentExample3 {
private:

public:
	std::shared_ptr<DependencyExample1> mDependency;
	const Config mConfig;

	DependentExample3(std::shared_ptr<DependencyExample1> dependency, const Config& config) : mConfig(config), mDependency(dependency)
	{
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
		container.registerTransient<DependentExample1>();

		// When
		std::shared_ptr<DependentExample1> dependent1 = container.resolve<DependentExample1>();
		std::shared_ptr<DependentExample1> dependent2 = container.resolve<DependentExample1>();

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
		container.registerSingleton<DependentExample1>();

		// When
		std::shared_ptr<DependentExample1> dependent1 = container.resolve<DependentExample1>();
		std::shared_ptr<DependentExample1> dependent2 = container.resolve<DependentExample1>();

		// Then 
		EXPECT_EQ(dependent1.get(), dependent2.get());
	}

	TEST(ContainerCanConstructSingleton, CaseWithCopyableConstructorArguments)
	{
		// Given
		DependencyInjectionContainer container;

		Config myConfig = {};
		myConfig.x = 5;
		myConfig.y = 3.4f;

		container.registerSingleton<Config>(myConfig);
		container.registerSingleton<DependencyExample1>();
		container.registerSingleton<DependentExample2>();

		// When
		std::shared_ptr<DependentExample2> dependentOnConfig = container.resolve<DependentExample2>();

		// Then 
		EXPECT_EQ(dependentOnConfig->mConfig.x, 5);
		EXPECT_EQ(dependentOnConfig->mConfig.y, 3.4f);
	}

	TEST(ContainerCanConstructSingleton, CaseWithConstReferenceConstructorArguments)
	{
		// Given
		// Constructor arguments are the opposite way around in resolved dependent
		// Resolved dependent also recieves a const T& parameter instead of an std::shared_ptr

		DependencyInjectionContainer container;

		Config myConfig = {};
		myConfig.x = 5;
		myConfig.y = 3.4f;

		container.registerSingleton<DependencyExample1>();
		container.registerSingleton<Config>(myConfig);
		container.registerSingleton<DependentExample3>();

		// When
		std::shared_ptr<DependentExample3> dependentOnConfig = container.resolve<DependentExample3>();

		// Then 
		EXPECT_EQ(dependentOnConfig->mConfig.x, 5);
		EXPECT_EQ(dependentOnConfig->mConfig.y, 3.4f);
	}
}