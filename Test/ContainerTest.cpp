#include "gtest/gtest.h"
#include "Container.h"

namespace ContainerTest
{
	class DependencyExampleDefaultConstructible
	{
	private:
		int a = 0;

	public:
		DependencyExampleDefaultConstructible() {};
	};

	// Setting members overrides inherited constructors
	class ExampleDependent : public Dependent<DependencyExampleDefaultConstructible>
	{
	private:
		int a;

	public:
		ExampleDependent(const std::vector<ObjectReference>& dependencies) : Dependent<DependencyExampleDefaultConstructible>(dependencies), a(0) {}
	};

	// Setting members overrides inherited constructors
	class ExampleDualDependent : public Dependent<ExampleDependent, DependencyExampleDefaultConstructible>
	{
	private:
		int a;

	public:
		ExampleDualDependent(const std::vector<ObjectReference>& dependencies) : Dependent<ExampleDependent, DependencyExampleDefaultConstructible>(dependencies), a(0) {}
	};

	TEST(ContainerCanAddTransient, Case1)
	{
		bool hasTransient;

		// Given
		Container container;

		// When
		hasTransient = container.isTransient<DependencyExampleDefaultConstructible>();

		// Then
		EXPECT_FALSE(hasTransient);

		// When
		container.addTransient<DependencyExampleDefaultConstructible>();
		hasTransient = container.isTransient<DependencyExampleDefaultConstructible>();

		// Then
		EXPECT_TRUE(hasTransient);
	}

	TEST(ContainerCanAddSingleton, Case1)
	{
		bool hasSingleton;

		// Given
		Container container;

		// When
		hasSingleton = container.isSingleton<ExampleDependent>();

		// Then
		EXPECT_FALSE(hasSingleton);

		// When
		container.addSingleton<ExampleDependent>();
		hasSingleton = container.isSingleton<ExampleDependent>();

		// Then
		EXPECT_TRUE(hasSingleton);
	}

	TEST(ContainerCanConstructTransient, DefaultConstructible)
	{
		// Given
		Container container;
		container.addTransient<DependencyExampleDefaultConstructible>();

		try
		{
			// When
			
			DependencyExampleDefaultConstructible* transientReference1 = container.getService<DependencyExampleDefaultConstructible>();
			DependencyExampleDefaultConstructible* transientReference2 = container.getService<DependencyExampleDefaultConstructible>();

			// Then
			EXPECT_NE(transientReference1, transientReference2);
		} 
		catch (std::exception e)
		{
			// Then
			std::cerr << e.what() << std::endl;
			assert(false);
		}
	}

	TEST(ContainerCanConstructTransient, DependentOnTransient)
	{
		// Given
		Container container;
		container.addTransient<DependencyExampleDefaultConstructible>();
		container.addTransient<ExampleDependent>();

		try
		{
			// When
			
			ExampleDependent* transientReference1 = container.getService<ExampleDependent>();
			ExampleDependent* transientReference2 = container.getService<ExampleDependent>();

			DependencyExampleDefaultConstructible* dependencyReference1 = &transientReference1->dependencies.get<0>();
			DependencyExampleDefaultConstructible* dependencyReference2 = &transientReference2->dependencies.get<0>();

			// Then
			EXPECT_NE(transientReference1, transientReference2);
			EXPECT_NE(dependencyReference1, dependencyReference2);
		} 
		catch (std::exception e)
		{
			// Then
			std::cerr << e.what() << std::endl;
			assert(false);
		}
	}

	TEST(ContainerCanConstructTransient, DependentOnSingleton)
	{
		// Given
		Container container;
		container.addSingleton<DependencyExampleDefaultConstructible>();
		container.addTransient<ExampleDependent>();

		try
		{
			// When
			
			ExampleDependent* transientReference1 = container.getService<ExampleDependent>();
			ExampleDependent* transientReference2 = container.getService<ExampleDependent>();

			DependencyExampleDefaultConstructible* dependencyReference1 = &transientReference1->dependencies.get<0>();
			DependencyExampleDefaultConstructible* dependencyReference2 = &transientReference2->dependencies.get<0>();

			// Then
			EXPECT_NE(transientReference1, transientReference2);
			EXPECT_EQ(dependencyReference1, dependencyReference2);
		}
		catch (std::exception e)
		{
			// Then
			std::cerr << e.what() << std::endl;
			assert(false);
		}
	}

