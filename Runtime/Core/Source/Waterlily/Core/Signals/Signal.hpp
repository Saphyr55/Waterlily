#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Function/Function.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Signals/Connection.hpp"

#define WL_SIGNAL_DETAILS(keyword, name, ...)            \
    using name##_signal_t = ::Wl::Signal<__VA_ARGS__>; \
    keyword name##_signal_t name

#define WL_SIGNAL_STATIC(name, ...) WL_SIGNAL_DETAILS(inline static, name, __VA_ARGS__)
#define WL_SIGNAL_MEMBER(name, ...) WL_SIGNAL_DETAILS(mutable, name, __VA_ARGS__)
#define WL_SIGNAL(name, ...) WL_SIGNAL_MEMBER(name, __VA_ARGS__)

namespace Wl
{

    template<typename... ArgumentTypes>
    class Signal
    {
    public:
        using ConnectionType = Connection<ArgumentTypes...>;

    public:
        template<typename Callable>
        ConnectionType Connect(Callable&& callable);

        template<typename ObjectType>
        ConnectionType Connect(ObjectType* object, void (ObjectType::*method)(ArgumentTypes...));

        template<typename ObjectType>
        ConnectionType Connect(SharedPtr<ObjectType> object, void (ObjectType::*method)(ArgumentTypes...));

        void Emit(ArgumentTypes... args);

        void Disconnect(size_t id);

        void DisconnectAll();

        size_t GetConnectionCount() const;

        bool IsEmpty() const;

        // Emit
        void operator()(ArgumentTypes... args);

    public:
        Signal() = default;
        ~Signal() = default;

        Signal(const Signal&) = delete;
        Signal& operator=(const Signal&) = delete;
        Signal(Signal&& other) noexcept = default;
        Signal& operator=(Signal&& other) noexcept = default;

    private:
        void Clean();
        friend class Connection<ArgumentTypes...>;

    private:
        size_t m_nextID = 1;
        HashMap<size_t, Function<void(ArgumentTypes...)>> m_handlers;
        Array<size_t> m_pendingRemovals;
        bool m_isEmitting = false;
    };

    template<typename... ArgumentTypes>
    template<typename Callable>
    auto Signal<ArgumentTypes...>::Connect(Callable&& callable) -> ConnectionType
    {
        size_t id = m_nextID++;
        m_handlers[id] = [callable = std::move(callable)](ArgumentTypes... args) mutable -> void
        {
            callable(std::forward<ArgumentTypes>(args)...);
        };
        return ConnectionType(id, this);
    }

    template<typename... ArgumentTypes>
    template<typename ObjectType>
    auto Signal<ArgumentTypes...>::Connect(ObjectType* object, void (ObjectType::*method)(ArgumentTypes...)) -> ConnectionType
    {
        auto callback = [object, method](ArgumentTypes... args)
        {
            if (object)
            {
                (object->*method)(args...);
            }
        };
        return Connect(callback);
    }

    template<typename... ArgumentTypes>
    template<typename ObjectType>
    auto Signal<ArgumentTypes...>::Connect(SharedPtr<ObjectType> object, void (ObjectType::*method)(ArgumentTypes...))
            -> ConnectionType
    {
        return Connect(object.GetResource(), method);
    }

    template<typename... ArgumentTypes>
    void Signal<ArgumentTypes...>::Emit(ArgumentTypes... args)
    {
        if (m_handlers.IsEmpty())
        {
            return;
        }

        m_isEmitting = true;

        for (auto [id, handler]: m_handlers)
        {
            handler(std::forward<ArgumentTypes>(args)...);
        }

        m_isEmitting = false;
        Clean();
    }

    template<typename... ArgumentTypes>
    void Signal<ArgumentTypes...>::operator()(ArgumentTypes... args)
    {
        Emit(std::forward<ArgumentTypes>(args)...);
    }

    template<typename... Args>
    void Signal<Args...>::Disconnect(size_t id)
    {
        if (m_isEmitting)
        {
            m_pendingRemovals.Append(id);
            return;
        }

        m_handlers.Remove(id);
    }

    template<typename... Args>
    void Signal<Args...>::DisconnectAll()
    {
        if (!m_isEmitting)
        {
            m_handlers.Clear();
            return;
        }

        for (auto [id, _]: m_handlers)
        {
            m_pendingRemovals.Append(id);
        }
    }

    template<typename... Args>
    void Signal<Args...>::Clean()
    {
        for (size_t id: m_pendingRemovals)
        {
            m_handlers.Remove(id);
        }
        m_pendingRemovals.Clear();
    }

    template<typename... Args>
    size_t Signal<Args...>::GetConnectionCount() const
    {
        return m_handlers.GetSize();
    }

    template<typename... Args>
    bool Signal<Args...>::IsEmpty() const
    {
        return m_handlers.IsEmpty();
    }

}// namespace Wl