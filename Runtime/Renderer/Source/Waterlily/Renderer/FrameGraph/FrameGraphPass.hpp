#pragma once

#include <concepts>
#include <utility>

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Containers/Option.hpp"
#include "Waterlily/Core/Function/Function.hpp"
#include "Waterlily/Core/Memory/Cast.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Framebuffer.hpp"
#include "Waterlily/RHI/RenderPass.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    struct RHIRenderPass;

    class FrameContext;
    class FrameGraph;
    class FrameGraphPass;
    class FrameGraphPassBuilder;

    enum class FrameGraphPassStage
    {
        Graphics,
        Compute,// Not implemented yet.
    };

    struct FrameGraphPassSetupContext
    {
        SharedPtr<RHIDevice> Device;
        SharedPtr<FrameContext> FrameContext;
        FrameGraph* FrameGraph;
    };

    struct WL_RENDERER_API FrameGraphPassExecutionContext
    {
    public:
        SharedPtr<RHIDevice> Device;
        SharedPtr<FrameContext> FrameContext;
        FrameGraph* FrameGraph;
        FrameGraphPass* Pass = nullptr;
        RHIRenderPass* RenderPass = nullptr;
        RHIFramebuffer* Framebuffer = nullptr;
        RHICommandBuffer* CommandBuffer = nullptr;

    public:
        RHIRenderPassBeginInfo CreateRenderPassBeginInfo(Vector4f color,
                                                         Rect2D area,
                                                         float depth = 1.0f,
                                                         uint32_t stencil = 0);
    };

    class WL_RENDERER_API FrameGraphPassInterface
    {
    public:
        virtual void OnSetup(FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder) = 0;

        virtual void OnExecute(FrameGraphPassExecutionContext& context) = 0;

        virtual ~FrameGraphPassInterface() = default;
    };

    class WL_RENDERER_API FrameGraphPassDelegate : public FrameGraphPassInterface
    {
    public:
        using OnSetupCallback = Function<void(FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder)>;
        using OnExecuteCallback = Function<void(FrameGraphPassExecutionContext& context)>;

        virtual void OnSetup(FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder) override
        {
            m_onSetupCallback(context, builder);
        }

        virtual void OnExecute(FrameGraphPassExecutionContext& context) override
        {
            m_onExecuteCallaback(context);
        }

        void SetOnSetup(OnSetupCallback&& onSetup)
        {
            m_onSetupCallback = std::move(onSetup);
        }

        void SetOnExecute(OnExecuteCallback&& onExecute)
        {
            m_onExecuteCallaback = std::move(onExecute);
        }

        FrameGraphPassDelegate() = default;
        virtual ~FrameGraphPassDelegate() = default;

    private:
        OnSetupCallback m_onSetupCallback;
        OnExecuteCallback m_onExecuteCallaback;
    };

    class FrameGraphPassBuilder;

    class WL_RENDERER_API FrameGraphPass
    {
    public:
        template<typename FrameGraphPassDerived, typename... Args>
            requires std::derived_from<FrameGraphPassDerived, FrameGraphPassInterface>
        inline FrameGraphPassDerived& EmplaceDelegate(Args&&... args)
        {
            m_delegate = MakeShared<FrameGraphPassDerived>(std::forward<Args>(args)...);
            return *GetDelegate<FrameGraphPassDerived>();
        }

        inline FrameGraphPassDelegate& EmplaceDefault()
        {
            return EmplaceDelegate<FrameGraphPassDelegate>();
        }

        template<typename PassType>
        inline SharedPtr<PassType> GetDelegate()
        {
            return Wl::StaticPtrCast<PassType>(m_delegate);
        }

        inline SharedPtr<FrameGraphPassInterface> GetDelegate()
        {
            return m_delegate;
        }

        inline const StringID& GetName() const
        {
            return m_name;
        }

        inline size_t GetIndex() const
        {
            return m_index;
        }

        inline size_t GetOrder() const
        {
            return m_order;
        }

        inline FrameGraphPassStage GetStage()
        {
            return m_stage;
        }

        ArrayView<FrameGraphPass*> GetParents()
        {
            return m_parents;
        }

        ArrayView<FrameGraphPass*> GetChildrens()
        {
            return m_childrens;
        }

        bool IsComputeStage()
        {
            return m_stage == FrameGraphPassStage::Compute;
        }

        bool IsGraphicsStage()
        {
            return m_stage == FrameGraphPassStage::Graphics;
        }

        void Setup(FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder)
        {
            if (m_delegate)
            {
                m_delegate->OnSetup(context, builder);
            }
        }

        void Execute(FrameGraphPassExecutionContext& context)
        {
            if (m_delegate)
            {
                m_delegate->OnExecute(context);
            }
        }

        FrameGraphPass(const StringID& name)
            : m_name(name)
        {
        }

        ~FrameGraphPass() = default;

    private:
        friend class FrameGraph;
        friend class FrameGraphPassBuilder;

        StringID m_name;

        FrameGraphPassStage m_stage = FrameGraphPassStage::Graphics;

        Array<FrameGraphTextureHandle> m_textureReads;
        HashMap<FrameGraphTextureHandle, RHITextureLayout> m_textureReadStates;

        Array<FrameGraphTextureHandle> m_textureWrites;
        HashMap<FrameGraphTextureHandle, RHITextureLayout> m_textureWriteStates;

        Option<FrameGraphTextureHandle> m_depthStencil;

        Array<FrameGraphBufferHandle> m_bufferReads;
        Array<FrameGraphBufferHandle> m_bufferWrites;

        SharedPtr<FrameGraphPassInterface> m_delegate;

        Array<FrameGraphPass*> m_parents;
        Array<FrameGraphPass*> m_childrens;

        Array<FrameGraphTextureBarrier> m_barriers;

        size_t m_order = 0;
        size_t m_index = UINT_MAX;

        bool m_isFrameGraphOutput = false;
    };

}// namespace Wl
