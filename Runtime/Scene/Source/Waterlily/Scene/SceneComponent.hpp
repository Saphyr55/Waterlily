#pragma once

#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Entity/Entity.hpp"
#include "Waterlily/Entity/EntityRegistry.hpp"
#include "Waterlily/Scene/Camera.hpp"

namespace Wl
{

    struct NameComponent
    {
        StringID Name;
    };

    struct TransformComponent
    {
        Vector3f Position;
    };

    struct HierarchyComponent
    {
        Entity Parent = EntityRegistry::InvalidEntity;
        Array<Entity> Childrens;

        bool IsRoot() const
        {
            return Parent == EntityRegistry::InvalidEntity;
        }

        bool HasChildren() const
        {
            return !Childrens.IsEmpty();
        }
    };

    struct CameraComponent
    {
        Camera camera;
    };

}// namespace Wl