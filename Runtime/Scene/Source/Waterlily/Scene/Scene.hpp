#pragma once

#include "Waterlily/Entity/Registry.hpp"

namespace Wl
{

    class Scene
    {
    public:
        Scene() = default;

        Entity CreateEntity()
        {
            return m_registry.Create();
        }

        void DestroyEntity(Entity e)
        {
            m_registry.Destroy(e);
        }

        template<typename T, typename... Args>
        T& AddComponent(Entity e, Args&&... args)
        {
            return m_registry.AddComponent<T>(e, T{std::forward<Args>(args)...});
        }

        template<typename T>
        T* GetComponent(Entity e)
        {
            return m_registry.GetComponent<T>(e);
        }

        template<typename... T>
        bool HasComponents(Entity e)
        {
            return m_registry.HasComponents<T...>(e);
        }

        template<typename... T, typename Func>
        void ForEach(Func&& f)
        {
            m_registry.ForEach<T...>(std::forward<Func>(f));
        }

    private:
        EntityRegistry m_registry;
    };

}// namespace Wl