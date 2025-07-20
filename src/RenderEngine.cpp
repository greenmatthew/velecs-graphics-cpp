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
#include "velecs/graphics/PipelineBuilder.hpp"
#include "velecs/graphics/RenderPipelineLayout.hpp"
#include "velecs/graphics/RenderPipeline.hpp"
#include "velecs/graphics/Vertex.hpp"
#include "velecs/graphics/Shader.hpp"
#include "velecs/graphics/Shader/Reflection/ShaderReflector.hpp"

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

    CreateTriangleBuffers();
    
    return SDL_APP_CONTINUE;
}

void RenderEngine::Draw()
{
    PreDraw();

    vkCmdBindPipeline(_mainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _vertexColorsPipeline);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    auto windowExtent = GetWindowExtent();
    viewport.width = static_cast<float>(windowExtent.width);
    viewport.height = static_cast<float>(windowExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = {static_cast<uint32_t>(windowExtent.width), static_cast<uint32_t>(windowExtent.height)};

    vkCmdSetViewport(_mainCommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(_mainCommandBuffer, 0, 1, &scissor);

    //bind the mesh vertex buffer with offset 0
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(_mainCommandBuffer, 0, 1, &_triangleVertexBuffer._buffer, &offset);

    vkCmdBindIndexBuffer(_mainCommandBuffer, _triangleIndexBuffer._buffer, 0, VK_INDEX_TYPE_UINT16);

    //we can now draw the mesh
    vkCmdDrawIndexed(_mainCommandBuffer, (uint32_t)3, 1, 0, 0, 0);

    PostDraw();
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

    VkExtent2D windowExtent = GetWindowExtent();
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
    if (result != VK_SUCCESS)
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
    // ATTACHMENTS

    VkAttachmentDescription color_attachment = {};
    
    color_attachment.format = _swapchainImageFormat; // The attachment will have the format needed by the swapchain
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT; // 1 sample, we won't be doing MSAA 
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;  // We Clear when this attachment is loaded
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // We keep the attachment stored when the renderpass ends
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // We don't care about stencil
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // We don't care about stencil
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // We don't know nor care about the starting layout of the attachment
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // After the renderpass ends, the image has to be on a layout ready for display

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0; // Attachment number will index into the pAttachments array in the parent renderpass itself
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    VkAttachmentDescription depth_attachment = {};
    depth_attachment.flags = 0;
    depth_attachment.format = _depthFormat;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref = {};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Array of 2 attachments, one for the color, and other for depth
    VkAttachmentDescription attachments[2] = { color_attachment, depth_attachment };

    // SUBPASS

    // We are going to create 1 subpass, which is the minimum you can do
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref; // Hook the depth attachment into the subpass

    // DEPENDENCIES

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency depth_dependency = {};
    depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depth_dependency.dstSubpass = 0;
    depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.srcAccessMask = 0;
    depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency dependencies[2] = { dependency, depth_dependency };

    // RENDER PASS

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 2;
    render_pass_info.pAttachments = &attachments[0];
    render_pass_info.subpassCount = 1; // Connect the subpass to the info
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 2;
    render_pass_info.pDependencies = &dependencies[0];

    VkResult result = vkCreateRenderPass(_device, &render_pass_info, nullptr, &_renderPass);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create render pass: " << result << std::endl;
        return false;
    }

    return true;
}

bool RenderEngine::InitFramebuffers()
{
    // Create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;

    fb_info.renderPass = _renderPass;
    fb_info.attachmentCount = 1;
    VkExtent2D windowExtent = GetWindowExtent();
    fb_info.width = windowExtent.width;
    fb_info.height = windowExtent.height;
    fb_info.layers = 1;

    // Grab how many images we have in the swapchain
    const size_t swapchain_imagecount = _swapchainImages.size();
    _framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

    // Create framebuffers for each of the swapchain image views
    for (int i = 0; i < swapchain_imagecount; i++)
    {
        VkImageView attachments[2];
        attachments[0] = _swapchainImageViews[i];
        attachments[1] = _depthImageView;

        fb_info.pAttachments = attachments;
        fb_info.attachmentCount = 2;

        VkResult result = vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]);
        if (result != VK_SUCCESS)
        {
            std::cerr << "Failed to create framebuffer " << i << ": " << result << std::endl;
            return false;
        }
    }

    return true;
}

