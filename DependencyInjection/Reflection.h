#pragma once
#include <tuple>
#include <type_traits>
#include <utility>
#include <memory>

namespace refl {

    // --== Function arguments as tuple type ==--

    template <typename T>
    struct function_traits;

    // Specialization for regular functions
    template <typename Ret, typename... Args>
    struct function_traits<Ret(Args...)> {
        using return_type = Ret;
        using argument_types = std::tuple<Args...>;
    };

    // Specialization for member functions
    template <typename Ret, typename ClassType, typename... Args>
    struct function_traits<Ret(ClassType::*)(Args...)> {
        using return_type = typename Ret;
        using argument_types = typename std::tuple<Args...>;
    };

    // Specialization for const member functions
    template <typename Ret, typename ClassType, typename... Args>
    struct function_traits<Ret(ClassType::*)(Args...) const> {
        using return_type = typename Ret;
        using argument_types = typename std::tuple<Args...>;
    };

    template <typename T>
    using function_arguments_as_tuple_type = typename function_traits<T>::argument_types;

    // --== Constructor arguments as tuple type ==--
    // 
    // From Stack Overflow answer by Deni, available at: https://stackoverflow.com/questions/41623899/c-obtaining-the-type-of-a-constructor

    // Based on
    // * http://alexpolt.github.io/type-loophole.html
    //   https://github.com/alexpolt/luple/blob/master/type-loophole.h
    //   by Alexandr Poltavsky, http://alexpolt.github.io
    // * https://www.youtube.com/watch?v=UlNUNxLtBI0
    //   Better C++14 reflections - Antony Polukhin - Meeting C++ 2018

    // tag<T, N> generates friend declarations and helps with overload resolution.
    // There are two types: one with the auto return type, which is the way we read types later.
    // The second one is used in the detection of instantiations without which we'd get multiple
    // definitions.
    template <typename T, int N>
    struct tag {
        friend auto loophole(tag<T, N>);
        constexpr friend int cloophole(tag<T, N>);
    };

    // The definitions of friend functions.
    template <typename T, typename U, int N, bool B,
        typename = typename std::enable_if_t<
        !std::is_same_v<
        std::remove_cv_t<std::remove_reference_t<T>>,
        std::remove_cv_t<std::remove_reference_t<U>>>>>
        struct fn_def {
        friend auto loophole(tag<T, N>) { return U{}; }
        constexpr friend int cloophole(tag<T, N>) { return 0; }
    };

    // This specialization is to avoid multiple definition errors.
    template <typename T, typename U, int N> struct fn_def<T, U, N, true> {};

    // This has a templated conversion operator which in turn triggers instantiations.
    // Important point, using sizeof seems to be more reliable. Also default template
    // arguments are "cached" (I think). To fix that I provide a U template parameter to
    // the ins functions which do the detection using constexpr friend functions and SFINAE.
    template <typename T, int N>
    struct c_op {
        template <typename U, int M>
        static auto ins(...) -> int;
        template <typename U, int M, int = cloophole(tag<T, M>{}) >
        static auto ins(int) -> char;

        template <typename U, int = sizeof(fn_def<T, U, N, sizeof(ins<U, N>(0)) == sizeof(char)>)>
        operator U();
    };

    // Here we detect the data type field number. The byproduct is instantiations.
    // Uses list initialization. Won't work for types with user-provided constructors.
    // In C++17 there is std::is_aggregate which can be added later.
    template <typename T, int... Ns>
    constexpr int fields_number(...) { return sizeof...(Ns) - 1; }

    template <typename T, int... Ns>
    constexpr auto fields_number(int) -> decltype(T{ c_op<T, Ns>{}... }, 0) {
        return fields_number<T, Ns..., sizeof...(Ns)>(0);
    }

    // Here is a version of fields_number to handle user-provided constructor.
    // NOTE: It finds the first constructor with the shortest unambigious set
    //       of parameters.
    template <typename T, int... Ns>
    constexpr auto fields_number_ctor(int) -> decltype(T(c_op<T, Ns>{}...), 0) {
        return sizeof...(Ns);
    }

    template <typename T, int... Ns>
    constexpr int fields_number_ctor(...) {
        return fields_number_ctor<T, Ns..., sizeof...(Ns)>(0);
    }

    // This is a helper to turn a constructor into a tuple type.
    template <typename T, typename U> struct loophole_tuple;

    template <typename T, int... Ns>
    struct loophole_tuple<T, std::integer_sequence<int, Ns...>> {
        using type = std::tuple<decltype(loophole(tag<T, Ns>{}))... > ;
    };

    template <typename T>
    using constructor_arguments_as_tuple_type =
        typename loophole_tuple<T, std::make_integer_sequence<int, fields_number_ctor<T>(0)>>::type;
}

// -== Construct from tuple ==--
// 
// From Stack Overflow answer by Barry, available at : https://stackoverflow.com/questions/38440844/constructor-arguments-from-a-tuples

template <class T, class Tuple, size_t... Is>
T constructFromTuple(Tuple&& tuple, std::index_sequence<Is...>) {
    return T{ std::get<Is>(std::forward<Tuple>(tuple))... };
}

template <class T, class Tuple>
T constructFromTuple(Tuple&& tuple) {
    return constructFromTuple<T>(std::forward<Tuple>(tuple),
        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{}
    );
}

template <class T, class Tuple, size_t... Is>
std::shared_ptr<T> makeSharedFromTuple(Tuple&& tuple, std::index_sequence<Is...>) {
    return std::make_shared<T>(std::get<Is>(std::forward<Tuple>(tuple))...);
}

template <class T, class Tuple>
std::shared_ptr<T> makeSharedFromTuple(Tuple&& tuple) {
    return makeSharedFromTuple<T>(std::forward<Tuple>(tuple),
        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{}
    );
}