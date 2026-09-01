#include "RenderView.hpp"
#include "Waterlily/Core/Math/Matrix4.hpp"

namespace Wl
{

    RenderView RenderView::CreateFromCamera(const Camera& camera, const Matrix4f& proj)
    {
        RenderView view;
        view.View = camera.View;
        view.Proj = proj;
        view.ViewProj = view.Proj * view.View;
        view.Eye = camera.Position;
        return view;
    }

}// namespace Wl
