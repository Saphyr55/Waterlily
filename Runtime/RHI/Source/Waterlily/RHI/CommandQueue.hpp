#pragma once

#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Fence.hpp"
#include "Waterlily/RHI/Semaphore.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    /**
 * @brief Interface for a GPU command queue.
 *
 * A command queue is responsible for submitting command buffers to the GPU,
 * synchronizing via fences/semaphores, and presenting images to a swapchain.
 */
    class RHICommandQueue
    {
    public:
        virtual void Submit(const Array<RHICommandBuffer*>& commandBuffers,
                            const Array<RHISemaphore*>& waitSemaphores,
                            const Array<RHISemaphore*>& signalSemaphores,
                            const RHIFence* fence) const = 0;


        virtual void Present(RHISwapchain* swapchain, RHISemaphore* semaphore) const = 0;

        /**
     * @brief Get the type of this queue.
     * @return The queue type.
     */
        virtual RHIQueueType GetQueueType() const = 0;

        inline bool IsQueueType(RHIQueueType type) const
        {
            return (GetQueueType() & type) == type;
        }

        inline bool IsGraphicsQueueType() const
        {
            return IsQueueType(RHIQueueType::Graphics);
        }

        inline bool IsComputeQueueType() const
        {
            return IsQueueType(RHIQueueType::Compute);
        }

        inline bool IsTransferQueueType() const
        {
            return IsQueueType(RHIQueueType::Transfer);
        }

        /**
     * @brief Block until all submitted work on this queue has completed.
     */
        virtual void WaitIdle() = 0;

        /**
     * @brief Check if the queue supports the present mode.
     * @return Return true if supported, false otherwise.
     */
        virtual bool IsPresentMode() const = 0;

        /**
     * @brief Destructor.
     */
        virtual ~RHICommandQueue() = default;
    };

}// namespace Wl
