#include "RenderScene.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Renderer/Mesh/StaticMesh.hpp"
#include "Waterlily/Renderer/Model/Model.hpp"
#include "Waterlily/Renderer/RenderService.hpp"

namespace Wl
{

    RenderScene::RenderScene(const SharedPtr<RenderService>& renderService)
        : m_renderService(renderService)
        , m_mesh(renderService->GetDevice())
    {
    }

    void RenderScene::Activate(SharedPtr<AssetManager> assetManager, ArrayView<Model*> models)
    {
        for (Model* model: models)
        {
            Array<StaticMesh*> modelStaticMeshesAsset = Model::GetMeshes(model, assetManager);
            for (StaticMesh* staticMesh : modelStaticMeshesAsset)
            {
                
            }
        }
    }

    void RenderScene::Desactivate()
    {

    }

    void RenderScene::PrepareFrame(EntityRegistry& registry, Frame& frame, const Camera& camera)
    {
        float aspectRatio = m_renderService->GetFrameContext()->GetAspectRatio();

        Matrix4f proj = Matrix4f::Perspective(Math::Radians(75.0f), aspectRatio, 0.1f, 1000.0f);
        RenderView view = RenderView::CreateFromCamera(camera, proj);
    }

}// namespace Wl