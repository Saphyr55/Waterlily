#pragma once

#include "Waterlily/RHI/RHIExports.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/Resource.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    /**
 * @struct RHIBufferDescription
 * @brief Describes the properties of a buffer in the RHI (Rendering Hardware Interface).
 *
 * This struct contains information about the intended usage, access mode, and size of a buffer.
 */
    struct RHIBufferDescription
    {
        size_t Size = 0;
        RHIBufferUsageFlags Usage = RHIBufferUsageFlags::None;
        RHIMemoryUsage MemoryUsage = RHIMemoryUsage::Host;
        RHISharingMode SharingMode = RHISharingMode::Private;
    };

    /**
 * @brief Interface representing a buffer resource in the RHI.
 *
 * RHIBuffer provides an interface for buffer objects, allowing querying of usage, access mode, size,
 * and supporting operations such as binding, mapping, unmapping, and updating buffer data.
 */
    class WL_RHI_API RHIBuffer : public RHIResource
    {
    public:
        /**
     * @brief Gets the usage type of the buffer.
     * @return The usage type as an RHIBufferUsage enum.
     */
        virtual RHIBufferUsageFlags GetUsage() = 0;

        /**
     * @brief Gets the sharing mode of the buffer.
     * @return The access mode enum.
     */
        virtual RHISharingMode GetSharingMode() = 0;

        /**
     * @brief Gets the size of the buffer in bytes.
     * @return The size of the buffer.
     */
        virtual size_t GetSize() = 0;

        /**
     * @brief Binds the buffer to the current context.
     */
        virtual void Bind() = 0;

        /**
     * @brief Maps a region of the buffer for CPU access.
     * @param offset The offset in bytes from the start of the buffer.
     * @param size The size in bytes to map.
     * @return Pointer to the mapped memory region.
     */
        virtual void* Map(size_t offset = 0, size_t size = 0) = 0;

        /**
     * @brief Unmaps the buffer from CPU access.
     */
        virtual void Unmap() = 0;

        /**
     * @brief Updates the buffer with new data.
     * @param data Pointer to the source data.
     * @param size Size of the data in bytes.
     * @param offset Offset in bytes from the start of the buffer to update.
     */
        virtual void Update(const void* data, size_t size, size_t offset = 0) = 0;

        /**
     * @brief destructor.
     */
        virtual ~RHIBuffer() = default;
    };

}// namespace Wl