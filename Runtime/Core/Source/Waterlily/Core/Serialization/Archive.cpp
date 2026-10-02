#include "Archive.hpp"
#include "Waterlily/Core/Object/Object.hpp"

namespace Wl
{
    
    void OutputArchive::Write(StringRef name, const Object& value)
    {
        Write(name, &value, value.GetObjectType());
    }
    
    bool InputArchive::Read(StringRef name, Object& value)
    {
        return Read(name, &value, value.GetObjectType());
    }

}// namespace Wl