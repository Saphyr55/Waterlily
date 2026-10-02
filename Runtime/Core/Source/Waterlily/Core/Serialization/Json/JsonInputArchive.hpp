#pragma once

#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/Serialization/InputArchive.hpp"

#include <nlohmann/json.hpp>

namespace Wl
{

    class WL_CORE_API JsonInputArchive : public InputArchive
    {
    public:
        using Json = nlohmann::json;
        using InputArchive::Read;

    public:
        virtual bool BeginObject(StringRef name) override;
        virtual void EndObject() override;

        virtual usize BeginArray(StringRef name) override;
        virtual void EndArray() override;

        virtual bool Read(StringRef name, void* value, Type type) override;

    public:
        explicit JsonInputArchive(const Json& json)
            : m_root(json)
        {
            m_stack.Emplace(&m_root, 0);
        }

        explicit JsonInputArchive(StringRef json)
            : JsonInputArchive(Json::parse(json.GetData()))
        {
            m_stack.Emplace(&m_root, 0);
        }

    private:
        void Pop();

        template<typename T>
        bool ReadValue(StringRef name, T& value)
        {
            Json* json = Find(name);
            if (!json)
            {
                return false;
            }

            try
            {
                value = json->get<T>();
                return true;
            }
            catch (const Json::exception& e)
            {
                WL_LOG_ERROR("JSONArchive", "ReadValue with name: '%s'.", name.GetData());
                WL_LOG_ERROR("JSONArchive", "%s", e.what());
                return false;
            }
        }

    private:
        struct Frame
        {
            Json* value;
            usize index;
        };

        Frame* Current();
        Json* Find(StringRef name);

        Json m_root;
        Array<Frame> m_stack;
    };

}// namespace Wl