	TEST(ContainerCanConstructTransient, DependentOnMultiple)
	{
		// Given
		Container container;
		container.addSingleton<DependencyExampleDefaultConstructible>();
		container.addTransient<ExampleDependent>();
		container.addTransient<ExampleDualDependent>();

		try
		{
			// When
			ExampleDualDependent* transientReference1 = container.getService<ExampleDualDependent>();
			ExampleDualDependent* transientReference2 = container.getService<ExampleDualDependent>();

			// Then
			EXPECT_NE(transientReference1, transientReference2);
		}
		catch (std::exception e)
		{
			// Then
			std::cerr << e.what() << std::endl;
			assert(false);
		}
	}

	TEST(ContainerCanConstructSingleton, DefaultConstructible)
	{
		// Given
		Container container;
		container.addSingleton<DependencyExampleDefaultConstructible>();

		try
		{
			// When
			
			DependencyExampleDefaultConstructible* singletonReference1 = container.getService<DependencyExampleDefaultConstructible>();
			DependencyExampleDefaultConstructible* singletonReference2 = container.getService<DependencyExampleDefaultConstructible>();

			// Then
			EXPECT_EQ(singletonReference1, singletonReference2);

		}
		catch (std::exception e)
		{
			// Then
			std::cerr << e.what() << std::endl;
			assert(false);
		}
	}

	TEST(ContainerCanConstructSingleton, DependentOnTransient)
	{
		// Given
		Container container;
		container.addTransient<DependencyExampleDefaultConstructible>();
		container.addSingleton<ExampleDependent>();
		
		try
		{
			// When
			
			ExampleDependent* singletonReference1 = container.getService<ExampleDependent>();
			ExampleDependent* singletonReference2 = container.getService<ExampleDependent>();

			DependencyExampleDefaultConstructible* dependencyReference1 = &singletonReference1->dependencies.get<0>();
			DependencyExampleDefaultConstructible* dependencyReference2 = &singletonReference2->dependencies.get<0>();

			// Then
			EXPECT_EQ(singletonReference1, singletonReference2);
			EXPECT_EQ(dependencyReference1, dependencyReference2);
		} 
		catch (std::exception e)
		{
			// Then
			std::cerr << e.what() << std::endl;
			assert(false);
		}
	}

	TEST(ContainerCanConstructSingleton, DependentOnSingleton)
	{
		// Given
		Container container;
		container.addSingleton<DependencyExampleDefaultConstructible>();
		container.addSingleton<ExampleDependent>();

		try
		{
			// When
			
			ExampleDependent* singletonReference1 = container.getService<ExampleDependent>();
			ExampleDependent* singletonReference2 = container.getService<ExampleDependent>();

			DependencyExampleDefaultConstructible* dependencyReference1 = &singletonReference1->dependencies.get<0>();
			DependencyExampleDefaultConstructible* dependencyReference2 = &singletonReference2->dependencies.get<0>();

			// Then
			EXPECT_EQ(singletonReference1, singletonReference2);
			EXPECT_EQ(dependencyReference1, dependencyReference2);
		}
		catch (std::exception e)
		{
			// Then
			std::cerr << e.what() << std::endl;
			assert(false);
		}
	}

	TEST(ContainerCanConstructSingleton, DependentOnMultiple)
	{
		// Given
		Container container;
		container.addSingleton<DependencyExampleDefaultConstructible>();
		container.addTransient<ExampleDependent>();
		container.addSingleton<ExampleDualDependent>();

		try
		{
			// When
			ExampleDualDependent* singletonReference1 = container.getService<ExampleDualDependent>();
			ExampleDualDependent* singletonReference2 = container.getService<ExampleDualDependent>();

			// Then
			EXPECT_EQ(singletonReference1, singletonReference2);
		}
		catch (std::exception e)
		{
			// Then
			std::cerr << e.what() << std::endl;
			assert(false);
		}
	}
}