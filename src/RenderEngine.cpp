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
#include "velecs/graphics/DescriptorLayoutBuilder.hpp"
#include "velecs/graphics/RenderPipelineLayout.hpp"
#include "velecs/graphics/RenderPipeline.hpp"
#include "velecs/graphics/PipelineBuilder.hpp"
#include "velecs/graphics/Vertex.hpp"
#include "velecs/graphics/Mesh.hpp"
#include "velecs/graphics/Shader.hpp"
#include "velecs/graphics/Shader/Reflection/ShaderReflector.hpp"
#include "velecs/graphics/Components/MeshRenderer.hpp"
#include "velecs/graphics/Components/PerspectiveCamera.hpp"
#include "velecs/graphics/Components/OrthographicCamera.hpp"
#include "velecs/graphics/ObjectUniforms.hpp"

#include <velecs/ecs/Registry.hpp>
#include <velecs/ecs/Entity.hpp>
#include <velecs/ecs/Components/Transform.hpp>
using namespace velecs::ecs;

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <stdexcept>

namespace velecs::graphics {

// Public Fields

const int RenderEngine::VULKAN_MAJOR_VERSION = 1;
const int RenderEngine::VULKAN_MINOR_VERSION = 3;
const int RenderEngine::VULKAN_PATCH_VERSION = 0;

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
    if (!InitSyncStructures()) return SDL_APP_FAILURE;
    if (!InitDescriptors()   ) return SDL_APP_FAILURE;
    if (!InitPipelines()     ) return SDL_APP_FAILURE;

    _wasInitialized = true;

    return SDL_APP_CONTINUE;
}

