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

#include <iostream>
#include <fstream>
#include <chrono>

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace velecs::graphics {

// Public Fields

const bool RenderEngine::ENABLE_VALIDATION_LAYERS
#ifdef DEBUG_MODE
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

    // #ifdef DEBUG_MODE
    // // After building the instance, but before creating the device:
    // if (builderResult)
    // {
    //     std::cout << "Available validation layers:" << std::endl;
    //     uint32_t layerCount;
    //     vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    //     std::vector<VkLayerProperties> availableLayers(layerCount);
    //     vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    //     for (const auto& layer : availableLayers)
    //     {
    //         std::cout << "\t" << layer.layerName << std::endl;
    //     }
    // }
    // #endif

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
    return true;
}

bool RenderEngine::InitCommands()
{
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