bool RenderEngine::InitSyncStructures()
{
    // We want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
    VkFenceCreateInfo fenceCreateInfo = VkExtFenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

    VkResult result = vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create render fence: " << result << std::endl;
        return false;
    }

    VkFenceCreateInfo uploadFenceCreateInfo = VkExtFenceCreateInfo();

    result = vkCreateFence(_device, &uploadFenceCreateInfo, nullptr, &_uploadContext._uploadFence);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create upload fence: " << result << std::endl;
        return false;
    }

    // For the semaphores we don't need any flags
    VkSemaphoreCreateInfo semaphoreCreateInfo = VkExtSemaphoreCreateInfo();

    result = vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create present semaphore: " << result << std::endl;
        return false;
    }

    result = vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create render semaphore: " << result << std::endl;
        return false;
    }

    _mainDeletionQueue.PushDeletor
    (
        [=]()
        {
            vkDestroyFence(_device, _uploadContext._uploadFence, nullptr);
            vkDestroyFence(_device, _renderFence, nullptr);

            vkDestroySemaphore(_device, _presentSemaphore, nullptr);
            vkDestroySemaphore(_device, _renderSemaphore, nullptr);
        }
    );

    return true;
}

bool RenderEngine::InitPipelines()
{
    // RasterizationShaderProgram program{};
    // program.vert = Shader::FromFile(_device, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, "Engine/shaders/test.vert.spv");
    // program.frag = Shader::FromFile(_device, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, "Engine/shaders/test.frag.spv");

    // Material mat{};
    // mat.shaderProgram = program;
    // mat.fields == ShaderReflector::Merge(ShaderReflector::Reflect(program.vert + program.frag + ...));

    RasterizationShaderProgram vertexColorsProgram{};
    vertexColorsProgram.vert = Shader::FromFile(_device, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, "Engine/shaders/vertex_colors.vert.spv");
    vertexColorsProgram.frag = Shader::FromFile(_device, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, "Engine/shaders/vertex_colors.frag.spv");

    RenderPipelineLayout pipelineLayout{};
    pipelineLayout.SetDevice(_device);

    RenderPipeline pipeline{};
    pipeline.SetDevice(_device)
        .SetRenderPass(_renderPass)
        .SetViewport(GetWindowExtent())
        .SetPipelineLayout(pipelineLayout.GetLayout())
        .SetVertexInput(Vertex::GetVertexInputInfo())
        .AddShader(*vertexColorsProgram.vert)
        .AddShader(*vertexColorsProgram.frag)
        ;
    _vertexColorsPipeline = pipeline.GetPipeline();

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

void RenderEngine::CleanupVulkan()
{

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

void RenderEngine::ImmediateSubmit(std::function<void(VkCommandBuffer)>&& function)
{
    VkCommandBuffer cmd = _uploadContext._commandBuffer;
    
    VkCommandBufferBeginInfo cmdBeginInfo = VkExtCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VkResult result = vkBeginCommandBuffer(cmd, &cmdBeginInfo);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to begin command buffer: " << result << std::endl;
        return;
    }
    
    function(cmd);
    
    result = vkEndCommandBuffer(cmd);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to end command buffer: " << result << std::endl;
        return;
    }
    
    VkSubmitInfo submit = VkExtSubmitInfo(&cmd);
    result = vkQueueSubmit(_graphicsQueue, 1, &submit, _uploadContext._uploadFence);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to submit to queue: " << result << std::endl;
        return;
    }
    
    vkWaitForFences(_device, 1, &_uploadContext._uploadFence, true, 9999999999);
    vkResetFences(_device, 1, &_uploadContext._uploadFence);
    vkResetCommandPool(_device, _uploadContext._commandPool, 0);
}