void RenderEngine::Draw()
{
    // Start the Dear ImGui frame
    // ImGui_ImplVulkan_NewFrame();
    // ImGui_ImplSDL2_NewFrame();
    // ImGui::NewFrame();

    // Wait until the GPU has finished rendering the last frame. Timeout of 1 second
    VkResult result = vkWaitForFences(_device, 1, &(GetCurrentFrame().renderFence), true, 1000000000);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to wait for fences: " << result << std::endl;
        return;
    }

    GetCurrentFrame().deletionQueue.Flush();

    result = vkResetFences(_device, 1, &(GetCurrentFrame().renderFence));
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to reset fences: " << result << std::endl;
        return;
    }

    // Request image from the swapchain
    uint32_t swapchainImageIndex;
    result = vkAcquireNextImageKHR(
        _device,
        _swapchain,
        1000000000,
        GetCurrentFrame().swapchainSemaphore,
        nullptr,
        &swapchainImageIndex
    );
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to acquire next image from swapchain: " << result << std::endl;
        return;
    }

    // Short alias for current frame's main command buffer
    const VkCommandBuffer cmd = GetCurrentFrame().mainCommandBuffer;

    // Now that we are sure that the commands finished executing,
    // we can safely reset the command buffer to begin recording again.
    result = vkResetCommandBuffer(cmd, 0);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to reset command buffer: " << result << std::endl;
        return;
    }

    // Begin the command buffer recording.
    // We will use this command buffer exactly once, so we want to let Vulkan know that
    VkCommandBufferBeginInfo cmdBeginInfo = VkExtCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // Start the command buffer recording
    result = vkBeginCommandBuffer(cmd, &cmdBeginInfo);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to begin command buffer recording: " << result << std::endl;
        return;
    }

    // Change swapchain image's to writeable mode before rendering
    TransitionImage(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    // Make a clear-color from frame number. This will flash with a 120 frame period.
    VkClearColorValue clearValue;
    float flash = std::abs(std::sin(_frameNumber / 120.f));
    clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

    VkImageSubresourceRange clearRange = VkExtImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    // Clear the image
    vkCmdClearColorImage(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

    // vkCmdBindPipeline(_mainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _opaquePipeline);

    // VkViewport viewport = {};
    // viewport.x = 0.0f;
    // viewport.y = 0.0f;
    // auto windowExtent = GetWindowExtent();
    // viewport.width = static_cast<float>(windowExtent.width);
    // viewport.height = static_cast<float>(windowExtent.height);
    // viewport.minDepth = 0.0f;
    // viewport.maxDepth = 1.0f;

    // VkRect2D scissor = {};
    // scissor.offset = {0, 0};
    // scissor.extent = {static_cast<uint32_t>(windowExtent.width), static_cast<uint32_t>(windowExtent.height)};

    // vkCmdSetViewport(_mainCommandBuffer, 0, 1, &viewport);
    // vkCmdSetScissor(_mainCommandBuffer, 0, 1, &scissor);

    // auto& registry = Registry::Get();

    // // Get the first active camera that also has a Transform and throw an error if more are found
    // Camera* cam{nullptr};
    // registry.view<PerspectiveCamera, Transform>().each([&registry, &cam](auto e, auto& pCam, auto& transform){
    //     if (cam) throw std::runtime_error("Currently a single active camera is all that is supported.");
    //     else cam = &pCam;
    // });
    // registry.view<OrthographicCamera, Transform>().each([&cam](auto e, auto& oCam, auto& transform){
    //     if (cam) throw std::runtime_error("Currently a single active camera is all that is supported.");
    //     else cam = &oCam;
    // });

    // if (cam)
    // {
    //     auto view = registry.view<Transform, MeshRenderer>();
    //     view.each([&](auto e, Transform& transform, MeshRenderer& renderer) {
    //         Entity entity{e};

    //         if (renderer.mesh != nullptr)
    //         {
    //             // Upload mesh if dirty
    //             if (renderer.mesh->IsDirty())
    //             {
    //                 renderer.mesh->UploadImmediately(_device, _allocator, [this](std::function<void(VkCommandBuffer)> func) {
    //                     ImmediateSubmit(std::move(func));
    //                 });
    //             }

    //             // ObjectUniforms uniforms{ transform.GetWorldMatrix(), Color32::RED };

    //             // // Pass the descriptor pool and layout
    //             // renderer.mesh->UploadModelUniformsImmediate(
    //             //     _device, 
    //             //     _allocator, 
    //             //     _descriptorPool,                    // Add this
    //             //     _objectDescriptorSetLayout,         // Add this
    //             //     uniforms, 
    //             //     [this](std::function<void(VkCommandBuffer)> func) {
    //             //         ImmediateSubmit(std::move(func));
    //             //     }
    //             // );

    //             // Draw the mesh
    //             renderer.mesh->Draw(_mainCommandBuffer, _opaquePipelineLayout);
    //         }
    //     });
    // }

    // Rendering imgui
    // ImGui::Render();
    // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _mainCommandBuffer);

    // Change swapchain image's to presentable mode
    TransitionImage(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // Finalize the command buffer (we can no longer add commands, but it can now be executed)
    result = vkEndCommandBuffer(cmd);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to end command buffer recording: " << result << std::endl;
        return;
    }

    // Prepare the submission for the queue. 
    // We want to wait on the presentSemaphore, as that semaphore is signaled when the swapchain is ready
    // We will signal the renderSemaphore, to signal that rendering has finished

    VkCommandBufferSubmitInfo cmdinfo = VkExtCommandBufferSubmitInfo(cmd);
    
    VkSemaphoreSubmitInfo waitInfo = VkExtSemaphoreSubmitInfo(
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        GetCurrentFrame().swapchainSemaphore
    );

    VkSemaphoreSubmitInfo signalInfo = VkExtSemaphoreSubmitInfo(
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        _renderSemaphores[swapchainImageIndex]
    );    
    
    VkSubmitInfo2 submit = VkExtSubmitInfo2(&cmdinfo, &signalInfo, &waitInfo);

    // Submit command buffer to the queue and execute it.
    // renderFence will now block until the graphic commands finish execution
    result = vkQueueSubmit2(_graphicsQueue, 1, &submit, GetCurrentFrame().renderFence);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to submit to queue: " << result << std::endl;
        return;
    }

    // Prepare present
    // This will put the image we just rendered to into the visible window.
    // We want to wait on the renderSemaphore for that, 
    // as its necessary that drawing commands have finished before the image is displayed to the user
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &_swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = &(_renderSemaphores[swapchainImageIndex]);
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pImageIndices = &swapchainImageIndex;

    result = vkQueuePresentKHR(_graphicsQueue, &presentInfo);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to present queue: " << result << std::endl;
        return;
    }

    // Increase the number of frames drawn
    _frameNumber++;
}

