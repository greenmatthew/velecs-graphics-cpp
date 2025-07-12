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

#include <vulkan/vulkan.h>

#include <vma/vk_mem_alloc.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

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
};

} // namespace velecs::graphics
