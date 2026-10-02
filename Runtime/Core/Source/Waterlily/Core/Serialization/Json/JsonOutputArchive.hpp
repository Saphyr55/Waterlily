#pragma once

#include "Waterlily/Core/Serialization/OutputArchive.hpp"

#include <nlohmann/json.hpp>

namespace Wl
{

    class WL_CORE_API JsonOutputArchive : public OutputArchive
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
        JsonOutputArchive()
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

}// namespace Wl