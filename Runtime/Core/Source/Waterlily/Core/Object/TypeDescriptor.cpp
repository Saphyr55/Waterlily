#include "TypeDescriptor.hpp"
#include "TypeInfo.hpp"
#include "Waterlily/Core/String/Format.hpp"

namespace Wl
{

    String TypeDescriptor::GetTypeName() const
    {
        const TypeInfo& info = GetUnderlyingType().GetTypeInfo();
        String fullTypeName(info.name.GetText());

        if (IsVolatile())
        {
            fullTypeName = "volatile " + fullTypeName;
        }

        if (IsConst())
        {
            fullTypeName = "const " + fullTypeName;
        }

        for (uint32_t i = 0; i < GetPointerCount(); i++)
        {
            fullTypeName += "*";
        }

        if (IsArray())
        {
            String sizeStr = Wl::Format("%d", GetArraySize());
            fullTypeName += "[";
            fullTypeName += sizeStr;
            fullTypeName += "]";
        }

        if (IsReference())
        {
            fullTypeName += "&";
        }
        else if (IsRValueReference())
        {
            fullTypeName += "&&";
        }

        return fullTypeName;
    }

}// namespace Wl