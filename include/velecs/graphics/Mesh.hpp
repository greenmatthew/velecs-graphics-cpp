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

#include "velecs/graphics/Memory/AllocatedBuffer.hpp"

#include <filesystem>
#include <vector>
#include <memory>

namespace velecs::graphics {

/// @class Mesh
/// @brief Brief description.
///
/// Rest of description.
template<typename VertexType, typename IndexType>
class Mesh : MeshBase {
public:
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

    // @brief Sets vertex data and marks mesh as dirty.
    /// @param verts Vector of vertices to copy
    /// @details Replaces existing vertex data. Call Upload() to sync with GPU.
    void SetVertices(const std::vector<VertexType>& verts);

    /// @brief Sets index data and marks mesh as dirty.
    /// @param idx Vector of indices to copy
    /// @details Replaces existing index data. Call Upload() to sync with GPU.
    void SetIndices(const std::vector<IndexType>& idx);

    /// @brief Adds a single vertex to the mesh.
    /// @param vertex The vertex to add
    /// @details Useful for procedural generation. Marks mesh as dirty.
    void AddVertex(const VertexType& vertex);

    /// @brief Reserves space for vertices to avoid reallocations.
    /// @param count Number of vertices to reserve space for
    void ReserveVertices(size_t count);

    /// @brief Loads mesh data from a file, replacing existing data.
    /// @param filePath Path to the mesh file (supports formats: .obj, .fbx, .gltf, .dae, etc.)
    /// @param meshIndex Index of mesh to load from multi-mesh files (default: 0)
    /// @return True if loading succeeded, false on error
    /// @details Uses ASSIMP to load file. Marks mesh as dirty on success.
    ///          For multi-mesh files, loads the first mesh by default.
    bool LoadFrom(const std::filesystem::path& filePath, uint32_t meshIndex = 0);

    /// @brief Static factory method to create mesh from file.
    /// @param filePath Path to the mesh file
    /// @param meshIndex Index of mesh to load from multi-mesh files (default: 0)
    /// @return Unique pointer to loaded mesh, or nullptr on failure
    /// @details Convenience method for creating and loading in one step.
    static std::unique_ptr<Mesh> CreateFrom(const std::filesystem::path& filePath, uint32_t meshIndex = 0);

    /// @brief Loads all meshes from a multi-mesh file.
    /// @param filePath Path to the mesh file
    /// @return Vector of loaded meshes (empty on failure)
    /// @details Useful for loading scenes with multiple objects.
    static std::vector<std::unique_ptr<Mesh>> CreateAllFrom(const std::filesystem::path& filePath);

    /// @brief Gets direct access to vertex data.
    /// @return Const reference to vertex vector
    /// @details Use carefully - modifying through non-const access requires calling MarkDirty().
    const std::vector<VertexType>& GetVertices() const { return vertices; }

    /// @brief Gets direct access to index data.
    /// @return Const reference to index vector
    const std::vector<IndexType>& GetIndices() const { return indices; }

    /// @brief Checks if mesh has index data.
    /// @return True if mesh uses indexed rendering
    bool IsIndexed() const { return !indices.empty(); }

    // MeshBase interface implementation
    void Upload(VkDevice device, VmaAllocator allocator) override;
    void Draw(VkCommandBuffer cmd) override;
    VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() const override;
    uint32_t GetVertexCount() const override;
    uint32_t GetPrimitiveCount() const override;

protected:
    // Protected Fields

    /// @brief CPU-side vertex data.
    std::vector<VertexType> vertices;
    
    /// @brief CPU-side index data (optional).
    std::vector<IndexType> indices;
    
    /// @brief GPU vertex buffer.
    AllocatedBuffer vertexBuffer;
    
    /// @brief GPU index buffer.
    AllocatedBuffer indexBuffer;

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

    /// @brief Converts ASSIMP mesh data to our vertex format.
    /// @param assimpMesh The ASSIMP mesh to convert
    /// @return True on successful conversion
    bool ConvertFromAssimp(const struct aiMesh* assimpMesh);
};

} // namespace velecs::graphics