void RenderEngine::Cleanup()
{
    if (!_wasInitialized) return;

    // Make sure the GPU has stopped doing its things
    vkDeviceWaitIdle(_device);

    for (size_t i{0}; i < FRAME_OVERLAP; ++i)
    {
        FrameData& frame = _frames[i];

        vkDestroyCommandPool(_device, frame.commandPool, nullptr);

        // Also destroy sync objects
        vkDestroyFence(_device, frame.renderFence, nullptr);
        vkDestroySemaphore(_device, frame.swapchainSemaphore, nullptr);

        frame.deletionQueue.Flush();
    }

    const size_t swapchainImagesCount = _swapchainImages.size();
    for (size_t i{0}; i < swapchainImagesCount; ++i)
    {
        vkDestroySemaphore(_device ,_renderSemaphores[i], nullptr);
    }

    _mainDeletionQueue.Flush();

    CleanupSwapchain();

    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyDevice(_device, nullptr);
    
    vkb::destroy_debug_utils_messenger(_instance, _debugMessenger);
    vkDestroyInstance(_instance, nullptr);

    _wasInitialized = false;
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
        .use_default_debug_messenger()
        .require_api_version(VULKAN_MAJOR_VERSION, VULKAN_MINOR_VERSION, VULKAN_PATCH_VERSION)
        .build()
        ;

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
        
        if (!foundValidationLayer)
        {
            std::cerr << "WARNING: Validation layers requested but VK_LAYER_KHRONOS_validation not found!" << std::endl;
        }
    }

    vkb::Instance vkbInstance = builderResult.value();

    // Store the instance
    _instance = vkbInstance.instance;
    // Store the debug messenger
    _debugMessenger = vkbInstance.debug_messenger;

    if (_debugMessenger == nullptr)
    {
        std::cerr << "Failed to create debug messenger." << std::endl;
        return false;
    }

    // Get the surface of the window we opened with SDL
    if (!SDL_Vulkan_CreateSurface(_window, _instance, NULL, &_surface))
    {
        std::cerr << "Failed to create Vulkan surface. SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    VkPhysicalDeviceVulkan13Features desiredFeatures{};
    desiredFeatures.dynamicRendering = true;
    desiredFeatures.synchronization2 = true;

    // Use vkbootstrap to select a GPU.
    // We want a GPU that can write to the SDL surface and supports our version of Vulkan
    vkb::PhysicalDeviceSelector selector{ vkbInstance };
    auto selectorResult = selector
        .set_minimum_version(VULKAN_MAJOR_VERSION, VULKAN_MINOR_VERSION)
        .set_surface(_surface)
        .set_required_features_13(desiredFeatures)
        .select()
        ;

    // Check if physical device selection was successful before proceeding
    if (!selectorResult)
    {
        std::cerr << "Failed to select Vulkan physical device. Error: " << selectorResult.error().message() << std::endl;
        return false;
    }
    vkb::PhysicalDevice physicalDevice = selectorResult.value();

    // Create the final Vulkan device
    vkb::DeviceBuilder deviceBuilder{ physicalDevice };
    // Automatically propagate needed data from instance & physical device
    auto deviceBuilderResult = deviceBuilder.build();
    if (!deviceBuilderResult)
    {
        std::cerr << "Failed to create Vulkan device. Error: " << deviceBuilderResult.error().message() << std::endl;
        return false;
    }

    vkb::Device vkbDevice = deviceBuilderResult.value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    _device = vkbDevice.device;
    _chosenGPU = physicalDevice.physical_device;

    // Use vkbootstrap to get a Graphics queue
    _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    _graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    // // Initialize the memory allocator
    // VmaAllocatorCreateInfo allocatorInfo = {};
    // allocatorInfo.physicalDevice = _chosenGPU;
    // allocatorInfo.device = _device;
    // allocatorInfo.instance = _instance;
    // vmaCreateAllocator(&allocatorInfo, &_allocator);

    return true;
}

