#include "Type.hpp"

namespace Wl
{

    Type::Registry& Type::GetRegistry()
    {
        static Registry registry;
        return registry;
    }

    constexpr Type::Type(uint64_t id)
        : m_id(id)
    {
    }

}// namespace Wl