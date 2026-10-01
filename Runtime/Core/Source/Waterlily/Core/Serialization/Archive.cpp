#include "Archive.hpp"
#include "Waterlily/Core/Object/Object.hpp"

namespace Wl
{
    
    void OutputArchive::Write(const StringID& name, const Object& value)
    {
        Write(name, &value, value.GetObjectType());
    }
    
    bool InputArchive::Read(const StringID& name, Object& value)
    {
        return Read(name, &value, value.GetObjectType());
    }

}// namespace Wl