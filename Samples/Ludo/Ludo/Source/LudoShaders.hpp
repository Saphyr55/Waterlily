#pragma once

#include "LudoExports.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{

    LUDO_API bool CompileShaders();
    LUDO_API void ReloadShaders(SharedPtr<RHIDevice> device,
                                SharedPtr<PipelineManager> pipelineManager,
                                HashMap<StringID, GraphicsPipelineProperties*>& propsMap);

}// namespace Wl