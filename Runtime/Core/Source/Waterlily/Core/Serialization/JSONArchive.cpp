#include "JSONArchive.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    const JSONOutputArchive::Json& JSONOutputArchive::GetJson() const
    {
        return m_root;
    }

    String JSONOutputArchive::Dump(int indent) const
    {
        return String(m_root.dump(indent).c_str());
    }

    void JSONOutputArchive::BeginObject(StringRef name)
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

    void JSONOutputArchive::EndObject()
    {
        Pop();
    }

    void JSONOutputArchive::BeginArray(StringRef name)
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

    void JSONOutputArchive::EndArray()
    {
        Pop();
    }

    void JSONOutputArchive::Write(StringRef name, const void* value, Type type)
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
        else if (type == MetaTable::TypeOf<int32_t>())
        {
            AddChild(name, Json(*static_cast<const int32_t*>(value)));
        }
        else if (type == MetaTable::TypeOf<int64_t>())
        {
            AddChild(name, Json(*static_cast<const int64_t*>(value)));
        }
        else if (type == MetaTable::TypeOf<uint32_t>())
        {
            AddChild(name, Json(*static_cast<const uint32_t*>(value)));
        }
        else if (type == MetaTable::TypeOf<uint64_t>())
        {
            AddChild(name, Json(*static_cast<const uint64_t*>(value)));
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

    JSONOutputArchive::Json* JSONOutputArchive::AddChild(
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

    JSONOutputArchive::Json* JSONOutputArchive::Current()
    {
        if (m_stack.IsEmpty())
        {
            return nullptr;
        }

        return m_stack.Back();
    }

    void JSONOutputArchive::Pop()
    {
        WL_CHECK(!m_stack.IsEmpty());
        m_stack.PopBack();
    }


    bool JSONInputArchive::BeginObject(StringRef name)
    {
        Json* value = Find(name);

        if (!value || !value->is_object())
        {
            return false;
        }

        m_stack.Emplace(value, 0);
        return true;
    }

    void JSONInputArchive::EndObject()
    {
        Pop();
    }

    size_t JSONInputArchive::BeginArray(StringRef name)
    {
        Json* value = Find(name);

        if (!value || !value->is_array())
        {
            return 0;
        }

        m_stack.Emplace(value, 0);
        return value->size();
    }

    void JSONInputArchive::EndArray()
    {
        Pop();
    }

    bool JSONInputArchive::Read(
            StringRef name,
            void* value,
            Type type)
    {
        WL_CHECK(value);

        if (type == MetaTable::TypeOf<void>())
        {
            return false;
        }

        if (!Current())
        {
            return false;
        }

        if (type == MetaTable::TypeOf<bool>())
        {
            return ReadValue(name, *static_cast<bool*>(value));
        }

        if (type == MetaTable::TypeOf<int32_t>())
        {
            return ReadValue(name, *static_cast<int32_t*>(value));
        }

        if (type == MetaTable::TypeOf<int64_t>())
        {
            return ReadValue(name, *static_cast<int64_t*>(value));
        }

        if (type == MetaTable::TypeOf<uint32_t>())
        {
            return ReadValue(name, *static_cast<uint32_t*>(value));
        }

        if (type == MetaTable::TypeOf<uint64_t>())
        {
            return ReadValue(name, *static_cast<uint64_t*>(value));
        }

        if (type == MetaTable::TypeOf<float>())
        {
            return ReadValue(name, *static_cast<float*>(value));
        }

        if (type == MetaTable::TypeOf<double>())
        {
            return ReadValue(name, *static_cast<double*>(value));
        }

        if (type == MetaTable::TypeOf<String>())
        {
            Json* json = Find(name);

            if (!json || !json->is_string())
            {
                return false;
            }

            const std::string jsonValue = json->get<std::string>();

            *static_cast<String*>(value) = String(jsonValue.c_str());
            return true;
        }

        if (type == MetaTable::TypeOf<StringRef>())
        {
            Json* json = Find(name);

            if (!json || !json->is_string())
            {
                return false;
            }

            const std::string jsonValue = json->get<std::string>();

            *static_cast<StringRef*>(value) = StringRef(jsonValue.c_str());
            return true;
        }

        WL_LOG_ERROR(
                "JSONArchive",
                "Impossible to read '%s' type '%s' not supported.",
                name.GetData(),
                type.GetName().GetData());

        return false;
    }

    JSONInputArchive::Frame* JSONInputArchive::Current()
    {
        if (m_stack.IsEmpty())
        {
            return nullptr;
        }

        return &m_stack.Back();
    }

    JSONInputArchive::Json* JSONInputArchive::Find(StringRef name)
    {
        Frame* current = Current();
        if (!current)
        {
            return nullptr;
        }

        Json* json = current->value;
        if (!json)
        {
            return nullptr;
        }

        if (json->is_array())
        {
            if (current->index >= json->size())
            {
                return nullptr;
            }

            return &(*json)[current->index++];
        }

        if (!json->is_object())
        {
            return nullptr;
        }

        auto it = json->find(name.GetData());
        if (it == json->end())
        {
            return nullptr;
        }

        return &(*it);
    }

    void JSONInputArchive::Pop()
    {
        WL_CHECK(!m_stack.IsEmpty());
        m_stack.Pop();
    }

}// namespace Wl