bool RenderEngine::InitSwapchain()
{
    VkExtent2D windowExtent = GetWindowExtent();
    CreateSwapchain(windowExtent);

    // Reserve one render semaphore per swapchain image
    // The swapchain does not always equal the number of frames
    // Some GPUs have 2 swapchain images others have 3,
    // My NVIDIA RTX 4070 SUPER has 3 instead of 2.
    const size_t swapchainImagesCount = _swapchainImages.size();
    _renderSemaphores.assign(swapchainImagesCount, VK_NULL_HANDLE);

    // TODO: Should GetWindowExtent() or _swapchainExtent be used?
    windowExtent = _swapchainExtent;

    

    // //depth image size will match the window
    // VkExtent3D depthImageExtent{
    //     windowExtent.width,
    //     windowExtent.height,
    //     1
    // };

    // //hardcoding the depth format to 32 bit float
    // _depthFormat = VK_FORMAT_D32_SFLOAT;

    // //the depth image will be an image with the format we selected and Depth Attachment usage flag
    // VkImageCreateInfo dimg_info = VkExtImageCreateInfo(_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

    // //for the depth image, we want to allocate it from GPU local memory
    // VmaAllocationCreateInfo dimg_allocinfo = {};
    // dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    // dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // //allocate and create the image
    // VkResult result = vmaCreateImage(_allocator, &dimg_info, &dimg_allocinfo, &_depthImage._image, &_depthImage._allocation, nullptr);
    // if (result != VK_SUCCESS)
    // {
    //     std::cerr << "Failed to create depth image: " << result << std::endl;
    //     return false;
    // }

    // //build an image-view for the depth image to use for rendering
    // VkImageViewCreateInfo dview_info = VkExtImageviewCreateInfo(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);

    // result = vkCreateImageView(_device, &dview_info, nullptr, &_depthImageView);
    // if (result != VK_SUCCESS)
    // {
    //     std::cerr << "Failed to create Vulkan image view: " << result << std::endl;
    //     return false;
    // }

    //add to deletion queues
    // _mainDeletionQueue.PushDeleter
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
    VkCommandPoolCreateInfo commandPoolCreateInfo = VkExtCommandPoolCreateInfo(
        _graphicsQueueFamily,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
    );

    for (size_t i{0}; i < FRAME_OVERLAP; ++i)
    {
        VkResult result = vkCreateCommandPool(_device, &commandPoolCreateInfo, nullptr, &_frames[i].commandPool);
        if (result != VK_SUCCESS)
        {
            std::cerr << "Failed to create command pool: " << result << std::endl;
            return false;
        }

        // Allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo cmdAllocInfo = VkExtCommandBufferAllocateInfo(
            _frames[i].commandPool,
            1,
            VK_COMMAND_BUFFER_LEVEL_PRIMARY
        );

        result = vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i].mainCommandBuffer);
        if (result != VK_SUCCESS)
        {
            std::cerr << "Failed to allocate command buffers: " << result << std::endl;
            return false;
        }
    }

    return true;
}

