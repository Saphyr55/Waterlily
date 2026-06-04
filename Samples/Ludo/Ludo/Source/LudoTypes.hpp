#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/UploadScheduler.hpp"

namespace Wl
{

    inline constexpr uint32_t GlobalSRGIndex = 0;
    inline constexpr uint32_t GlobalSRGRenderViewBinding = 0;
    inline constexpr uint32_t GlobalSRGLightBinding = 1;

    inline constexpr uint32_t DrawItemSRGIndex = 1;
    inline constexpr uint32_t DrawItemSRGBinding = 0;

    inline constexpr uint32_t LudoTextureGRGIndex = 2;
    inline constexpr uint32_t LudoTexturesSRGBinding = 0;

    inline constexpr uint32_t LudoMaterialsSRGIndex = 3;
    inline constexpr uint32_t LudoMaterialsSRGBinding = 0;

    inline const StringID LudoForwardPassName = WL_SID("Forward");
    inline const StringID LudoUIPassName = WL_SID("UI");

    struct UIDrawElement
    {
    public:
        Array<Vector3f> Positions = {
                Vector3f(-0.9f, -0.9f, 0.0f),// Bottom left
                Vector3f(-0.8f, -0.9f, 0.0f),// Bottom right
                Vector3f(-0.8f, -0.8f, 0.0f),// Top right
                Vector3f(-0.9f, -0.8f, 0.0f),// Top left
        };

        Array<Vector4f> Colors = {
                Vector4f(0.25f, 0.15f, 0.15f, 1.0f),
                Vector4f(0.25f, 0.15f, 0.15f, 1.0f),
                Vector4f(0.25f, 0.15f, 0.15f, 1.0f),
                Vector4f(0.25f, 0.15f, 0.15f, 1.0f),
        };

        Array<uint32_t> Indices = {
                0,
                1,
                2,// triangle 1
                2,
                3,
                0,// triangle 2
        };

    public:
        RenderMesh Instanciate(SharedPtr<RHIDevice> device, UploadScheduler& uploader);
    };

}// namespace Wl
