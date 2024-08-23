#include "Reflection.h"
#include <unordered_map>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <stdexcept>

// TODO:
// Thread Safety : Ensure thread safety for creating and accessing mSingletons.
// Error Messaging : Improve error messages for easier debugging when resolving unregistered types.
// Documentation : Enhance comments and documentation for better readability and maintainability.

/*
template<typename Tuple, typename Condition, std::size_t Index = 0>
struct are_all_tuple_elements {
    static constexpr bool value = Index == std::tuple_size<Tuple>::value ||
        (Condition<std::tuple_element_t<Index, Tuple>>::value &&
            are_all_tuple_elements<Tuple, Condition, Index + 1>::value);
};
*/

/*
Dependency injection container for classes that have std::shared_pointer<T> dependencies specified in their constructors.
The order dependencies are constructed depends on when they are required.
If multiple services are required by one class, they are constructed in the same order
as the order they appear in within the constructor of the class.
*/
class DependencyInjectionContainer {
public:
    /*
    Registers a type that can be injected, 
    or optionally an interface type can be specified and registered, which gets injected with the true type.
    Note: If a singleton of the specified type is already present, the reference to it will be dropped, and a new singleton will be instantiated.
    */
    template <typename InterfaceType, typename TrueType = InterfaceType>
    void registerSingleton()
    {
        mSingletons[std::type_index(typeid(InterfaceType))] = nullptr; // Reset the singleton instance
        mFactories[std::type_index(typeid(InterfaceType))] = [this]() -> std::shared_ptr<void> 
        { // Instantiate when needed
            std::shared_ptr<void>& instance = mSingletons[std::type_index(typeid(InterfaceType))];
            if (!instance) {
                instance = createInstance<TrueType>();
            }
            return std::static_pointer_cast<void>(instance);
        };
    }

    template <typename InterfaceType, typename TrueType = InterfaceType>
    void registerSingleton(std::shared_ptr<TrueType> singleton)
    {
        mSingletons[std::type_index(typeid(InterfaceType))] = std::static_pointer_cast<void>(singleton); // Assign singleton instance
        mFactories[std::type_index(typeid(InterfaceType))] = [this, singleton]() -> std::shared_ptr<void>
        {
            return std::static_pointer_cast<void>(singleton);
        };
    }

    /*
    For registering a primitive type.
    Note: The specified type must be copy constructible.
    */ 
    template <typename InterfaceType, typename TrueType = InterfaceType>
    void registerSingleton(const TrueType& singleton)
    {
        mSingletons[std::type_index(typeid(InterfaceType))] = nullptr; // Reset the singleton instance
        mFactories[std::type_index(typeid(InterfaceType))] = [this, singleton]() -> std::shared_ptr<void>
        {
            // Instantiate when needed
            std::shared_ptr<void>& instance = mSingletons[std::type_index(typeid(InterfaceType))];
            if (!instance) {
                instance = std::make_shared<TrueType>(singleton);
            }
            return std::static_pointer_cast<void>(instance);
        };
    }

    /*
    Registers an interface type for which the true type is injected.
    */
    template <typename InterfaceType, typename TrueType = InterfaceType>
    void registerTransient()
    {
        mFactories[std::type_index(typeid(InterfaceType))] = [this]() -> std::shared_ptr<void> {
            std::shared_ptr<InterfaceType> instance = createInstance<TrueType>();
            return (std::shared_ptr<void>)instance;
        };
    }

    template <typename T>
    std::shared_ptr<T> resolve()
    {
        std::unordered_map<std::type_index, std::function<std::shared_ptr<void>()>>::const_iterator it = mFactories.find(std::type_index(typeid(T)));
        if (it != mFactories.end()) {
            std::shared_ptr<T> ptr = std::static_pointer_cast<T>(it->second());
            return ptr;
        }
        throw std::runtime_error("Attempted to resolve type that has not been registered.");
    }

    template <typename T>
    bool isRegistered()
    {
        return mFactories.contains(std::type_index(typeid(T)));
    }

private:
    std::unordered_map<std::type_index, std::function<std::shared_ptr<void>()>> mFactories;
    std::unordered_map<std::type_index, std::shared_ptr<void>> mSingletons;

    /*
    Resolves each constructor argument within the tuple.
    */
    template <std::size_t I = 0, typename... Ts>
    typename std::enable_if<I == sizeof...(Ts), void>::type resolveArgumentTuple(std::tuple<Ts...>&) {}

    template <std::size_t I = 0, typename... Ts>
    typename std::enable_if<I < sizeof...(Ts), void>::type resolveArgumentTuple(std::tuple<Ts...>& t)
    {
        using ArgTypeRaw = std::tuple_element_t<I, std::tuple<Ts...>>;
        
        // Resolve the argument using the inner type of the std::shared_ptr type
        if constexpr (is_shared_ptr<ArgTypeRaw>::value)
        {
            // If constructor argument type is an std::shared_ptr resolve as normal
            using ArgType = std::tuple_element_t<I, std::tuple<Ts...>>::element_type;
            std::get<I>(t) = resolve<ArgType>();
        }
        else
        {
            // Otherwise, resolve and dereference pointer
            std::get<I>(t) = *resolve<ArgTypeRaw>().get();
        }

        // Resolve remaining arguments
        resolveArgumentTuple<I + 1, Ts...>(t);
    }

    /*
    Creates instance by resolving constructor arguments.
    */ 
    template<typename T>
    std::shared_ptr<T> createInstance() {
        refl::constructor_arguments_as_tuple_type<T> tuple{};
        resolveArgumentTuple(tuple);
        return makeSharedFromTuple<T>(tuple);
    }
};
