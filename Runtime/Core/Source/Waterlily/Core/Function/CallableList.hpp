#pragma once

#include "Waterlily/Core/Containers/Array.hpp"

#include <utility>

namespace Wl
{

    template<typename Signature>
    class CallableList
    {
    };

    template<typename ReturnType, typename... ArgumentTypes>
    class CallableList<ReturnType(ArgumentTypes...)>
    {
    };

    template<typename... ArgumentTypes>
    class CallableList<void(ArgumentTypes...)>
    {
    public:
        using Callback = void (*)(void*, ArgumentTypes&&...);

        void Append(Callback callback)
        {
            m_callbackList.Append(callback);
        }

        void Invoke(ArgumentTypes... args)
        {
            for (Callback& callback: m_callbackList)
            {
                callback(std::forward<ArgumentTypes>(args)...);
            }
        }

    private:
        Array<Callback> m_callbackList;
    };

}// namespace Wl