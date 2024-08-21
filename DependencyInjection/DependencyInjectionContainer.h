#include "Reflection.h"
#include <unordered_map>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <stdexcept>

enum class Lifetime {
    TRANSIENT,
    SINGLETON
};

class DependencyInjectionContainer {
public:
    template <typename T>
    void registerType(Lifetime scope)
    {
        mFactories[std::type_index(typeid(T))] = [this, scope]() -> std::shared_ptr<void> {
            if (scope == Lifetime::SINGLETON) {
                static std::shared_ptr<T> instance = createInstance<T>();
                std::shared_ptr<void> p = instance;
                return p;
            }
            else if (scope == Lifetime::TRANSIENT)
            {
                std::shared_ptr<T> instance = createInstance<T>();
                std::shared_ptr<void> p = instance;
                return p;
            }
            return nullptr;
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
        // Assuming tuple element is an std::shared_ptr
        using ArgTypePtr = std::tuple_element_t<I, std::tuple<Ts...>>; 
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
