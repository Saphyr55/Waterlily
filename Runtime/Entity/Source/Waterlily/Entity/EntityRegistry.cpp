#include "Waterlily/Entity/EntityRegistry.hpp"

#include "Waterlily/Entity/Entity.hpp"

namespace Wl
{
    
    Entity EntityRegistry::Create()
    {
        Entity e = m_nextIndex++;
        m_entityToIndex[e] = m_LivingEntities.GetSize();
        m_LivingEntities.Append(e);
        return e;
    }

    void EntityRegistry::Destroy(Entity entity)
    {
        auto it = m_entityToIndex.find(entity);
        if (it == m_entityToIndex.end())
        {
            return;
        }

        size_t index = (*it).Value;
        Entity last = m_LivingEntities.Back();

        m_LivingEntities[index] = last;
        m_entityToIndex[last] = index;

        m_LivingEntities.Pop();
        m_entityToIndex.Remove(entity);

        for (auto [_, pool]: m_componentPools)
        {
            pool->RemoveComponent(entity);
        }
    }

    void EntityRegistry::Dispose()
    {
        for (auto [type, pool]: m_componentPools)
        {
            pool->Dispose();
        }

        m_entityToIndex.Clear();
        m_componentPools.Clear();
        m_LivingEntities.Clear();
        m_nextIndex = 1;
    }

}// namespace Wl