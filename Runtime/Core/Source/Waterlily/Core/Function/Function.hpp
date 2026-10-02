#pragma once

#include <functional>
#include <type_traits>

namespace Wl
{

    template<typename Signature>
    using Function = std::function<Signature>;

    template<typename Signature>
    class FunctionRef;

    template<typename ReturnType, typename... ArgumentTypes>
    class FunctionRef<ReturnType(ArgumentTypes...)>
    {
    public:
        constexpr FunctionRef() = default;

        template<typename Callable, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Callable>, FunctionRef>>>
        constexpr FunctionRef(Callable&& c)
        {
            m_obj = (void*)std::addressof(c);
            m_callback = [](void* obj, ArgumentTypes&&... args) -> ReturnType
            {
                return (*reinterpret_cast<std::add_pointer_t<Callable>>(obj))(std::forward<ArgumentTypes>(args)...);
            };
        }

        ReturnType operator()(ArgumentTypes... args) const
        {
            return m_callback(m_obj, std::forward<ArgumentTypes>(args)...);
        }

        explicit operator bool() const noexcept
        {
            return m_callback != nullptr;
        }

    private:
        using Callback = ReturnType (*)(void*, ArgumentTypes&&...);

        void* m_obj = nullptr;
        Callback m_callback = nullptr;
    };

}