/// @file    RenderEngine.cpp
/// @author  Matthew Green
/// @date    2025-07-11 17:55:19
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/RenderEngine.hpp"

#include "velecs/graphics/VulkanInitializers.hpp"

#include <iostream>
#include <fstream>
#include <chrono>

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace velecs::graphics {

// Public Fields

const bool RenderEngine::ENABLE_VALIDATION_LAYERS
#ifdef _DEBUG
    = true;
#else
    = false;
#endif

// Constructors and Destructors

// Public Methods

SDL_AppResult RenderEngine::Init()
{
    if (!InitVulkan()        ) return SDL_APP_FAILURE;
    if (!InitSwapchain()     ) return SDL_APP_FAILURE;
    if (!InitCommands()      ) return SDL_APP_FAILURE;
    if (!InitRenderPass()    ) return SDL_APP_FAILURE;
    if (!InitFramebuffers()  ) return SDL_APP_FAILURE;
    if (!InitSyncStructures()) return SDL_APP_FAILURE;
    if (!InitPipelines()     ) return SDL_APP_FAILURE;
    
    return SDL_APP_CONTINUE;
}

void RenderEngine::Cleanup()
{
    CleanupVulkan();
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

bool RenderEngine::InitVulkan()
{
    vkb::InstanceBuilder builder;

    const char* windowTitle = SDL_GetWindowTitle(_window);

    auto builderResult = builder.set_app_name(windowTitle)
            .request_validation_layers(ENABLE_VALIDATION_LAYERS)
            .require_api_version(1, 1, 0)
            .use_default_debug_messenger()
            .build();

    // Check if instance creation was successful before proceeding
    if (!builderResult)
    {
        std::cerr << "Failed to create Vulkan instance. Error: " << builderResult.error().message() << std::endl;
        return false;
    }

    if (ENABLE_VALIDATION_LAYERS && builderResult) 
    {
        std::cout << "Available validation layers:" << std::endl;
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        bool foundValidationLayer = false;
        for (const auto& layer : availableLayers) 
        {
            std::cout << "\t" << layer.layerName << std::endl;
            if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
                foundValidationLayer = true;
            }
        }
        
        if (!foundValidationLayer) {
            std::cout << "WARNING: Validation layers requested but VK_LAYER_KHRONOS_validation not found!" << std::endl;
        }
    }

    vkb::Instance vkb_inst = builderResult.value();

    //store the instance
    _instance = vkb_inst.instance;
    //store the debug messenger
    _debug_messenger = vkb_inst.debug_messenger;

    if (_debug_messenger == nullptr)
    {
        std::cout << "Failed to create debug messenger." << std::endl;
    }

    // Get the surface of the window we opened with SDL
    if (!SDL_Vulkan_CreateSurface(_window, _instance, NULL, &_surface))
    {
        std::cerr << "Failed to create Vulkan surface. SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    //use vkbootstrap to select a GPU.
    //We want a GPU that can write to the SDL surface and supports Vulkan 1.1
    vkb::PhysicalDeviceSelector selector{ vkb_inst };

    // Create a VkPhysicalDeviceFeatures structure and set the fillModeNonSolid feature to VK_TRUE
    VkPhysicalDeviceFeatures desiredFeatures = {};
    desiredFeatures.fillModeNonSolid = VK_TRUE;

    auto phys_ret = selector
        .set_minimum_version(1, 1)
        .set_surface(_surface)
        .set_required_features(desiredFeatures)
        .select();

    // Check if physical device selection was successful before proceeding
    if (!phys_ret)
    {
        std::cerr << "Failed to select Vulkan physical device. Error: " << phys_ret.error().message() << std::endl;
        return false;
    }
    vkb::PhysicalDevice physicalDevice = phys_ret.value();

    // Create the final Vulkan device
    vkb::DeviceBuilder deviceBuilder{ physicalDevice };
    // Automatically propagate needed data from instance & physical device
    auto dev_ret = deviceBuilder.build();
    if (!dev_ret)
    {
        std::cerr << "Failed to create Vulkan device. Error: " << dev_ret.error().message() << std::endl;
        return false;
    }

    vkb::Device vkbDevice = dev_ret.value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    _device = vkbDevice.device;
    _chosenGPU = physicalDevice.physical_device;

    // Use vkbootstrap to get a Graphics queue
    _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    _graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    // Initialize the memory allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _chosenGPU;
    allocatorInfo.device = _device;
    allocatorInfo.instance = _instance;
    vmaCreateAllocator(&allocatorInfo, &_allocator);

    return true;
}

bool RenderEngine::InitSwapchain()
{
    vkb::SwapchainBuilder swapchainBuilder = vkb::SwapchainBuilder{_chosenGPU, _device, _surface};

    // use this if u need to test the Color32 struct, otherwise the displayed color will be slightly different, probably brighter.
    VkSurfaceFormatKHR surfaceFormat = {};
    surfaceFormat.colorSpace = VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT;
    surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;

    vkb::Result<vkb::Swapchain> vkbSwapchainRet = swapchainBuilder
        .set_desired_format(surfaceFormat)
        // .use_default_format_selection()
        .build()
        ;
    
    if (!vkbSwapchainRet.has_value())
    {
        std::cerr << "Failed to create swapchain. VkResult: " << vkbSwapchainRet.vk_result() << std::endl;
        return false;
    }
    
    vkb::Swapchain vkbSwapchain = vkbSwapchainRet.value();

    vkbSwapchain.extent = windowExtent;
    //use vsync present mode
    vkbSwapchain.present_mode = VK_PRESENT_MODE_FIFO_KHR;

    //store swapchain and its related images
    _swapchain = vkbSwapchain.swapchain;
    _swapchainImages = vkbSwapchain.get_images().value();
    _swapchainImageViews = vkbSwapchain.get_image_views().value();

    _swapchainImageFormat = vkbSwapchain.image_format;



    //depth image size will match the window
    VkExtent3D depthImageExtent{
        windowExtent.width,
        windowExtent.height,
        1
    };

    //hardcoding the depth format to 32 bit float
    _depthFormat = VK_FORMAT_D32_SFLOAT;

    //the depth image will be an image with the format we selected and Depth Attachment usage flag
    VkImageCreateInfo dimg_info = VkExtImageCreateInfo(_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

    //for the depth image, we want to allocate it from GPU local memory
    VmaAllocationCreateInfo dimg_allocinfo = {};
    dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    //allocate and create the image
    VkResult result = vmaCreateImage(_allocator, &dimg_info, &dimg_allocinfo, &_depthImage._image, &_depthImage._allocation, nullptr);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create depth image: " << result << std::endl;
        return false;
    }

    //build an image-view for the depth image to use for rendering
    VkImageViewCreateInfo dview_info = VkExtImageviewCreateInfo(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);

    result = vkCreateImageView(_device, &dview_info, nullptr, &_depthImageView);
    if (result)
    {
        std::cerr << "Failed to create Vulkan image view: " << result << std::endl;
        return false;
    }

    //add to deletion queues
    // _mainDeletionQueue.PushDeletor
    // (
    //     [=]()
    //     {
    //         vkDestroyImageView(_device, _depthImageView, nullptr);
    //         vmaDestroyImage(_allocator, _depthImage._image, _depthImage._allocation);
    //     }
    // );

    return true;
}

bool RenderEngine::InitCommands()
{
    // Create a command pool for commands submitted to the graphics queue.
    // We also want the pool to allow for resetting of individual command buffers
    VkCommandPoolCreateInfo commandPoolInfo = VkExtCommandPoolCreateInfo(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkResult result = vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create command pool: " << result << std::endl;
        return false;
    }

    // Allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo cmdAllocInfo = VkExtCommandBufferAllocateInfo(_commandPool, 1);
    result = vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to allocate main command buffer: " << result << std::endl;
        return false;
    }

    // Create pool for upload context
    VkCommandPoolCreateInfo uploadCommandPoolInfo = VkExtCommandPoolCreateInfo(_graphicsQueueFamily);
    result = vkCreateCommandPool(_device, &uploadCommandPoolInfo, nullptr, &_uploadContext._commandPool);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create upload command pool: " << result << std::endl;
        return false;
    }

    _mainDeletionQueue.PushDeletor
    (
        [=]()
        {
            vkDestroyCommandPool(_device, _uploadContext._commandPool, nullptr);
        }
    );

    // Allocate the default command buffer that we will use for the instant commands
    VkCommandBufferAllocateInfo cmdAllocInfo2 = VkExtCommandBufferAllocateInfo(_uploadContext._commandPool, 1);
    result = vkAllocateCommandBuffers(_device, &cmdAllocInfo2, &_uploadContext._commandBuffer);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to allocate upload command buffer: " << result << std::endl;
        return false;
    }

    return true;
}

bool RenderEngine::InitRenderPass()
{
    return true;
}

bool RenderEngine::InitFramebuffers()
{
    return true;
}

bool RenderEngine::InitSyncStructures()
{
    return true;
}

bool RenderEngine::InitPipelines()
{
    return true;
}

void RenderEngine::CleanupVulkan()
{

}

} // namespace velecs::graphics
