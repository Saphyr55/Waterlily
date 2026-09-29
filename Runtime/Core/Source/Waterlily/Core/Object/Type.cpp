#include "Type.hpp"

namespace Wl
{

    Type::Registry& Type::GetRegistry()
    {
        static Registry registry;
        return registry;
    }

    constexpr Type::Type(IdentifierType id) noexcept
        : m_id(id)
    {
    }

}// namespace Wl