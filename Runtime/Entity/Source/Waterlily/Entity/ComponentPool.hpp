#pragma once

#include "Waterlily/Core/Containers/SparseSet.hpp"
#include "Waterlily/Entity/Entity.hpp"
#include "Waterlily/Entity/EntityExports.hpp"

namespace Wl
{

    class WL_ENTITY_API IComponentPool
    {
    public:
        virtual void RemoveComponent(Entity entity) = 0;

        virtual void Dispose() = 0;

        virtual bool HasComponent(Entity e) const = 0;

        virtual Array<Entity>& GetEntities() = 0;

        virtual ~IComponentPool() = default;
    };

    template<typename ComponentType>
    class ComponentPool : public IComponentPool
    {
    public:
        ComponentType& AddComponent(Entity entity, const ComponentType& component)
        {
            return *m_storage.Put(entity, component);
        }

        virtual void RemoveComponent(Entity entity) override
        {
            if (HasComponent(entity))
            {
                m_storage.Remove(entity);
            }
        }

        virtual bool HasComponent(Entity entity) const override
        {
            return m_storage.Get(entity) != nullptr;
        }

        ComponentType* GetComponent(Entity entity)
        {
            return m_storage.Get(entity);
        }

        template<typename Func>
        void ForEach(Func&& func)
        {
            auto& values = m_storage.GetElements();
            for (auto& v: values)
            {
                func(v);
            }
        }

        virtual void Dispose() override
        {
            m_storage.Clear();
        }

        const SparseSet<ComponentType, Entity>& GetStorage() const
        {
            return m_storage;
        }

        SparseSet<ComponentType, Entity>& GetStorage()
        {
            return m_storage;
        }

        virtual Array<Entity>& GetEntities() override
        {
            return m_storage.GetDense();
        }

    private:
        SparseSet<ComponentType, Entity> m_storage;
    };
}// namespace Wl