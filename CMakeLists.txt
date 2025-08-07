# @file    CMakeLists.txt
# @author  Matthew Green
# @date    2023-11-30 13:49:39
# 
# @section LICENSE
# 
# Copyright (c) 2023 Matthew Green - All rights reserved
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential

cmake_minimum_required(VERSION 3.14)
project(velecs-graphics VERSION 0.1.0)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories to bin if its the cmake build source
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # Only set output directories when building this library standalone
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Vulkan SDK
find_package(Vulkan REQUIRED)

# Verify we have the SDK path for additional components
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable not set")
endif()

set(VULKAN_SDK_PATH $ENV{VULKAN_SDK})

# Create individual interface libraries for SDK components
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE 
    ${VULKAN_SDK_PATH}/Include/vma
)

add_library(volk INTERFACE)
target_include_directories(volk INTERFACE 
    ${VULKAN_SDK_PATH}/Include/Volk
)
target_link_libraries(volk INTERFACE 
    ${VULKAN_SDK_PATH}/Lib/volk.lib
)

# Configure vk-bootstrap options before adding subdirectory
set(VK_BOOTSTRAP_TEST OFF CACHE BOOL "Disable vk-bootstrap tests")
set(VK_BOOTSTRAP_INSTALL OFF CACHE BOOL "Disable vk-bootstrap install")

# Add vk-bootstrap
add_subdirectory(libs/vk-bootstrap)

add_library(spirv_cross INTERFACE)
target_include_directories(spirv_cross INTERFACE 
    ${VULKAN_SDK_PATH}/Include/spirv_cross
)
target_link_libraries(spirv_cross INTERFACE 
    $<$<CONFIG:Debug>:${VULKAN_SDK_PATH}/Lib/spirv-cross-cored.lib>
    $<$<CONFIG:Release>:${VULKAN_SDK_PATH}/Lib/spirv-cross-core.lib>
)

# Configure Assimp options BEFORE adding the subdirectory
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Assimp as static library")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Don't build assimp tools")
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "Don't build assimp samples") 
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Don't build assimp tests")
set(ASSIMP_INSTALL OFF CACHE BOOL "Don't install assimp")
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Don't treat warnings as errors")
set(ASSIMP_INJECT_DEBUG_POSTFIX ON CACHE BOOL "Add 'd' postfix to debug builds")

# Graphics engine specific options
set(ASSIMP_NO_EXPORT ON CACHE BOOL "Build import-only version (smaller)")
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Build assimp's own zlib")
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "Don't build exporters")

# Add Assimp
add_subdirectory(libs/assimp)

# Restore BUILD_SHARED_LIBS to not affect other libraries
set(BUILD_SHARED_LIBS OFF)

# Add other velecs dependencies
get_property(VELECS_DEPS_LOADED GLOBAL PROPERTY VELECS_DEPS_LOADED)
if(NOT VELECS_DEPS_LOADED)
    add_subdirectory(../velecs-deps ${CMAKE_BINARY_DIR}/velecs-deps)
endif()

if(NOT TARGET velecs-common)
    add_subdirectory(../velecs-common ${CMAKE_BINARY_DIR}/velecs-common)
endif()

if(NOT TARGET velecs-math)
    add_subdirectory(../velecs-math ${CMAKE_BINARY_DIR}/velecs-math)
endif()

if(NOT TARGET velecs-ecs)
    add_subdirectory(../velecs-ecs ${CMAKE_BINARY_DIR}/velecs-ecs)
endif()

# ImGui sources - compile directly into our library
set(IMGUI_SOURCES
    libs/imgui/imgui.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    # Add backend sources for SDL3 and Vulkan
    libs/imgui/backends/imgui_impl_sdl3.cpp
    libs/imgui/backends/imgui_impl_vulkan.cpp
)

set(IMGUI_HEADERS
    libs/imgui/imgui.h
    libs/imgui/imgui_internal.h
    libs/imgui/imconfig.h
    libs/imgui/imstb_rectpack.h
    libs/imgui/imstb_textedit.h
    libs/imgui/imstb_truetype.h
    # Backend headers
    libs/imgui/backends/imgui_impl_sdl3.h
    libs/imgui/backends/imgui_impl_vulkan.h
)

