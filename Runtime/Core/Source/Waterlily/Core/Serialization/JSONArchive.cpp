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
        return String(m_root.dump(indent).data());
    }

    void JSONOutputArchive::BeginObject(const StringID& name)
    {
        std::string stdName = name.GetText().GetData();

        Json object = Json::object();
        Json* parent = Current();

        if (parent->is_array())
        {
            parent->push_back(std::move(object));
            m_stack.Append(&parent->back());
        }
        else
        {
            (*parent)[stdName] = std::move(object);
            m_stack.Append(&(*parent)[stdName]);
        }
    }

    void JSONOutputArchive::EndObject()
    {
        Pop();
    }

    void JSONOutputArchive::BeginArray(const StringID& name)
    {
        std::string stdName = name.GetText().GetData();
        Json array = Json::array();
        Json* parent = Current();

        if (parent->is_array())
        {
            parent->push_back(std::move(array));
            m_stack.Append(&parent->back());
        }
        else
        {
            (*parent)[stdName] = std::move(array);
            m_stack.Append(&(*parent)[stdName]);
        }
    }

    void JSONOutputArchive::EndArray()
    {
        Pop();
    }

    void JSONOutputArchive::Write(const StringID& name, const void* value, Type type)
    {
        WL_CHECK(value);

        if (type == MetaTable::TypeOf<void>())
        {
            return;
        }

        Json* parentPtr = Current();
        if (!parentPtr)
        {
            return;
        }

        Json& parent = *parentPtr;
        const char* nameStr = name.GetText().GetData();

        if (type == MetaTable::TypeOf<bool>())
        {
            parent[nameStr] = *reinterpret_cast<const bool*>(value);
        }
        else if (type == MetaTable::TypeOf<int64_t>() || type == MetaTable::TypeOf<int32_t>())
        {
            int64_t v = *reinterpret_cast<const int64_t*>(value);
            parent[nameStr] = v;
        }
        else if (type == MetaTable::TypeOf<uint32_t>() || type == MetaTable::TypeOf<uint64_t>())
        {
            parent[nameStr] = *reinterpret_cast<const uint64_t*>(value);
        }
        else if (type == MetaTable::TypeOf<double>() || type == MetaTable::TypeOf<float>())
        {
            parent[nameStr] = *reinterpret_cast<const double*>(value);
        }
        else if (type == MetaTable::TypeOf<String>())
        {
            parent[nameStr] = reinterpret_cast<const String*>(value)->GetData();
        }
        else if (type == MetaTable::TypeOf<StringRef>())
        {
            parent[nameStr] = reinterpret_cast<const StringRef*>(value)->GetData();
        }
    }

    JSONOutputArchive::Json* JSONOutputArchive::Current()
    {
        return m_stack.Back();
    }

    void JSONOutputArchive::Pop()
    {
        WL_CHECK(!m_stack.IsEmpty());
        m_stack.PopBack();
    }

    bool JSONInputArchive::BeginObject(const StringID& name)
    {
        Json* value = Find(name);

        if (value == nullptr || !value->is_object())
            return false;

        m_stack.push_back(value);
        return true;
    }

    void JSONInputArchive::EndObject()
    {
        Pop();
    }

    size_t JSONInputArchive::BeginArray(const StringID& name)
    {
        Json* value = Find(name);

        if (value == nullptr || !value->is_array())
            return 0;

        m_stack.push_back(value);
        return value->size();
    }

    void JSONInputArchive::EndArray()
    {
        Pop();
    }

    bool JSONInputArchive::Read(const StringID& name, void* value, Type type)
    {
        if (type == MetaTable::TypeOf<void>())
        {
            return false;
        }

        if (type == MetaTable::TypeOf<bool>())
        {
            return ReadValue(name, *reinterpret_cast<bool*>(value));
        }
        else if (type == MetaTable::TypeOf<int64_t>() || type == MetaTable::TypeOf<int32_t>())
        {
            return ReadValue(name, *reinterpret_cast<int64_t*>(value));
        }
        else if (type == MetaTable::TypeOf<uint32_t>() || type == MetaTable::TypeOf<uint64_t>())
        {
            return ReadValue(name, *reinterpret_cast<uint64_t*>(value));
        }
        else if (type == MetaTable::TypeOf<double>() || type == MetaTable::TypeOf<float>())
        {
            return ReadValue(name, *reinterpret_cast<double*>(value));
        }
        else if (type == MetaTable::TypeOf<String>())
        {
            String& str = *reinterpret_cast<String*>(value);
            const Json* json = Find(name);

            if (!json || !json->is_string())
            {
                return false;
            }

            str = String(json->get<std::string>().data());
            return true;
        }

        return false;
    }

    const JSONInputArchive::Json* JSONInputArchive::Current() const
    {
        return m_stack.Back();
    }

    JSONInputArchive::Json* JSONInputArchive::Find(const StringID& name)
    {
        const Json* current = Current();

        if (!current->is_object())
        {
            return nullptr;
        }

        std::string key = name.GetText().GetData();

        auto it = current->find(key);

        if (it == current->end())
        {
            return nullptr;
        }

        return const_cast<Json*>(&(*it));
    }

    void JSONInputArchive::Pop()
    {
        WL_CHECK(!m_stack.IsEmpty());
        m_stack.Pop();
    }

}// namespace Wl
