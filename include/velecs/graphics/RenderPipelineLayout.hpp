/// @file    RenderPipelineLayout.hpp
/// @author  Matthew Green
/// @date    2025-07-18 15:45:47
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>
#include <typeinfo>
#include <type_traits>

namespace velecs::graphics {

/// @class RenderPipelineLayout
/// @brief Brief description.
///
/// Rest of description.
class RenderPipelineLayout {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    RenderPipelineLayout() = default;

    /// @brief Default deconstructor.
    ~RenderPipelineLayout() = default;

    // Delete copy operations to prevent accidental copying of Vulkan handles
    RenderPipelineLayout(const RenderPipelineLayout&) = delete;
    RenderPipelineLayout& operator=(const RenderPipelineLayout&) = delete;

    // Allow move operations
    RenderPipelineLayout(RenderPipelineLayout&& other) noexcept;
    RenderPipelineLayout& operator=(RenderPipelineLayout&& other) noexcept;

    // Public Methods

    /// @brief Gets the device handle used by this layout
    /// @return The Vulkan device handle
    inline VkDevice GetDevice() const { return _device; }

    /// @brief Validates that a push constant type matches what was configured
    /// @tparam T The type to validate against the configured push constant
    /// @return True if the type matches the configured push constant type
    template<typename T>
    bool ValidatePushConstantType() const
    {
        return _pushConstantTypeHash == typeid(T).hash_code() && 
               _pushConstantSize == sizeof(T);
    }

    /// @brief Gets the size of the configured push constant
    /// @return Size in bytes of the push constant, or 0 if none configured
    inline uint32_t GetPushConstantSize() const { return _pushConstantSize; }

    /// @brief Checks if this layout has push constants configured
    /// @return True if push constants are configured
    inline bool HasPushConstants() const { return !_pushConstantRanges.empty(); }

    /// @brief Gets the number of descriptor set layouts
    /// @return Number of descriptor set layouts in this pipeline layout
    inline size_t GetDescriptorSetLayoutCount() const { return _descriptorSetLayouts.size(); }

    inline RenderPipelineLayout& SetDevice(VkDevice device) { _device = device; return *this; }

    /// @brief Sets up a push constant for the specified type and shader stages
    /// @tparam T The type of the push constant struct (must be standard layout)
    /// @param stageFlags Which shader stages can access this push constant
    /// @param offset Optional offset in bytes (default: 0)
    /// @return Reference to this layout for method chaining
    /// @throws std::runtime_error if layout is already built or if type requirements aren't met
    template<typename T>
    RenderPipelineLayout& SetPushConstant(VkShaderStageFlags stageFlags, uint32_t offset = 0)
    {
        static_assert(std::is_standard_layout_v<T>, "Push constant type must have standard layout");
        static_assert(std::is_trivially_copyable_v<T>, "Push constant type must be trivially copyable");
        
        VkPushConstantRange range{};
        range.stageFlags = stageFlags;
        range.offset = offset;
        range.size = sizeof(T);

        _pushConstantRanges.push_back(range);
        _pushConstantTypeHash = typeid(T).hash_code();
        _pushConstantSize = sizeof(T);

        return *this;
    }

    /// @brief Adds a descriptor set layout to the pipeline layout
    /// @param setLayout The descriptor set layout to add
    /// @return Reference to this layout for method chaining
    /// @throws std::runtime_error if layout is already built
    RenderPipelineLayout& AddDescriptorSetLayout(VkDescriptorSetLayout setLayout);

    /// @brief Adds multiple descriptor set layouts to the pipeline layout
    /// @param setLayouts Vector of descriptor set layouts to add
    /// @return Reference to this layout for method chaining
    /// @throws std::runtime_error if layout is already built
    RenderPipelineLayout& AddDescriptorSetLayouts(const std::vector<VkDescriptorSetLayout>& setLayouts);

    VkPipelineLayout GetLayout();

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    VkDevice _device{VK_NULL_HANDLE};                           /// @brief Vulkan device handle
    VkPipelineLayout _layout{VK_NULL_HANDLE};                   /// @brief The built pipeline layout
    std::vector<VkDescriptorSetLayout> _descriptorSetLayouts;   /// @brief Descriptor set layouts
    std::vector<VkPushConstantRange> _pushConstantRanges;       /// @brief Push constant ranges

    size_t _pushConstantTypeHash{0};                            /// @brief Hash of the push constant type for validation
    uint32_t _pushConstantSize{0};                              /// @brief Size of the push constant type

    // Private Methods
};

} // namespace velecs::graphics