bool RenderEngine::InitSyncStructures()
{
    // Create syncronization structures
    // One fence to control when the gpu has finished rendering the frame,
    // and 2 semaphores to synchronize rendering with swapchain
    // We want the fence to start signalled so we can wait on it on the first frame
    VkFenceCreateInfo fenceCreateInfo = VkExtFenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphoreCreateInfo = VkExtSemaphoreCreateInfo();

    for (size_t i{0}; i < FRAME_OVERLAP; ++i)
    {
        VkResult result = vkCreateFence(_device, &fenceCreateInfo, nullptr, &(_frames[i].renderFence));
        if (result != VK_SUCCESS)
        {
            std::cerr << "Failed to create fence: " << result << std::endl;
            return false;
        }

        result = vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &(_frames[i].swapchainSemaphore));
        if (result != VK_SUCCESS)
        {
            std::cerr << "Failed to create semaphore: " << result << std::endl;
            return false;
        }
    }

    const size_t swapchainImagesCount = _swapchainImages.size();
    for (size_t i{0}; i < swapchainImagesCount; ++i)
    {
        VkResult result = vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphores[i]);
        if (result != VK_SUCCESS)
        {
            std::cerr << "Failed to create semaphore: " << result << std::endl;
            return false;
        }
    }

    return true;
}

bool RenderEngine::InitDescriptors()
{
    // //create a descriptor pool that will hold 10 sets with 1 image each
    // std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
    // {
    //     { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
    // };

    // _descriptorAllocator.InitPool(_device, 10, sizes);

    // _objectDescriptorSetLayout = DescriptorLayoutBuilder{}
    //     .AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
    //     .Build(_device, VK_SHADER_STAGE_COMPUTE_BIT)
    //     ;
    
    // _objectDescriptorSet = _descriptorAllocator.Allocate(_device, _objectDescriptorSetLayout);

    // VkDescriptorImageInfo imgInfo{};
    // imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    // imgInfo.imageView = _drawImage.imageView;
    
    // VkWriteDescriptorSet drawImageWrite = {};
    // drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    // drawImageWrite.pNext = nullptr;
    
    // drawImageWrite.dstBinding = 0;
    // drawImageWrite.dstSet = _drawImageDescriptors;
    // drawImageWrite.descriptorCount = 1;
    // drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    // drawImageWrite.pImageInfo = &imgInfo;

    // vkUpdateDescriptorSets(_device, 1, &drawImageWrite, 0, nullptr);

    // //make sure both the descriptor allocator and the new layout get cleaned up properly
    // _mainDeletionQueue.push_function([&]() {
    //     globalDescriptorAllocator.destroy_pool(_device);

    //     vkDestroyDescriptorSetLayout(_device, _drawImageDescriptorLayout, nullptr);
    // });

    return true;
}

