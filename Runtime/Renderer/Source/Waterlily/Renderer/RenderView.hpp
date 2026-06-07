#pragma once

#include "Waterlily/Core/Math/Matrix4.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"

namespace Wl
{

    struct RenderView
    {
        Matrix4f View = Matrix4f::Identity();
        Matrix4f Proj = Matrix4f::Identity();
        Matrix4f ViewProj = Matrix4f::Identity();
        Vector3f Eye = Vector3f(0.0f);
    };
    
}// namespace Wl