# Source files for the library
set(LIB_SOURCES
    src/RenderEngine.cpp

    # Vulkan Memory Management
    src/Memory/AllocatedBuffer.cpp
    src/Memory/DeletionQueue.cpp
    src/Memory/DescriptorAllocator.cpp

    # Render Pipeline
    src/VulkanInitializers.cpp
    src/RenderPipelineLayoutBuilder.cpp
    src/RenderPipelineBuilder.cpp
    src/ComputePipelineBuilder.cpp
    src/PipelineBuilder.cpp
    src/VertexBufferParamsBuilder.cpp
    src/DescriptorLayoutBuilder.cpp

    # Meshes
    src/Mesh.cpp
    src/Material.cpp

    # Cameras
    src/Components/Camera.cpp
    src/Components/PerspectiveCamera.cpp
    src/Components/OrthographicCamera.cpp

    # Shaders
    src/Shader/PushConstant.cpp
    src/Shader/ShaderPrograms/ShaderProgramBase.cpp
    src/Shader/Shaders/Shader.cpp
    src/Shader/ShaderPrograms/RasterizationShaderProgram.cpp
    src/Shader/Shaders/VertexShader.cpp
    src/Shader/Shaders/GeometryShader.cpp
    src/Shader/Shaders/FragmentShader.cpp
    src/Shader/Shaders/TessellationControlShader.cpp
    src/Shader/Shaders/TessellationEvaluationShader.cpp
    src/Shader/ShaderPrograms/ComputeShaderProgram.cpp
    src/Shader/Shaders/ComputeShader.cpp
    # Shader Reflection
    src/Shader/Reflection/ShaderMember.cpp
    src/Shader/Reflection/ShaderResource.cpp
    src/Shader/Reflection/ShaderReflectionData.cpp
    src/Shader/Reflection/ShaderReflector.cpp
    
    src/Color32.cpp

    # Include ImGui sources directly
    ${IMGUI_SOURCES}
)

# Header files for the library (for IDE organization)  
set(LIB_HEADERS
    include/velecs/graphics/Common.hpp

    include/velecs/graphics/RenderEngine.hpp

    # Vulkan Memory Management
    include/velecs/graphics/Memory/AllocatedBuffer.hpp
    include/velecs/graphics/Memory/AllocatedImage.hpp
    include/velecs/graphics/Memory/DeletionQueue.hpp
    include/velecs/graphics/Memory/UploadContext.hpp
    include/velecs/graphics/Memory/DescriptorAllocator.hpp

    # Render Pipeline
    include/velecs/graphics/VulkanInitializers.hpp
    include/velecs/graphics/PipelineBuilderBase.hpp
    include/velecs/graphics/RenderPipelineBuilder.hpp
    include/velecs/graphics/RenderPipelineLayoutBuilder.hpp
    include/velecs/graphics/ComputePipelineBuilder.hpp
    include/velecs/graphics/PipelineBuilder.hpp
    include/velecs/graphics/VertexBufferParamsBuilder.hpp
    include/velecs/graphics/DescriptorLayoutBuilder.hpp

    # Meshes
    include/velecs/graphics/Components/MeshRenderer.hpp
    include/velecs/graphics/MeshBase.hpp
    include/velecs/graphics/Mesh.hpp
    include/velecs/graphics/Vertex.hpp
    include/velecs/graphics/Material.hpp
    include/velecs/graphics/ObjectUniforms.hpp

    # Cameras
    include/velecs/graphics/Components/Camera.hpp
    include/velecs/graphics/Components/PerspectiveCamera.hpp
    include/velecs/graphics/Components/OrthographicCamera.hpp

    # Shaders
    include/velecs/graphics/Shader.hpp
    include/velecs/graphics/Shader/PushConstant.hpp
    include/velecs/graphics/Shader/ShaderPrograms/ShaderProgramBase.hpp
    include/velecs/graphics/Shader/Shaders/Shader.hpp
    include/velecs/graphics/Shader/ShaderPrograms/RasterizationShaderProgram.hpp
    include/velecs/graphics/Shader/Shaders/VertexShader.hpp
    include/velecs/graphics/Shader/Shaders/GeometryShader.hpp
    include/velecs/graphics/Shader/Shaders/FragmentShader.hpp
    include/velecs/graphics/Shader/Shaders/TessellationControlShader.hpp
    include/velecs/graphics/Shader/Shaders/TessellationEvaluationShader.hpp
    include/velecs/graphics/Shader/ShaderPrograms/ComputeShaderProgram.hpp
    include/velecs/graphics/Shader/Shaders/ComputeShader.hpp
    # Shader Reflection
    include/velecs/graphics/Shader/Reflection/ShaderMemberType.hpp
    include/velecs/graphics/Shader/Reflection/ShaderMember.hpp
    include/velecs/graphics/Shader/Reflection/ShaderResourceType.hpp
    include/velecs/graphics/Shader/Reflection/ShaderResource.hpp
    include/velecs/graphics/Shader/Reflection/ShaderReflectionData.hpp
    include/velecs/graphics/Shader/Reflection/ShaderReflector.hpp

    include/velecs/graphics/Rect.hpp
    include/velecs/graphics/Color32.hpp

    # Include ImGui headers for IDE organization
    ${IMGUI_HEADERS}
)

# Always build the library
add_library(velecs-graphics ${LIB_SOURCES} ${LIB_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(velecs-graphics
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    # Add ImGui include paths
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends>
)

target_link_libraries(velecs-graphics
    PUBLIC SDL3::SDL3
    PUBLIC Vulkan::Vulkan
    PUBLIC vma
    PUBLIC volk
    PUBLIC vk-bootstrap::vk-bootstrap
    PUBLIC spirv_cross
    PUBLIC assimp
    PUBLIC velecs-common
    PUBLIC velecs-math
    PUBLIC velecs-ecs
)

if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(VELECS_GRAPHICS_LIBRARIES velecs-graphics PARENT_SCOPE)
endif()