bool RenderEngine::InitPipelines()
{
    RasterizationShaderProgram program{};
    program.vert = Shader::FromFile(_device, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, "internal/shaders/basic.vert.spv");
    program.frag = Shader::FromFile(_device, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, "internal/shaders/basic.frag.spv");

    // Material mat{};
    // mat.shaderProgram = program;
    // mat.fields == ShaderReflector::Merge(ShaderReflector::Reflect(program.vert + program.frag + ...));

    // // Define the uniform buffer binding
    // VkDescriptorSetLayoutBinding objectUboBinding{};
    // objectUboBinding.binding = 0;
    // objectUboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // objectUboBinding.descriptorCount = 1;
    // objectUboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    // objectUboBinding.pImmutableSamplers = nullptr;

    // VkDescriptorSetLayoutCreateInfo layoutInfo{};
    // layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    // layoutInfo.bindingCount = 1;
    // layoutInfo.pBindings = &objectUboBinding;

    // VkResult result = vkCreateDescriptorSetLayout(_device, &layoutInfo, nullptr, &_objectDescriptorSetLayout);
    // if (result != VK_SUCCESS) {
    //     std::cerr << "Failed to create descriptor set layout: " << result << std::endl;
    //     return false;
    // }

    // VkDescriptorPoolSize poolSize{};
    // poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // poolSize.descriptorCount = 1000;

    // VkDescriptorPoolCreateInfo poolInfo{};
    // poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    // poolInfo.maxSets = 1000;
    // poolInfo.poolSizeCount = 1;
    // poolInfo.pPoolSizes = &poolSize;

    // result = vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descriptorPool);
    // if (result != VK_SUCCESS)
    // {
    //     std::cerr << "Failed to create a descriptor pool: " << result << std::endl;
    //     return false;
    // }

    // RenderPipelineLayout pipelineLayout{};
    // pipelineLayout.SetDevice(_device)
    //     // .AddDescriptorSetLayout(_objectDescriptorSetLayout)
    //     ;
    // _opaquePipelineLayout = pipelineLayout.GetLayout();

    // RenderPipeline pipeline{};
    // pipeline.SetDevice(_device)
    //     .SetRenderPass(_renderPass)
    //     .SetViewport(GetWindowExtent())
    //     .SetPipelineLayout(_opaquePipelineLayout)
    //     .SetVertexInput(Vertex::GetVertexInputInfo())
    //     .AddShader(*program.vert.get())
    //     .AddShader(*program.frag.get())
    //     .SetCullMode(VK_CULL_MODE_NONE)
    //     ;
    // _opaquePipeline = pipeline.GetPipeline();

    // //build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
    // PipelineBuilder pipelineBuilder;

    // //vertex input controls how to read vertices from vertex buffers. We aren't using it yet
    // pipelineBuilder._vertexInputInfo = VkExtVertexInputStateCreateInfo();

    // //input assembly is the configuration for drawing triangle lists, strips, or individual points.
    // //we are just going to draw triangle list
    // pipelineBuilder._inputAssembly = VkExtInputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    // //build viewport and scissor from the swapchain extents
    // pipelineBuilder._viewport.x = 0.0f;
    // pipelineBuilder._viewport.y = 0.0f;
    // VkExtent2D windowExtent = GetWindowExtent();
    // pipelineBuilder._viewport.width = static_cast<float>(windowExtent.width);
    // pipelineBuilder._viewport.height = static_cast<float>(windowExtent.height);
    // pipelineBuilder._viewport.minDepth = 0.0f;
    // pipelineBuilder._viewport.maxDepth = 1.0f;

    // pipelineBuilder._scissor.offset = { 0, 0 };
    // pipelineBuilder._scissor.extent = windowExtent;

    // //configure the rasterizer to draw filled triangles
    // pipelineBuilder._rasterizer = VkExtRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);

    // //we don't use multisampling, so just run the default one
    // pipelineBuilder._multisampling = VkExtMultisamplingStateCreateInfo();

    // //a single blend attachment with no blending and writing to RGBA
    // pipelineBuilder._colorBlendAttachment = VkExtColorBlendAttachmentState();

    // pipelineBuilder._depthStencil = VkExtDepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);












    // //we start from just the default empty pipeline layout info
    // VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = VkExtPipelineLayoutCreateInfo();

    // VkResult result = vkCreatePipelineLayout(_device, &mesh_pipeline_layout_info, nullptr, &_meshPipelineLayout);
    // if (result != VK_SUCCESS)
    // {
    //     std::cerr << "Failed to create mesh pipeline layout: " << result << std::endl;
    //     return false;
    // }

    // pipelineBuilder._vertexInputInfo = Vertex::GetVertexInputInfo();

    // // //add the other shaders
    // // const ShaderModule meshVertShader = ShaderModule::CreateVertShader(_device, "Mesh/Mesh.vert.spv");
    // // pipelineBuilder._shaderStages.push_back(meshVertShader.pipelineShaderStageCreateInfo);

    // // //make sure that triangleFragShader is holding the compiled colored_triangle.frag
    // // const ShaderModule meshFragShader = ShaderModule::CreateFragShader(_device, "Mesh/Mesh.frag.spv");
    // // pipelineBuilder._shaderStages.push_back(meshFragShader.pipelineShaderStageCreateInfo);

    // pipelineBuilder._pipelineLayout = _meshPipelineLayout;

    // _meshPipeline = pipelineBuilder.BuildPipeline(_device, _renderPass);

    // // Material::Create(ecs(), "Mesh/Mesh", &_meshPipeline, &_meshPipelineLayout);

    // //clear the shader stages for the builder
    // pipelineBuilder._shaderStages.clear();

    







    // //we start from just the default empty pipeline layout info
    // VkPipelineLayoutCreateInfo simple_mesh_pipeline_layout_info = VkExtPipelineLayoutCreateInfo();

    // //setup push constants
    // VkPushConstantRange simple_mesh_push_constant = {};
    // //this push constant range starts at the beginning
    // simple_mesh_push_constant.offset = 0;
    // //this push constant range takes up the size of a MeshPushConstants struct
    // simple_mesh_push_constant.size = sizeof(MeshPushConstants);
    // //this push constant range is accessible only in the vertex shader
    // simple_mesh_push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;  // Accessible from both stages

    // simple_mesh_pipeline_layout_info.pPushConstantRanges = &simple_mesh_push_constant;
    // simple_mesh_pipeline_layout_info.pushConstantRangeCount = 1;

    // result = vkCreatePipelineLayout(_device, &simple_mesh_pipeline_layout_info, nullptr, &simpleMeshPipelineLayout);
    // if (result != VK_SUCCESS)
    // {
    //     std::cerr << "Failed to create simple mesh pipeline layout: " << result << std::endl;
    //     return false;
    // }

    // VertexInputAttributeDescriptor simpleMeshVertexDescription = SimpleVertex::GetVertexDescription();

    // //connect the pipeline builder vertex input info to the one we get from Vertex
    // pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = simpleMeshVertexDescription.attributes.data();
    // pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)simpleMeshVertexDescription.attributes.size();

    // pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = simpleMeshVertexDescription.bindings.data();
    // pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)simpleMeshVertexDescription.bindings.size();

    
    // const ShaderModule simpleMeshVertShader = ShaderModule::CreateVertShader(_device, "SimpleMesh/SolidColor.vert.spv");
    // pipelineBuilder._shaderStages.push_back(simpleMeshVertShader.pipelineShaderStageCreateInfo);

    // //make sure that triangleFragShader is holding the compiled colored_triangle.frag
    // const ShaderModule simpleMeshFragShader = ShaderModule::CreateFragShader(_device, "SimpleMesh/SolidColor.frag.spv");
    // pipelineBuilder._shaderStages.push_back(simpleMeshFragShader.pipelineShaderStageCreateInfo);

    // pipelineBuilder._pipelineLayout = simpleMeshPipelineLayout;

    // //build the mesh triangle pipeline
    // simpleMeshPipeline = pipelineBuilder.BuildPipeline(_device, _renderPass);

    // Material::Create(ecs(), "SimpleMesh/SolidColor", &simpleMeshPipeline, &simpleMeshPipelineLayout);

    // pipelineBuilder._shaderStages.clear();



    // //add the other shaders
    // const ShaderModule rainbowTriangleVertShader = ShaderModule::CreateVertShader(_device, "SimpleMesh/Rainbow.vert.spv");
    // pipelineBuilder._shaderStages.push_back(rainbowTriangleVertShader.pipelineShaderStageCreateInfo);
    // const ShaderModule rainbowTriangleFragShader = ShaderModule::CreateFragShader(_device, "SimpleMesh/Rainbow.frag.spv");
    // pipelineBuilder._shaderStages.push_back(rainbowTriangleFragShader.pipelineShaderStageCreateInfo);

    // _rainbowSimpleMeshPipeline = pipelineBuilder.BuildPipeline(_device, _renderPass);

    // Material::Create(ecs(), "SimpleMesh/Rainbow", &_rainbowSimpleMeshPipeline, &simpleMeshPipelineLayout);

    return true;
}

