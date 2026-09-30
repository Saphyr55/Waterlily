#include "Method.hpp"

namespace Wl
{
    void MethodHandle::InvokeRaw(void* self, void** args, void* returnObject) const
    {
        WL_CHECK(IsValid());
        m_invoker(self, args, returnObject);
    }

}// namespace Wl