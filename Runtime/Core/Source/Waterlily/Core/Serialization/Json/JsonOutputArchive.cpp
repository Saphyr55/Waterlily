#include "JsonOutputArchive.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    const JsonOutputArchive::Json& JsonOutputArchive::GetJson() const
    {
        return m_root;
    }

    String JsonOutputArchive::Dump(int indent) const
    {
        return String(m_root.dump(indent).c_str());
    }

    void JsonOutputArchive::BeginObject(StringRef name)
    {
        Json* current = Current();
        WL_CHECK(current);

        Json& parent = *current;

        if (parent.is_array())
        {
            parent.push_back(Json::object());
            m_stack.Append(&parent.back());
            return;
        }

        parent[name.GetData()] = Json::object();
        m_stack.Append(&parent[name.GetData()]);
    }

    void JsonOutputArchive::EndObject()
    {
        Pop();
    }

    void JsonOutputArchive::BeginArray(StringRef name)
    {
        Json* current = Current();
        WL_CHECK(current);

        Json& parent = *current;

        if (parent.is_array())
        {
            parent.push_back(Json::array());
            m_stack.Append(&parent.back());
            return;
        }

        parent[name.GetData()] = Json::array();
        m_stack.Append(&parent[name.GetData()]);
    }

    void JsonOutputArchive::EndArray()
    {
        Pop();
    }

    void JsonOutputArchive::Write(StringRef name, const void* value, Type type)
    {
        WL_CHECK(value);

        if (type == MetaTable::TypeOf<void>())
        {
            return;
        }

        if (!Current())
        {
            return;
        }

        if (type == MetaTable::TypeOf<bool>())
        {
            AddChild(name, Json(*static_cast<const bool*>(value)));
        }
        else if (type == MetaTable::TypeOf<int32>())
        {
            AddChild(name, Json(*static_cast<const int32*>(value)));
        }
        else if (type == MetaTable::TypeOf<int64>())
        {
            AddChild(name, Json(*static_cast<const int64*>(value)));
        }
        else if (type == MetaTable::TypeOf<uint32>())
        {
            AddChild(name, Json(*static_cast<const uint32*>(value)));
        }
        else if (type == MetaTable::TypeOf<uint64>())
        {
            AddChild(name, Json(*static_cast<const uint64*>(value)));
        }
        else if (type == MetaTable::TypeOf<float>())
        {
            AddChild(name, Json(*static_cast<const float*>(value)));
        }
        else if (type == MetaTable::TypeOf<double>())
        {
            AddChild(name, Json(*static_cast<const double*>(value)));
        }
        else if (type == MetaTable::TypeOf<String>())
        {
            AddChild(name, Json(
                                   static_cast<const String*>(value)->GetData()));
        }
        else if (type == MetaTable::TypeOf<StringRef>())
        {
            AddChild(name, Json(
                                   static_cast<const StringRef*>(value)->GetData()));
        }
        else
        {
            WL_LOG_ERROR(
                    "JSONArchive",
                    "Impossible to write '%s' type '%s' not supported.",
                    name.GetData(),
                    type.GetName().GetData());
        }
    }

    JsonOutputArchive::Json* JsonOutputArchive::AddChild(
            StringRef name,
            const Json& value)
    {
        Json* current = Current();
        WL_CHECK(current);

        Json& parent = *current;

        if (parent.is_array())
        {
            parent.push_back(value);
            return &parent.back();
        }

        parent[name.GetData()] = value;
        return &parent[name.GetData()];
    }

    JsonOutputArchive::Json* JsonOutputArchive::Current()
    {
        if (m_stack.IsEmpty())
        {
            return nullptr;
        }

        return m_stack.Back();
    }

    void JsonOutputArchive::Pop()
    {
        WL_CHECK(!m_stack.IsEmpty());
        m_stack.PopBack();
    }

}// namespace Wl