VkExtent2D RenderEngine::GetWindowExtent() const
{
    int width, height;
    SDL_GetWindowSize(_window, &width, &height);
    return VkExtent2D{
        static_cast<uint32_t>(width), 
        static_cast<uint32_t>(height)
    };
}

void RenderEngine::CreateSwapchain(const VkExtent2D extent)
{
    vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU, _device, _surface };

    _swapchainImageFormat = VK_FORMAT_B8G8R8A8_SNORM;

    VkSurfaceFormatKHR surfaceFormat{};
    surfaceFormat.format = _swapchainImageFormat;
    surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    auto swapchainResult = swapchainBuilder
        .set_desired_format(surfaceFormat)
        // Using v-sync present mode
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(extent.width, extent.height)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        ;

    // Check if swapchain creation was successful before proceeding
    if (!swapchainResult)
    {
        std::cerr << "Failed to create a swapchain. Error: " << swapchainResult.error().message() << std::endl;
        return;
    }
    
    vkb::Swapchain vkbSwapchain = swapchainResult.value();
    
    // Store the swapchain and its related images and extent
    _swapchainExtent = vkbSwapchain.extent;
    _swapchain = vkbSwapchain.swapchain;
    _swapchainImages = vkbSwapchain.get_images().value();
    _swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void RenderEngine::CleanupSwapchain()
{
    vkDestroySwapchainKHR(_device, _swapchain, nullptr);

    // Destroy swapchain resources
    for (size_t i = 0; i < _swapchainImageViews.size(); ++i)
    {
        vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
    }
}

