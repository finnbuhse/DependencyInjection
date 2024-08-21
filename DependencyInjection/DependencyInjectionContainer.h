#include "Reflection.h"
#include <unordered_map>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <stdexcept>

// TODO:
// Singleton Reference Management : Review the management of static singleton instances to avoid unexpected behavior with their lifecycle.
// Thread Safety : Ensure thread safety for creating and accessing singletons.
// Error Messaging : Improve error messages for easier debugging when resolving unregistered types.
// Documentation : Enhance comments and documentation for better readability and maintainability.

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
        mFactories[std::type_index(typeid(InterfaceType))] = [this]() -> std::shared_ptr<void> {
            static std::shared_ptr<InterfaceType> instance = nullptr;
            instance = createInstance<TrueType>();
            return (std::shared_ptr<void>)instance;
        };
    }

    template <typename InterfaceType, typename TrueType = InterfaceType>
    void registerSingleton(std::shared_ptr<TrueType> singleton)
    {
        mFactories[std::type_index(typeid(InterfaceType))] = [this, singleton]() -> std::shared_ptr<void> {
            static std::shared_ptr<InterfaceType> instance = nullptr;
            instance = singleton;
            return (std::shared_ptr<void>)instance;
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

    /*
    Resolves each constructor argument within the tuple.
    */
    template <std::size_t I = 0, typename... Ts>
    typename std::enable_if<I == sizeof...(Ts), void>::type resolveArgumentTuple(std::tuple<Ts...>&) {}

    template <std::size_t I = 0, typename... Ts>
    typename std::enable_if<I < sizeof...(Ts), void>::type resolveArgumentTuple(std::tuple<Ts...>& t)
    {
        // Tuple element must be an std::shared_ptr
        using ArgTypePtr = std::tuple_element_t<I, std::tuple<Ts...>>;
        static_assert(is_shared_ptr<ArgTypePtr>::value, "Constructor argument must be an std::shared_ptr referencing any type");
        using ArgType = std::tuple_element_t<I, std::tuple<Ts...>>::element_type;
        
        // Resolve the argument using the inner type of the std::shared_ptr type
        std::get<I>(t) = resolve<ArgType>();

        // Resolve remaining arguments
        resolveArgumentTuple<I + 1, Ts...>(t);
    }

    /*
    Creates instance by resolving constructor arguments.
    */ 
    template<typename T>
    std::shared_ptr<T> createInstance() {
        refl::constructor_arguments_as_tuple_type<T> tuple{}; // std::tuple<std::shared_ptr<>, std::shared_ptr<>, ...>
        resolveArgumentTuple(tuple);
        return makeSharedFromTuple<T>(tuple);
    }
};
