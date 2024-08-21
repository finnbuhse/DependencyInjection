#include "Reflection.h"
#include "gtest/gtest.h"

class A {
public:
    int x;
    double y;

    A(int _x, double _y) : x(_x), y(_y) {}
};

TEST(ConstructorArgumentsAsTupleTypeTest, Case1)
{
    // When
    using ArgsA = refl::constructor_arguments_as_tuple_type<A>;

    // Then
    static_assert(std::is_same<ArgsA, std::tuple<int, double>>::value, "Constructor arguments of A do not match!");
}

TEST(ConstructFromTupleTest, Case1)
{
    // Given
    std::tuple<int, double> tuple(5, 3.4);

    // When
    A a = constructFromTuple<A>(tuple);

    // Then
    EXPECT_EQ(a.x, 5);
    EXPECT_EQ(a.y, 3.4);
}