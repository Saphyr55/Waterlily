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
            using reference = value_type;
            using pointer = void;
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
                return *this;
            }

            Iterator& operator--()
            {
                m_index--;
                return *this;
            }

            value_type operator*()
            {
                Entity entity = m_view->m_entities[m_index];
                return m_view->GetComponents(entity);
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

        template<typename Func>
        void ForEach(Func&& func)
        {
            for (Entity entity: m_entities)
            {
                if (HasComponents(entity))
                {
                    InvokeWithComponents(entity, std::forward<Func>(func));
                }
            }
        }

        template<typename Func>
        void ForEach(Func&& func) const
        {
            for (Entity entity: m_entities)
            {
                if (HasComponents(entity))
                {
                    InvokeWithComponents(entity, std::forward<Func>(func));
                }
            }
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
        bool HasComponents(Entity entity) const
        {
            return std::apply([entity](auto*... pools)
            {
                return (pools->HasComponent(entity) && ...);
            }, m_pools);
        }

        template<typename Func>
        void InvokeWithComponents(Entity entity, Func&& func)
        {
            std::apply([entity, &func](auto*... pools)
            {
                func(entity, *(pools->GetComponent(entity))...);
            }, m_pools);
        }

        template<typename Func>
        void InvokeWithComponents(Entity entity, Func&& func) const
        {
            std::apply([entity, &func](auto*... pools)
            {
                func(entity, *(pools->GetComponent(entity))...);
            }, m_pools);
        }

        auto GetComponents(Entity entity)
        {
            return std::apply(
                    [entity](auto*... pools)
            {
                return std::make_tuple(entity, std::ref(*(pools->GetComponent(entity)))...);
            },
                    m_pools);
        }

    private:
        const Array<Entity>& m_entities;
        std::tuple<ComponentPool<Components>*...> m_pools;
    };

}// namespace Wl
