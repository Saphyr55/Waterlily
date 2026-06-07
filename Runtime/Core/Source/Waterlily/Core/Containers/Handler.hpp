#pragma once

#include <functional>

namespace Wl
{

    template<typename HandleType, typename SizeType = size_t>
    class Handler
    {
    public:
        using handle_type = HandleType;
        using size_type = SizeType;

        constexpr static size_type INVALID_HANDLE = static_cast<size_type>(-1);

    public:
        operator size_type() const
        {
            return m_index;
        }

        size_type GetIndex() const
        {
            return m_index;
        }

        bool IsValid() const
        {
            return m_index != INVALID_HANDLE;
        }

        Handler() = default;
        Handler(size_type handle)
            : m_index(handle)
        {
        }
        ~Handler() = default;

    private:
        size_type m_index = INVALID_HANDLE;
    };

}// namespace Wl

template<typename HandleType, typename SizeType>
class std::hash<Wl::Handler<HandleType, SizeType>>
{
public:
    size_t operator()(const Wl::Handler<HandleType, SizeType>& handler) const
    {
        return std::hash<SizeType>()(handler.GetIndex());
    }
};