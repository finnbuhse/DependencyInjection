#include "Reflection.h"
#include "gtest/gtest.h"

namespace ReflectionTest
{
    class ClassExample {
    public:
        int x;
        double y;

        ClassExample(int _x, double _y) : x(_x), y(_y) {}
    };

    TEST(ConstructorArgumentsAsTupleTypeTest, Case1)
    {
        // When
        using ClassArgs = refl::constructor_arguments_as_tuple_type<ClassExample>;

        // Then
        static_assert(std::is_same<ClassArgs, std::tuple<int, double>>::value, "Constructor arguments of A do not match!");
    }

    TEST(ConstructFromTupleTest, Case1)
    {
        // Given
        std::tuple<int, double> tuple(5, 3.4);

        // When
        ClassExample myClass = constructFromTuple<ClassExample>(tuple);

        // Then
        EXPECT_EQ(myClass.x, 5);
        EXPECT_EQ(myClass.y, 3.4);
    }
}