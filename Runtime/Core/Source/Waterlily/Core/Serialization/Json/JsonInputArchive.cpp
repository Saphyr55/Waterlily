#include "JsonInputArchive.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    bool JsonInputArchive::BeginObject(StringRef name)
    {
        Json* value = Find(name);

        if (!value || !value->is_object())
        {
            return false;
        }

        m_stack.Emplace(value, 0);
        return true;
    }

    void JsonInputArchive::EndObject()
    {
        Pop();
    }

    usize JsonInputArchive::BeginArray(StringRef name)
    {
        Json* value = Find(name);

        if (!value || !value->is_array())
        {
            return 0;
        }

        m_stack.Emplace(value, 0);
        return value->size();
    }

    void JsonInputArchive::EndArray()
    {
        Pop();
    }

    bool JsonInputArchive::Read(
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

        if (type == MetaTable::TypeOf<int32>())
        {
            return ReadValue(name, *static_cast<int32*>(value));
        }

        if (type == MetaTable::TypeOf<int64>())
        {
            return ReadValue(name, *static_cast<int64*>(value));
        }

        if (type == MetaTable::TypeOf<uint32>())
        {
            return ReadValue(name, *static_cast<uint32*>(value));
        }

        if (type == MetaTable::TypeOf<uint64>())
        {
            return ReadValue(name, *static_cast<uint64*>(value));
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

    JsonInputArchive::Frame* JsonInputArchive::Current()
    {
        if (m_stack.IsEmpty())
        {
            return nullptr;
        }

        return &m_stack.Back();
    }

    JsonInputArchive::Json* JsonInputArchive::Find(StringRef name)
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

    void JsonInputArchive::Pop()
    {
        WL_CHECK(!m_stack.IsEmpty());
        m_stack.Pop();
    }

}// namespace Wl
