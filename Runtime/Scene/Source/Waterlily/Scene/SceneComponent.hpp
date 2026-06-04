#pragma once

#include "Waterlily/Core/Math/Matrix4.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Entity/Entity.hpp"
#include "Waterlily/Entity/Registry.hpp"

namespace Wl
{

    struct NameComponent
    {
        StringID Name;

        NameComponent() = default;
        NameComponent(const StringID& name)
            : Name(name)
        {
        }
    };

    struct TransformComponent
    {
        Matrix4f Local;
        Matrix4f World;
    };

    struct HierarchyComponent
    {
        Entity Parent = EntityRegistry::InvalidEntity;
        Array<Entity> Children;

        bool IsRoot() const
        {
            return Parent == EntityRegistry::InvalidEntity;
        }

        bool HasChildren() const
        {
            return !Children.IsEmpty();
        }
    };

}// namespace Wl