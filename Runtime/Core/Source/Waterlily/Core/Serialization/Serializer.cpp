#pragma once

#include "Waterlily/Core/Serialization/Serializer.hpp"

namespace Wl
{

    void Serialize(OutputArchive& ar, StringRef name, const String& value)
    {
        ar.Write(name, value);
    }

    void Deserialize(InputArchive& ar, StringRef name, String& value)
    {
        ar.Read(name, value);
    }

}// namespace Wl