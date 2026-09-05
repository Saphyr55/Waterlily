#pragma once

#include "Waterlily/Core/Math/Matrix4.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Scene/Camera.hpp"

namespace Wl
{

    class WL_RENDERER_API RenderView
    {
    public:
        Matrix4f View = Matrix4f::Identity();
        Matrix4f Proj = Matrix4f::Identity();
        Matrix4f ViewProj = Matrix4f::Identity();
        Vector3f Eye = Vector3f(0.0f);

        static RenderView CreateFromCamera(const Camera& camera, const Matrix4f& proj);
    };

}// namespace Wl
