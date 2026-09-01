#include "RenderInstance.hpp"
#include "Waterlily/Core/Object/Field.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"

namespace Wl
{

    void RenderInstanceLayout::UpdateData(uint8_t* dst, const RenderSubMesh& src)
    {
        Memory::Copy(dst + ModelOffset, &src.Model, sizeof(decltype(src.Model)));
        Memory::Copy(dst + MaterialOffset, &src.Material, sizeof(decltype(src.Material)));
    }

    RenderInstanceLayout RenderInstance::CreateLayout(size_t alignment)
    {
        size_t offset = 0;
        RenderInstanceLayout layout;

        offset = FieldOffsetAlignUp<Matrix4f>(offset, alignment, layout.ModelOffset);
        offset = FieldOffsetAlignUp<MaterialHandle>(offset, alignment, layout.MaterialOffset);

        layout.Stride = Memory::AlignUp(offset, alignment);

        return layout;
    }

}// namespace Wl