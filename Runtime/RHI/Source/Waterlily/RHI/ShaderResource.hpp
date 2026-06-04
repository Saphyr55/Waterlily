#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    /**
 * @brief Defines a single resource binding slot within a Shader Resource Group Layout.
 */
    struct RHIShaderResourceBinding
    {
        uint32_t Binding = 0;
        RHIShaderResourceType Type = RHIShaderResourceType::Uniform;
        RHIShaderStage Stage = RHIShaderStage::AllGraphics | RHIShaderStage::Compute;
        uint32_t Count = 1;

        RHIShaderResourceBinding() = default;
        RHIShaderResourceBinding(uint32_t binding,
                                 RHIShaderResourceType type,
                                 RHIShaderStage stage = RHIShaderStage::AllGraphics | RHIShaderStage::Compute,
                                 uint32_t count = 1)
            : Binding(binding)
            , Type(type)
            , Stage(stage)
            , Count(count)
        {
        }

        bool operator==(const RHIShaderResourceBinding& other) const
        {
            return Binding == other.Binding && Count == other.Count && Type == other.Type && Stage == other.Stage;
        }
    };

    struct RHIShaderResourceGroupLayoutDescription
    {
    public:
        Array<RHIShaderResourceBinding> Bindings;

    public:
        bool operator==(const RHIShaderResourceGroupLayoutDescription& other) const
        {
            return Bindings == other.Bindings;
        }
    };

    /**
 * @brief Data structure to update a Buffer (Uniform or Storage) resource in a Shader Resource Group.
 */
    struct RHIWriteBufferResource
    {
        RHIBuffer* Buffer = nullptr;
        uint32_t Binding = 0;
        size_t Offset = 0;
        size_t Range = 1;
        size_t ArrayIndex = 0;

        RHIWriteBufferResource() = default;

        RHIWriteBufferResource(uint32_t binding,
                               RHIBuffer* buffer,
                               size_t offset = 0,
                               size_t range = 1,
                               size_t arrayIndex = 0)
            : Buffer(buffer)
            , Binding(binding)
            , Offset(offset)
            , Range(range)
            , ArrayIndex(arrayIndex)
        {
        }
    };

    /**
 * @brief Data structure to update a Sampler resource (independent Sampler binding).
 */
    struct RHIWriteSamplerResource
    {
        RHISampler* Sampler = nullptr;
        uint32_t Binding = 0;
        size_t ArrayIndex = 0;

        RHIWriteSamplerResource() = default;

        RHIWriteSamplerResource(uint32_t binding, RHISampler* sampler, size_t arrayIndex = 0)
            : Sampler(sampler)
            , Binding(binding)
            , ArrayIndex(arrayIndex)
        {
        }
    };

    /**
 * @brief Data structure to update a Combined Image Sampler resource (Texture + Sampler).
 */
    struct RHIWriteTextureSamplerResource
    {
        RHITextureView* TextureView = nullptr;
        RHISampler* Sampler = nullptr;
        uint32_t Binding = 0;
        size_t ArrayIndex = 0;

        RHIWriteTextureSamplerResource() = default;

        RHIWriteTextureSamplerResource(uint32_t binding,
                                       RHITextureView* textureView,
                                       RHISampler* sampler,
                                       size_t arrayIndex = 0)
            : TextureView(textureView)
            , Sampler(sampler)
            , Binding(binding)
            , ArrayIndex(arrayIndex)
        {
        }
    };

    /**
 * @brief Data structure to update a Read-Only Texture (Sampled Image).
 * Used when the sampler is immutable or bound separately.
 */
    struct RHIWriteTextureResource
    {
        RHITextureView* TextureView = nullptr;
        uint32_t Binding = 0;
        size_t ArrayIndex = 0;

        RHIWriteTextureResource() = default;

        RHIWriteTextureResource(uint32_t binding, RHITextureView* textureView, size_t arrayIndex = 0)
            : TextureView(textureView)
            , Binding(binding)
            , ArrayIndex(arrayIndex)
        {
        }
    };

    /**
 * @brief Base class representing the immutable definition of a Shader Resource Group (Vulkan example:
 * VkDescriptorSetLayout).
 */
    class RHIShaderResourceGroupLayout
    {
    public:
        virtual ~RHIShaderResourceGroupLayout() = default;

        virtual RHIShaderResourceType GetResourceType(size_t binding) const = 0;

        virtual const Array<RHIShaderResourceBinding>& GetBindings() const = 0;

    protected:
        RHIShaderResourceGroupLayout() = default;
    };

    /**
 * @brief Base class representing a single instance of a Shader Resource Group (Vulkan example: VkDescriptorSet).
 */
    class RHIShaderResourceGroup
    {
    public:
        /**
     * @brief Writes an RHI Buffer (Uniform or Storage) to the specified binding.
     */
        virtual void SetBuffer(const RHIWriteBufferResource& resource) = 0;

        /**
     * @brief Writes a separate RHI Sampler to the specified binding.
     */
        virtual void SetSampler(const RHIWriteSamplerResource& resource) = 0;

        /**
     * @brief Writes a Combined Image Sampler (Texture and Sampler) to the specified binding.
     * Use this for textures that are sampled in the shader.
     */
        virtual void SetTextureSampler(const RHIWriteTextureSamplerResource& resource) = 0;

        /**
     * @brief Writes a Storage Image or a Read-Only Texture View to the specified binding.
     * Use this when the sampler is not part of the binding.
     */
        virtual void SetTexture(const RHIWriteTextureResource& resource) = 0;

        /**
     * @brief Finalizes the group if internal buffering or compilation is required (less common in Vulkan).
     * This method is often a no-op but included for RHI flexibility.
     */
        virtual void Update() = 0;

        /**
     * @brief
     */
        virtual uint32_t GetIndexPool() const = 0;

        virtual ~RHIShaderResourceGroup() = default;
    };

}// namespace Wl