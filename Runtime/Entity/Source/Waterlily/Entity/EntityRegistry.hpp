#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Cast.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Entity/ComponentPool.hpp"
#include "Waterlily/Entity/Entity.hpp"
#include "Waterlily/Entity/EntityExports.hpp"
#include "Waterlily/Entity/EntityView.hpp"

#include <tuple>
#include <utility>

namespace Wl
{

    class WL_ENTITY_API EntityRegistry
    {
    public:
        using ComponentPoolMap = HashMap<ComponentTypeID, SharedPtr<IComponentPool>>;

        static constexpr const Entity InvalidEntity = 0;

    public:
        Entity Create();

        void Destroy(Entity e);

        bool IsValid(Entity e) const
        {
            return m_entityToIndex.Contains(e);
        }

        template<typename ComponentType>
        ComponentType& AddComponent(Entity e, const ComponentType& component = ComponentType())
        {
            return GetPool<ComponentType>().AddComponent(e, component);
        }

        template<typename ComponentType>
        void RemoveComponent(Entity e)
        {
            if (auto it = m_componentPools.find(GetTypeIndex<ComponentType>()); it != m_componentPools.end())
            {
                Wl::StaticPtrCast<ComponentPool<ComponentType>>((*it).Value)->RemoveComponent(e);
            }
        }

        template<typename ComponentType>
        bool HasComponent(Entity e) const
        {
            auto it = m_componentPools.find(GetTypeIndex<ComponentType>());
            if (it == m_componentPools.cend())
            {
                return false;
            }
            return Wl::StaticPtrCast<ComponentPool<ComponentType>>((*it).Value)->HasComponent(e);
        }

        template<typename... Components>
        bool HasComponents(Entity e) const
        {
            return (... && HasComponent<Components>(e));
        }

        template<typename ComponentType>
        ComponentType* GetComponent(Entity e)
        {
            auto it = m_componentPools.find(GetTypeIndex<ComponentType>());
            if (it == m_componentPools.end())
            {
                return nullptr;
            }
            return Wl::StaticPtrCast<ComponentPool<ComponentType>>((*it).Value)->GetComponent(e);
        }

        template<typename... Components>
        EntityView<Components...> View()
        {
            std::tuple<ComponentPool<Components>*...> pools = GetPools<Components...>();
            IComponentPool* smallestPool = FindSmallestPool<Components...>();
            const Array<Entity>& entities = smallestPool->GetEntities();
            return EntityView<Components...>(entities, pools);
        }

        template<typename... Components, typename Func>
        void ForEach(Func&& func)
        {
            View<Components...>().ForEach(std::forward<Func>(func));
        }

        template<typename ComponentType>
        ComponentPool<ComponentType>& GetPool()
        {
            ComponentTypeID key = GetTypeIndex<ComponentType>();
            auto it = m_componentPools.find(key);
            if (it == m_componentPools.end())
            {
                SharedPtr<ComponentPool<ComponentType>> pool = MakeShared<ComponentPool<ComponentType>>();
                ComponentPool<ComponentType>* ptr = pool.GetResource();
                m_componentPools[key] = std::move(pool);
                return *ptr;
            }
            return *StaticPtrCast<ComponentPool<ComponentType>>((*it).Value);
        }

        void Dispose();

        EntityRegistry()
            : m_nextIndex(1)
        {
        }

        ~EntityRegistry()
        {
        }

    private:
        template<typename... Components, typename Func>
        void InvokeWithComponents(Entity entity, Func&& func)
        {
            if constexpr (sizeof...(Components) == 1)
            {
                auto* component = GetComponent<std::tuple_element_t<0, std::tuple<Components...>>>(entity);
                if (component)
                {
                    func(entity, *component);
                }
            }
            else
            {
                auto components = std::make_tuple(GetComponent<Components>(entity)...);
                std::apply([&](auto*... comps)
                {
                    func(entity, *comps...);
                }, components);
            }
        }

        template<typename... Components>
        std::tuple<ComponentPool<Components>*...> GetPools()
        {
            return std::make_tuple(&GetPool<Components>()...);
        }

        template<typename... Components>
        IComponentPool* FindSmallestPool()
        {
            std::tuple<ComponentPool<Components>*...> pools = GetPools<Components...>();
            IComponentPool* smallest = nullptr;
            size_t minSize = std::numeric_limits<size_t>::max();

            std::apply([&](auto*... poolsPtr)
            {
                size_t sizes[] = {poolsPtr->GetStorage().GetSize()...};
                IComponentPool* ptrs[] = {poolsPtr...};

                for (size_t i = 0; i < sizeof...(Components); i++)
                {
                    if (sizes[i] < minSize)
                    {
                        minSize = sizes[i];
                        smallest = ptrs[i];
                    }
                }
            }, pools);

            return smallest;
        }

    private:
        ComponentPoolMap m_componentPools;
        HashMap<Entity, size_t> m_entityToIndex;
        Array<Entity> m_LivingEntities;
        Entity m_nextIndex = InvalidEntity;
    };

}// namespace Wl