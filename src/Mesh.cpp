/// @file    Mesh.cpp
/// @author  Matthew Green
/// @date    2025-07-21 12:11:54
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Mesh.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

void Mesh::Clear()
{
    vertices.clear();
    indices.clear();
}

void Mesh::SetVertices(const std::vector<Vertex>& verts)
{
    vertices = verts;
    MarkDirty();
}

void Mesh::SetVertices(std::vector<Vertex>&& verts)
{
    vertices = std::move(verts);
    MarkDirty();
}

void Mesh::SetIndices(const std::vector<uint32_t>& indices)
{
    this->indices = indices;
    MarkDirty();
}

void Mesh::SetIndices(std::vector<uint32_t>&& idx)
{
    indices = std::move(idx);
    MarkDirty();
}

bool Mesh::LoadFrom(const std::filesystem::path& relPath, uint32_t meshIndex/* = 0*/)
{
    auto filePath = Paths::AssetsDir() / relPath;

    auto importer = AssimpLoadScene(filePath);
    const auto& scene = importer->GetScene();

    // Check if requested mesh index exists
    if (meshIndex >= scene->mNumMeshes)
    {
        std::ostringstream oss{};
        oss << "Mesh index " << meshIndex << " out of range. File has " << scene->mNumMeshes << " meshes";
        throw std::out_of_range(oss.str());
    }

    // Get the specific mesh
    const aiMesh* mesh = scene->mMeshes[meshIndex];

    LoadFromAssimpMesh(mesh);
        
    return true;
}

std::unique_ptr<Mesh> Mesh::CreateFrom(const std::filesystem::path& relPath, uint32_t meshIndex/* = 0*/)
{
    auto mesh = std::make_unique<Mesh>();
    if (!mesh->LoadFrom(relPath, meshIndex)) {
        return nullptr;  // Return null on failure
    }
    return mesh;
}

std::vector<std::unique_ptr<Mesh>> Mesh::CreateAllFrom(const std::filesystem::path& relPath)
{
    auto filePath = Paths::AssetsDir() / relPath;

    auto importer = AssimpLoadScene(filePath);
    const auto& scene = importer->GetScene();

    uint32_t numOfMeshes = scene->mNumMeshes;
    std::vector<std::unique_ptr<Mesh>> meshes;
    meshes.reserve(numOfMeshes);
    for (uint32_t i = 0; i < numOfMeshes; ++i)
    {
        const aiMesh* assimpMesh = scene->mMeshes[i];
        auto mesh = std::make_unique<Mesh>();
        mesh->LoadFromAssimpMesh(assimpMesh);
        meshes.push_back(std::move(mesh));
    }
        
    return meshes;
}

void Mesh::Upload(VkDevice device, VmaAllocator allocator)
{

}

void Mesh::Draw(VkCommandBuffer cmd)
{

}

VkPipelineVertexInputStateCreateInfo Mesh::GetVertexInputInfo() const
{
    throw std::exception("Function not implemented.");
}

size_t Mesh::GetPrimitiveCount() const
{ 
    if (IsIndexed()) {
        return indices.size() / 3;  // Triangles from indices
    }
    return vertices.size() / 3;     // Triangles from vertices
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

std::unique_ptr<Assimp::Importer> Mesh::AssimpLoadScene(const std::filesystem::path& filePath)
{
    if (!filePath.has_extension())
        throw std::invalid_argument("Mesh file requires an extension");

    std::string ext = filePath.extension().string();
    if (aiIsExtensionSupported(ext.c_str()) == AI_FALSE)
    {
        std::ostringstream oss{};
        oss << "Extension '" << ext << "' is not currently supported.";
        throw std::runtime_error(oss.str());
    }
    
    if (!std::filesystem::exists(filePath))
        throw std::runtime_error("File does not exist: " + filePath.string());
    
    // Create ASSIMP importer
    auto importer = std::make_unique<Assimp::Importer>();
    
    // Set import flags - these are common for graphics engines
    unsigned int importFlags = 
        aiProcess_Triangulate |         // Convert to triangles
        aiProcess_GenNormals |          // Generate normals if missing
        aiProcess_CalcTangentSpace |    // Calculate tangents for normal mapping
        aiProcess_OptimizeMeshes |      // Reduce number of meshes
        aiProcess_JoinIdenticalVertices // Remove duplicate vertices
        ;
    
    // Load the scene
    const aiScene* scene = importer->ReadFile(filePath.string(), importFlags);
    
    // Check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw std::runtime_error("ASSIMP Error: " + std::string(importer->GetErrorString()));

    return importer;
}

void Mesh::LoadFromAssimpMesh(const aiMesh* assimpMesh)
{
    // Clear existing data
    Clear();

    // Prepare vecs
    ReserveVertices(assimpMesh->mNumVertices);
    if (assimpMesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE)
    {
        ReserveIndices(assimpMesh->mNumFaces * 3);
    }
    else
    {
        throw std::runtime_error("Assimp mesh does not use primitive type triangle.");
    }
    
    // Load vertices
    
    for (unsigned int i = 0; i < assimpMesh->mNumVertices; i++)
    {
        Vertex vertex{};
        
        // Position (required)
        vertex.pos = Vec3{
            assimpMesh->mVertices[i].x,
            assimpMesh->mVertices[i].y,
            assimpMesh->mVertices[i].z
        };
        
        // // Normal (if available)
        // if (assimpMesh->HasNormals())
        // {
        //     assimpMesh.normal = Vec3{
        //         assimpMesh->mNormals[i].x,
        //         assimpMesh->mNormals[i].y,
        //         assimpMesh->mNormals[i].z
        //     };
        // }
        
        // // Texture coordinates (first set, if available)
        // if (assimpMesh->mTextureCoords[0])
        // {
        //     vertex.texCoord = Vec2{
        //         assimpMesh->mTextureCoords[0][i].x,
        //         assimpMesh->mTextureCoords[0][i].y
        //     };
        // }
        
        // // Color (first set, if available)
        // if (assimpMesh->mColors[0])
        // {
        //     vertex.color = Color32::FromFloat(
        //         assimpMesh->mColors[0][i].r,
        //         assimpMesh->mColors[0][i].g,
        //         assimpMesh->mColors[0][i].b,
        //         assimpMesh->mColors[0][i].a
        //     );
        // }
        // else
        // {
        //     vertex.color = Color32::WHITE; // Default color
        // }
        
        vertices.push_back(vertex);
    }
    
    // Load indices
    for (unsigned int i = 0; i < assimpMesh->mNumFaces; i++)
    {
        const aiFace& face = assimpMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(static_cast<uint32_t>(face.mIndices[j]));
        }
    }
    
    // Mark as dirty so GPU buffers get updated
    MarkDirty();
}

} // namespace velecs::graphics
