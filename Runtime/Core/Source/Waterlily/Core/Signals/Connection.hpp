#pragma once

#include "Waterlily/Core/CoreExports.hpp"

namespace Wl
{

    template<typename... ArgumentTypes>
    class Signal;

    template<typename... ArgumentTypes>
    class Connection
    {
    public:
        void Disconnect();

        bool IsConnected() const;

        size_t GetID() const;

    public:
        Connection() = default;
        Connection(size_t id, Signal<ArgumentTypes...>* signal);
        ~Connection();

        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        Connection(Connection&& other) noexcept = default;
        Connection& operator=(Connection&& other) noexcept = default;

    private:
        size_t m_id = 0;
        Signal<ArgumentTypes...>* m_signalPtr = nullptr;
        bool m_isConnected = false;
    };

    template<typename... Args>
    void Connection<Args...>::Disconnect()
    {
        if (m_isConnected && m_signalPtr)
        {
            m_signalPtr->disconnect(m_id);
            m_isConnected = false;
            m_signalPtr = nullptr;
        }
    }

    template<typename... Args>
    bool Connection<Args...>::IsConnected() const
    {
        return m_isConnected;
    }

    template<typename... Args>
    size_t Connection<Args...>::GetID() const
    {
        return m_id;
    }

    template<typename... ArgumentTypes>
    Connection<ArgumentTypes...>::Connection(size_t id, Signal<ArgumentTypes...>* signal)
        : m_id(id)
        , m_signalPtr(signal)
        , m_isConnected(true)
    {
    }

    template<typename... Args>
    Connection<Args...>::~Connection()
    {
    }

}// namespace Wl
