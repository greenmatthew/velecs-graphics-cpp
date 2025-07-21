/// @file    Mesh.hpp
/// @author  Matthew Green
/// @date    2025-07-20 16:20:42
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/MeshBase.hpp"

#include "velecs/graphics/Vertex.hpp"
#include "velecs/graphics/Memory/AllocatedBuffer.hpp"

#include <velecs/common/Paths.hpp>

#include <velecs/math/Vec3.hpp>

#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include <stdexcept>
#include <filesystem>
#include <vector>
#include <memory>
#include <sstream>
#include <functional>

namespace velecs::graphics {

/// @class Mesh
/// @brief Brief description.
///
/// Rest of description.
class Mesh : public MeshBase {
public:
    using Paths = velecs::common::Paths;
    using Vec3 = velecs::math::Vec3;

    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor for empty mesh.
    Mesh() = default;

    /// @brief Virtual destructor for proper cleanup.
    ~Mesh() = default;

    // Delete copy operations to prevent accidental copying of GPU resources
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Allow move operations
    Mesh(Mesh&&) = default;
    Mesh& operator=(Mesh&&) = default;

    // Public Methods

    void Clear();

    /// @brief Sets vertex data and marks mesh as dirty.
    /// @param verts Vector of vertices to copy
    /// @details Replaces existing vertex data. Call Upload() to sync with GPU.
    void SetVertices(const std::vector<Vertex>& verts);

    /// @brief Sets vertex data and marks mesh as dirty (move version).
    /// @param verts Vector of vertices to move
    /// @details Replaces existing vertex data efficiently. Call Upload() to sync with GPU.
    void SetVertices(std::vector<Vertex>&& verts);

    /// @brief Sets index data and marks mesh as dirty.
    /// @param indices Vector of indices to copy
    /// @details Replaces existing index data. Call Upload() to sync with GPU.
    void SetIndices(const std::vector<uint32_t>& indices);

    /// @brief Sets index data and marks mesh as dirty (move version).
    /// @param idx Vector of indices to move
    /// @details Replaces existing index data efficiently. Call Upload() to sync with GPU.
    void SetIndices(std::vector<uint32_t>&& idx);

    /// @brief Reserves space for vertices to avoid reallocations.
    /// @param count Number of vertices to reserve space for
    inline void ReserveVertices(const size_t count) { vertices.reserve(count); }

    /// @brief Reserves space for indices to avoid reallocations.
    /// @param count Number of indices to reserve space for
    /// @details Useful when building index data procedurally.
    void ReserveIndices(const size_t count) { indices.reserve(count); }

    /// @brief Loads mesh data from a file, replacing existing data.
    /// @param relPath Relative path to the mesh file from the assets directory (supports formats: .obj, .fbx, .gltf, .dae, etc.)
    /// @param meshIndex Index of mesh to load from multi-mesh files (default: 0)
    /// @return True if loading succeeded, false on error
    /// @details Uses ASSIMP to load file. File path is resolved relative to Paths::AssetsDir().
    ///          Marks mesh as dirty on success. For multi-mesh files, loads the first mesh by default.
    bool LoadFrom(const std::filesystem::path& relPath, uint32_t meshIndex = 0);

    /// @brief Static factory method to create mesh from file.
    /// @param relPath Relative path to the mesh file from the assets directory (supports formats: .obj, .fbx, .gltf, .dae, etc.)
    /// @param meshIndex Index of mesh to load from multi-mesh files (default: 0)
    /// @return Unique pointer to loaded mesh, or nullptr on failure
    /// @details Convenience method for creating and loading in one step.
    static std::unique_ptr<Mesh> CreateFrom(const std::filesystem::path& relPath, uint32_t meshIndex = 0);

    /// @brief Loads all meshes from a multi-mesh file.
    /// @param relPath Relative path to the mesh file from the assets directory (supports formats: .obj, .fbx, .gltf, .dae, etc.)
    /// @return Vector of loaded meshes (empty on failure)
    /// @details Useful for loading scenes with multiple objects.
    static std::vector<std::unique_ptr<Mesh>> CreateAllFrom(const std::filesystem::path& relPath);

    /// @brief Gets direct access to vertex data.
    /// @return Const reference to vertex vector
    /// @details Use carefully - modifying through non-const access requires calling MarkDirty().
    inline const std::vector<Vertex>& GetVertices() const { return vertices; }

    /// @brief Gets direct access to index data.
    /// @return Const reference to index vector
    inline const std::vector<uint32_t>& GetIndices() const { return indices; }

    /// @brief Checks if mesh has index data.
    /// @return True if mesh uses indexed rendering
    inline bool IsIndexed() const { return !indices.empty(); }

    // MeshBase interface implementation
    void UploadImmediately(
        VkDevice device,
        VmaAllocator allocator,
        std::function<void(std::function<void(VkCommandBuffer)>)> immediateSubmit
    );

    void Upload(VkDevice device, VmaAllocator allocator) override;

    void Draw(VkCommandBuffer cmd) override;

    VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() const override;
    
    inline size_t GetVertexCount() const override { return vertices.size(); }

    /// @brief Gets the total number of indices in this mesh.
    /// @return Number of indices (0 for non-indexed meshes)
    /// @details Useful for debugging and buffer size calculations.
    inline size_t GetIndexCount() const { return indices.size(); }
    
    /// @brief Gets the number of primitives (triangles, lines, points) in this mesh.
    /// @return Number of primitives that will be rendered
    /// @details Used for performance profiling and statistics gathering.
    ///          Assumes triangle primitives (divides by 3).
    size_t GetPrimitiveCount() const override;

protected:
    // Protected Fields

    /// @brief CPU-side vertex data.
    std::vector<Vertex> vertices;
    
    /// @brief CPU-side index data (optional).
    std::vector<uint32_t> indices;
    
    /// @brief GPU vertex buffer.
    std::unique_ptr<AllocatedBuffer> vertexBuffer{nullptr};
    
    /// @brief GPU index buffer.
    std::unique_ptr<AllocatedBuffer> indexBuffer{nullptr};

    // Protected Methods

private:
    // Private Fields

    // Private Methods

    /// @brief Internal helper to create GPU buffer from data.
    /// @param device Vulkan device handle
    /// @param allocator VMA allocator
    /// @param data Vector of data to upload
    /// @param usage Buffer usage flags
    /// @param buffer Output buffer to create
    /// @return True on success, false on failure
    template<typename T>
    bool CreateBuffer(VkDevice device, VmaAllocator allocator, 
                     const std::vector<T>& data, VkBufferUsageFlags usage,
                     AllocatedBuffer& buffer);
    
    static std::unique_ptr<Assimp::Importer> AssimpLoadScene(const std::filesystem::path& filePath);

    void LoadFromAssimpMesh(const aiMesh* assimpMesh);
};

} // namespace velecs::graphics
