/// @file    MeshBase.hpp
/// @author  Matthew Green
/// @date    2025-07-20 15:55:39
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <vulkan/vulkan_core.h>

#include <vma/vk_mem_alloc.h>

namespace velecs::graphics {

/// @class MeshBase
/// @brief Abstract base class for all renderable mesh types.
///
/// Provides a common interface for meshes with different vertex formats,
/// generation methods (CPU vertices, compute shaders, procedural), and
/// rendering techniques (standard, instanced, etc.).
class MeshBase {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Virtual destructor for proper cleanup of derived types.
    virtual ~MeshBase() = default;

    // Public Methods

    /// @brief Uploads mesh data to GPU memory.
    /// @param device The Vulkan device handle
    /// @param allocator The VMA allocator for buffer creation
    /// @details Call when mesh data changes to sync CPU data with GPU buffers.
    ///          Implementation should check IsDirty() to avoid unnecessary uploads.
    virtual void Upload(VkDevice device, VmaAllocator allocator) = 0;

    /// @brief Records draw commands to the command buffer.
    /// @param cmd The command buffer to record draw commands into
    /// @details Implementation should bind vertex/index buffers and issue draw calls.
    ///          Assumes render pass is active and pipeline is bound.
    virtual void Draw(VkCommandBuffer cmd) = 0;

    /// @brief Gets the vertex input description for pipeline creation.
    /// @return Vertex input state configuration for this mesh type
    /// @details Returns empty vertex input for vertex-less rendering (fullscreen quads, compute-generated geometry).
    virtual VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() const = 0;
    
    /// @brief Gets the total number of vertices in this mesh.
    /// @return Number of vertices (0 for vertex-less meshes)
    /// @details Used for debugging, validation, and draw call optimization.
    virtual size_t GetVertexCount() const = 0;
    
    /// @brief Gets the number of primitives (triangles, lines, points) in this mesh.
    /// @return Number of primitives that will be rendered
    /// @details Used for performance profiling and statistics gathering.
    virtual size_t GetPrimitiveCount() const = 0;

protected:
    // Protected Fields

    // Protected Methods

    /// @brief Checks if mesh data has changed and needs re-upload.
    /// @return True if mesh needs to be uploaded to GPU
    inline bool IsDirty() const { return _isDirty; }

    /// @brief Marks mesh data as changed, requiring GPU re-upload.
    /// @details Call this whenever CPU-side mesh data is modified.
    inline void MarkDirty() { _isDirty = true; }

    /// @brief Marks mesh as clean after successful GPU upload.
    /// @details Call this after Upload() completes successfully.
    inline void MarkClean() { _isDirty = false; }

private:
    // Private Fields

    /// @brief Tracks whether mesh data needs to be re-uploaded to GPU.
    bool _isDirty{true};

    // Private Methods
};

} // namespace velecs::graphics
