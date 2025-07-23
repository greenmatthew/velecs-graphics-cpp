/// @file    RenderEngine.hpp
/// @author  Matthew Green
/// @date    2025-07-11 17:54:28
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Memory/UploadContext.hpp"
#include "velecs/graphics/Memory/DeletionQueue.hpp"
#include "velecs/graphics/Memory/AllocatedImage.hpp"
#include "velecs/graphics/Memory/AllocatedBuffer.hpp"

#include "velecs/graphics/Mesh.hpp"

#include <vulkan/vulkan.h>

#include <vma/vk_mem_alloc.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <optional>
#include <memory>

namespace velecs::graphics {

/// @class RenderEngine
/// @brief Brief description.
///
/// Rest of description.
class RenderEngine {
public:
    // Enums

    // Public Fields

    static const bool ENABLE_VALIDATION_LAYERS;

    // Constructors and Destructors

    /// @brief Default constructor.
    inline RenderEngine(SDL_Window* const window)
        : _window(window) {}

    /// @brief Default deconstructor.
    ~RenderEngine() = default;

    // Public Methods

    SDL_AppResult Init();
    void Draw();
    void Cleanup();

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    SDL_Window* _window{nullptr}; /// @brief Pointer to the SDL window structure.

    VkInstance _instance{VK_NULL_HANDLE}; /// @brief Handle to the Vulkan library.
    VkDebugUtilsMessengerEXT _debug_messenger{VK_NULL_HANDLE}; /// @brief Handle for Vulkan debug messaging.
    VkPhysicalDevice _chosenGPU{VK_NULL_HANDLE}; /// @brief The chosen GPU for rendering operations.
    VkDevice _device{VK_NULL_HANDLE}; /// @brief Handle to the Vulkan device.
    VkSurfaceKHR _surface{VK_NULL_HANDLE}; /// @brief Handle to the Vulkan window surface.

    VkSwapchainKHR _swapchain{VK_NULL_HANDLE}; /// @brief Handle to the Vulkan swapchain.
    VkFormat _swapchainImageFormat{VK_FORMAT_UNDEFINED}; /// @brief The format used for swapchain images.
    std::vector<VkImage> _swapchainImages; /// @brief List of images within the swapchain.
    std::vector<VkImageView> _swapchainImageViews; /// @brief List of image views for accessing swapchain images.
    uint32_t swapchainImageIndex{0};

    VkQueue _graphicsQueue{VK_NULL_HANDLE}; /// @brief Queue used for submitting graphics commands.
    uint32_t _graphicsQueueFamily{0}; /// @brief Index of the queue family for graphics operations.
    VkCommandPool _commandPool{VK_NULL_HANDLE}; /// @brief Pool for allocating command buffers.
    VkCommandBuffer _mainCommandBuffer{VK_NULL_HANDLE}; /// @brief Main command buffer for recording rendering commands.

    VkRenderPass _renderPass{VK_NULL_HANDLE}; /// @brief Handle to the Vulkan render pass.
    std::vector<VkFramebuffer> _framebuffers; /// @brief List of framebuffers for rendering.

    VkSemaphore _presentSemaphore{VK_NULL_HANDLE}, _renderSemaphore{VK_NULL_HANDLE}; /// @brief Semaphore for synchronizing image presentation.
    VkFence _renderFence{VK_NULL_HANDLE}; /// @brief Fence for synchronizing rendering operations.

    VkPipeline currentPipeline{VK_NULL_HANDLE};

    std::vector<VkPipeline> pipelines;
    std::vector<VkPipelineLayout> pipelineLayouts;

    VkPipelineLayout _trianglePipelineLayout{VK_NULL_HANDLE}; /// @brief Handle to the pipeline layout.
    VkPipeline _triangleWireFramePipeline{VK_NULL_HANDLE}; /// @brief Handle to the pipeline.
    VkPipeline _rainbowSimpleMeshPipeline{VK_NULL_HANDLE}; /// @brief Handle to the pipeline.

    VkPipelineLayout _meshPipelineLayout{VK_NULL_HANDLE};
    VkPipeline _meshPipeline{VK_NULL_HANDLE};

