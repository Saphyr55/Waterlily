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
        virtual void BeginObject(const StringID& name) override;
        virtual void EndObject() override;

        virtual void BeginArray(const StringID& name) override;
        virtual void EndArray() override;

        virtual void Write(const StringID& name, const void* value, Type type) override;
        
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
        virtual bool BeginObject(const StringID& name) override;
        virtual void EndObject() override;

        virtual size_t BeginArray(const StringID& name) override;
        virtual void EndArray() override;

        virtual bool Read(const StringID& name, void* value, Type type) override;

    public:
        explicit JSONInputArchive(const Json& json)
            : m_root(json)
        {
            m_stack.push_back(&m_root);
        }

        explicit JSONInputArchive(StringRef json)
            : JSONInputArchive(Json::parse(json.GetData()))
        {
            m_stack.push_back(&m_root);
        }

    private:
        const Json* Current() const;

        Json* Find(const StringID& name);

        void Pop();

        template<typename T>
        bool ReadValue(const StringID& name, T& value)
        {
            const Json* json = Find(name);

            if (!json)
            {
                return false;
            }

            try
            {
                value = json->get<T>();
                return true;
            }
            catch (const Json::exception&)
            {
                return false;
            }
        }

    private:
        Json m_root;
        Array<const Json*> m_stack;
    };

}// namespace Wl