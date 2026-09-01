#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Entity/ComponentPool.hpp"
#include "Waterlily/Entity/Entity.hpp"

#include <tuple>

namespace Wl
{

    template<typename... Components>
    class EntityView
    {
    public:
        using ComponentTypes = std::tuple<Components...>;

        class Iterator
        {
        public:
            using value_type = std::tuple<Entity, Components&...>;
            using reference = value_type&;
            using pointer = value_type*;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            Iterator(EntityView* view, size_t index)
                : m_view(view)
                , m_index(index)
            {
            }

            bool operator!=(const Iterator& other) const
            {
                return m_index != other.m_index;
            }

            bool operator==(const Iterator& other) const
            {
                return m_index == other.m_index;
            }

            Iterator& operator++()
            {
                m_index++;
                SkipInvalid();
                return *this;
            }

            value_type operator*()
            {
                Entity entity = m_view->m_entities[m_index];
                return m_view->GetComponents(entity);
            }

        private:
            void SkipInvalid()
            {
                while (m_index < m_view->GetSize())
                {
                    if (m_view->HasComponents(m_view->m_entities[m_index]))
                    {
                        break;
                    }
                    m_index++;
                }
            }

        private:
            EntityView* m_view;
            size_t m_index;
        };

        using iterator = Iterator;

    public:
        EntityView(const Array<Entity>& entities, const std::tuple<ComponentPool<Components>*...>& pools)
            : m_entities(entities)
            , m_pools(pools)
        {
        }

        Entity First()
        {
            return m_entities.Front();
        }

        Entity Last()
        {
            return m_entities.Back();
        }

        template<typename Func>
        void ForEach(Func&& func) const
        {
            std::apply([&](ComponentPool<Components>*... pools)
            {
                for (Entity entity: m_entities)
                {
                    if ((pools->HasComponent(entity) && ...))
                    {
                        func(entity, *(pools->GetComponent(entity))...);
                    }
                }
            }, m_pools);
        }

        bool HasComponents(Entity entity) const
        {
            return std::apply([entity](ComponentPool<Components>*... pools)
            {
                return (pools->HasComponent(entity) && ...);
            }, m_pools);
        }

        auto GetComponents(Entity entity)
        {
            return std::apply([entity](ComponentPool<Components>*... pools)
            {
                return std::make_tuple(entity, std::ref(*(pools->GetComponent(entity)))...);
            }, m_pools);
        }

        size_t GetSize() const
        {
            return m_entities.GetSize();
        }

        bool IsEmpty() const
        {
            return m_entities.IsEmpty();
        }

        Iterator begin()
        {
            return Iterator(this, 0);
        }

        Iterator end()
        {
            return Iterator(this, m_entities.GetSize());
        }

    private:
    private:
        const Array<Entity>& m_entities;
        std::tuple<ComponentPool<Components>*...> m_pools;
    };

}// namespace Wl