    VkPipelineLayout simpleMeshPipelineLayout{VK_NULL_HANDLE};
    VkPipeline simpleMeshPipeline{VK_NULL_HANDLE};

    UploadContext _uploadContext;

    DeletionQueue _mainDeletionQueue;

    VmaAllocator _allocator{nullptr};

    VkImageView _depthImageView{VK_NULL_HANDLE};
    AllocatedImage _depthImage;
    VkFormat _depthFormat{VK_FORMAT_UNDEFINED};

    VkDescriptorPool imguiPool{VK_NULL_HANDLE};


    // These fields should be better handled
    VkPipeline _vertexColorsPipeline{VK_NULL_HANDLE};

    // Private Methods

    bool InitVulkan();
    bool InitSwapchain();
    bool InitCommands();
    bool InitRenderPass();
    bool InitFramebuffers();
    bool InitSyncStructures();
    bool InitPipelines();
    
    void CleanupVulkan();

    VkExtent2D GetWindowExtent() const;


    // These functions should be better handled
    void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& function);

    template<typename T>
    std::optional<AllocatedBuffer> CreateBuffer(const std::vector<T>& data, VkBufferUsageFlags usage)
    {
        const size_t bufferSize = data.size() * sizeof(T);
        
        // Create staging buffer (CPU accessible)
        VkBufferCreateInfo stagingBufferInfo{};
        stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        stagingBufferInfo.size = bufferSize;
        stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        
        VmaAllocationCreateInfo stagingAllocInfo{};
        stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        
        AllocatedBuffer stagingBuffer;
        VkResult result = vmaCreateBuffer(
            _allocator,
            &stagingBufferInfo,
            &stagingAllocInfo,
            &stagingBuffer._buffer,
            &stagingBuffer._allocation,
            nullptr
        );
        if (result != VK_SUCCESS)
        {
            std::cerr << "Failed to create staging buffer: " << result << std::endl;
            return std::nullopt; // Fixed: was "return;"
        }
        
        // Copy data to staging buffer
        void* mappedData;
        vmaMapMemory(_allocator, stagingBuffer._allocation, &mappedData);
        memcpy(mappedData, data.data(), bufferSize);
        vmaUnmapMemory(_allocator, stagingBuffer._allocation);
        
        // Create GPU buffer
        VkBufferCreateInfo gpuBufferInfo{};
        gpuBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        gpuBufferInfo.size = bufferSize;
        gpuBufferInfo.usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        
        VmaAllocationCreateInfo gpuAllocInfo{};
        gpuAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        
        AllocatedBuffer gpuBuffer;
        result = vmaCreateBuffer(
            _allocator,
            &gpuBufferInfo,
            &gpuAllocInfo,
            &gpuBuffer._buffer,
            &gpuBuffer._allocation,
            nullptr
        );
        if (result != VK_SUCCESS)
        {
            std::cerr << "Failed to create GPU buffer: " << result << std::endl;
            // Cleanup staging buffer before returning
            vmaDestroyBuffer(_allocator, stagingBuffer._buffer, stagingBuffer._allocation);
            return std::nullopt; // Fixed: was "std::optional::none"
        }
        
        // Copy from staging to GPU buffer
        ImmediateSubmit([=](VkCommandBuffer cmd) {
            VkBufferCopy copy{};
            copy.size = bufferSize;
            vkCmdCopyBuffer(cmd, stagingBuffer._buffer, gpuBuffer._buffer, 1, &copy);
        });
        
        // Cleanup staging buffer
        vmaDestroyBuffer(_allocator, stagingBuffer._buffer, stagingBuffer._allocation);
        
        // Add GPU buffer to deletion queue
        _mainDeletionQueue.PushDeletor([=]() {
            vmaDestroyBuffer(_allocator, gpuBuffer._buffer, gpuBuffer._allocation);
        });
        
        return gpuBuffer; // Success case
    }

    void CreateTriangleBuffers();

    void PreDraw();
    void PostDraw();
};

} // namespace velecs::graphics
