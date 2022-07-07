#pragma once

#include <string>
#include <functional>
#include <memory>

namespace yy
{
    // create class T with arguments
    template <typename T, typename... Args>
    class Factory
    {
        using Ctor = std::function<std::unique_ptr<T>(Args...)>;

    public:
        // register ctor for DerivedT with name
        template <typename DerivedT>
        void registerType(std::string name)
        {
            static_assert(std::is_base_of<T, DerivedT>::value,
                          "must provide a derived class");

            if (ctors_.find(name) != ctors_.end())
            {
                throw std::invalid_argument("registered same type [" + name + "] twice!");
            }

            Ctor ctor = [](Args &&...args) -> std::unique_ptr<T> {
                return std::make_unique<DerivedT>(std::forward<Args>(args)...);
            };
            ctors_.emplace(name, ctor);
        }

        std::unique_ptr<T> create(std::string name, Args &&...args) const
        {
            auto iter = ctors_.find(name);
            if (iter == ctors_.end())
            {
                throw std::invalid_argument("type [" + name + "] is never registered!");
            }
            return iter->second(std::forward<Args>(args)...);
        }

    private:
        std::unordered_map<std::string, Ctor> ctors_;
    };

    template <typename T, typename... Args>
    class StaticFactory
    {
    public:
        template <typename DerivedT>
        struct Registrar
        {
            explicit Registrar(std::string const &name)
            {
                factory().template registerType<DerivedT>(name);
            }
        };

        static auto create(std::string name, Args &&...args)
        {
            return factory().create(name, std::forward<Args>(args)...);
        }

        static auto &factory()
        {
            static Factory<T, Args...> factory;
            return factory;
        }
    };

// "FACTORY" can have namespace, but "CLASS" can not
// this effectively call the constructor of StaticFactory::Registrar
#define REGISTER_OBJECT(FACTORY, CLASS)                                 \
    inline static FACTORY::Registrar<CLASS> &___get##CLASS##Registrar() \
    {                                                                   \
        static FACTORY::Registrar<CLASS> registrar(#CLASS);             \
        return registrar;                                               \
    }                                                                   \
    inline static FACTORY::Registrar<CLASS> &___##CLASS##Registrar = ___get##CLASS##Registrar()
} // namespace yy
