#pragma once

#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/Serialization/Archive.hpp"

#include <nlohmann/json.hpp>

namespace Wl
{

    class WL_CORE_API JSONOutputArchive : public OutputArchive
    {
    public:
        using Json = nlohmann::json;
        using OutputArchive::Write;

    public:
        virtual void BeginObject(StringRef name) override;
        virtual void EndObject() override;

        virtual void BeginArray(StringRef name) override;
        virtual void EndArray() override;

        virtual void Write(StringRef name, const void* value, Type type) override;

        const Json& GetJson() const;
        String Dump(int indent = 2) const;

    public:
        JSONOutputArchive()
            : m_root(Json::object())
        {
            m_stack.Append(&m_root);
        }

    private:
        Json* Current();
        void Pop();
        Json* AddChild(StringRef name, const Json& value);

    private:
        Json m_root;
        Array<Json*> m_stack;
    };

    class WL_CORE_API JSONInputArchive : public InputArchive
    {
    public:
        using Json = nlohmann::json;
        using InputArchive::Read;

    public:
        virtual bool BeginObject(StringRef name) override;
        virtual void EndObject() override;

        virtual size_t BeginArray(StringRef name) override;
        virtual void EndArray() override;

        virtual bool Read(StringRef name, void* value, Type type) override;

    public:
        explicit JSONInputArchive(const Json& json)
            : m_root(json)
        {
            m_stack.Emplace(&m_root, 0);
        }

        explicit JSONInputArchive(StringRef json)
            : JSONInputArchive(Json::parse(json.GetData()))
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
            size_t index;
        };

        Frame* Current();
        Json* Find(StringRef name);

        Json m_root;
        Array<Frame> m_stack;
    };

}// namespace Wl