void RenderEngine::CreateTriangleBuffers()
{
    // Triangle data
    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, Color32::RED},    // 0: bottom-left
        {{ 0.5f, -0.5f, 0.0f}, Color32::GREEN},  // 1: bottom-right
        {{ 0.0f,  0.5f, 0.0f}, Color32::BLUE}    // 2: top
    };
    
    // Counter-clockwise winding: 0 → 2 → 1 (bottom-left → top → bottom-right)
    std::vector<uint16_t> indices = {0, 2, 1};

    
    // Create buffers with error handling
    auto vertexBufferOpt = CreateBuffer(vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    if (!vertexBufferOpt.has_value()) {
        throw std::runtime_error("Failed to create vertex buffer");
    }
    _triangleVertexBuffer = vertexBufferOpt.value();
    
    auto indexBufferOpt = CreateBuffer(indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    if (!indexBufferOpt.has_value()) {
        throw std::runtime_error("Failed to create index buffer");
    }
    _triangleIndexBuffer = indexBufferOpt.value();
}

void RenderEngine::PreDraw()
{
    // Start the Dear ImGui frame
    // ImGui_ImplVulkan_NewFrame();
    // ImGui_ImplSDL2_NewFrame();
    // ImGui::NewFrame();

    //wait until the GPU has finished rendering the last frame. Timeout of 1 second
    VkResult result = vkWaitForFences(_device, 1, &_renderFence, true, 1000000000);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to wait for fence: " << result << std::endl;
        return;
    }

    result = vkResetFences(_device, 1, &_renderFence);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to reset fence: " << result << std::endl;
        return;
    }

    //request image from the swapchain, one second timeout
    result = vkAcquireNextImageKHR(_device, _swapchain, 1000000000, _presentSemaphore, nullptr, &swapchainImageIndex);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to acquire image from swapchain: " << result << std::endl;
        return;
    }

    //now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
    result = vkResetCommandBuffer(_mainCommandBuffer, 0);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to reset command buffer: " << result << std::endl;
        return;
    }

    //begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
    VkCommandBufferBeginInfo cmdBeginInfo = {};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBeginInfo.pNext = nullptr;

    cmdBeginInfo.pInheritanceInfo = nullptr;
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    result = vkBeginCommandBuffer(_mainCommandBuffer, &cmdBeginInfo);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to begin command buffer: " << result << std::endl;
        return;
    }

    VkClearValue clearValue = {};
    // float flash = abs(sin(_frameNumber / 3840.f));
    // clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };
    Color32 color = Color32::FromHex("#181818");
    clearValue.color = { { color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f } };

    //clear depth at 1
	VkClearValue depthClear;
	depthClear.depthStencil.depth = 1.f;

    VkClearValue clearValues[] = { clearValue, depthClear };

    //start the main renderpass.
    //We will use the clear color from above, and the framebuffer of the index the swapchain gave us
    VkRenderPassBeginInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.pNext = nullptr;

    rpInfo.renderPass = _renderPass;
    rpInfo.renderArea.offset.x = 0;
    rpInfo.renderArea.offset.y = 0;
    rpInfo.renderArea.extent = GetWindowExtent();
    rpInfo.framebuffer = _framebuffers[swapchainImageIndex];

    //connect clear values
    rpInfo.clearValueCount = 2;
    rpInfo.pClearValues = &clearValues[0];

    vkCmdBeginRenderPass(_mainCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderEngine::PostDraw()
{
    // Rendering imgui
    // ImGui::Render();
    // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _mainCommandBuffer);

    //finalize the render pass
    vkCmdEndRenderPass(_mainCommandBuffer);
    //finalize the command buffer (we can no longer add commands, but it can now be executed)
    VkResult result = vkEndCommandBuffer(_mainCommandBuffer);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to end command buffer: " << result << std::endl;
        return;
    }


    //prepare the submission to the queue.
    //we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
    //we will signal the _renderSemaphore, to signal that rendering has finished

    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.pNext = nullptr;

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit.pWaitDstStageMask = &waitStage;

    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &_presentSemaphore;

    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &_renderSemaphore;

    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &_mainCommandBuffer;

    //submit command buffer to the queue and execute it.
    // _renderFence will now block until the graphic commands finish execution
    result = vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to submit queue: " << result << std::endl;
        return;
    }


    // this will put the image we just rendered into the visible window.
    // we want to wait on the _renderSemaphore for that,
    // as it's necessary that drawing commands have finished before the image is displayed to the user
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;

    presentInfo.pSwapchains = &_swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &_renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainImageIndex;

    result = vkQueuePresentKHR(_graphicsQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        // Ignore for now
    }
    else if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to present queue: " << result << std::endl;
        return;
    }

    // TODO: replace with multiple semaphores... one for each frame buffer
    vkDeviceWaitIdle(_device);
}

} // namespace velecs::graphics