FrameData& RenderEngine::GetCurrentFrame()
{
    return _frames[_frameNumber % FRAME_OVERLAP];
}

void RenderEngine::TransitionImage(
    const VkCommandBuffer cmd,
    const VkImage image,
    const VkImageLayout currentLayout,
    const VkImageLayout newLayout
)
{
    VkImageMemoryBarrier2 imageBarrier{};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    imageBarrier.pNext = nullptr;
    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
    imageBarrier.oldLayout = currentLayout;
    imageBarrier.newLayout = newLayout;
    VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
        ? VK_IMAGE_ASPECT_DEPTH_BIT
        : VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange = VkExtImageSubresourceRange(aspectMask);
    imageBarrier.image = image;

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;
    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;
    vkCmdPipelineBarrier2(cmd, &depInfo);
}

void RenderEngine::ImmediateSubmit(std::function<void(VkCommandBuffer)>&& function)
{
    // VkCommandBuffer cmd = _uploadContext._commandBuffer;
    
    // VkCommandBufferBeginInfo cmdBeginInfo = VkExtCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    // VkResult result = vkBeginCommandBuffer(cmd, &cmdBeginInfo);
    // if (result != VK_SUCCESS)
    // {
    //     std::cerr << "Failed to begin command buffer: " << result << std::endl;
    //     return;
    // }
    
    // function(cmd);
    
    // result = vkEndCommandBuffer(cmd);
    // if (result != VK_SUCCESS)
    // {
    //     std::cerr << "Failed to end command buffer: " << result << std::endl;
    //     return;
    // }
    
    // VkSubmitInfo submit = VkExtSubmitInfo(&cmd);
    // result = vkQueueSubmit(_graphicsQueue, 1, &submit, _uploadContext._uploadFence);
    // if (result != VK_SUCCESS)
    // {
    //     std::cerr << "Failed to submit to queue: " << result << std::endl;
    //     return;
    // }
    
    // vkWaitForFences(_device, 1, &_uploadContext._uploadFence, true, 9999999999);
    // vkResetFences(_device, 1, &_uploadContext._uploadFence);
    // vkResetCommandPool(_device, _uploadContext._commandPool, 0);
}

} // namespace velecs::graphics
