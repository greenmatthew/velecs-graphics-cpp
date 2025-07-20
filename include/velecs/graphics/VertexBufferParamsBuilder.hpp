/// @file    VertexBufferParamsBuilder.hpp
/// @author  Matthew Green
/// @date    2025-07-12 16:21:02
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>
#include <functional>

namespace velecs::graphics {

/// @class VertexBufferParamsBuilder
/// @brief Builder class for creating Vulkan vertex input state configurations with a fluent interface.
///
/// This class provides an elegant way to configure vertex buffer bindings and attributes for Vulkan pipelines.
/// It uses a builder pattern with method chaining to simplify the complex setup of VkPipelineVertexInputStateCreateInfo.
/// The builder automatically manages binding indices and location assignments, reducing boilerplate and errors.
///
/// @code
/// struct Vertex {
///     Vec3 pos;
///     uint16_t normX, normY;
///     Color32 color;
///
///     static VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() {
///         static const auto createInfo = VertexBufferParamsBuilder()
///             .AddBinding(sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX, [](VertexInputBinding& binding) {
///                 binding.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos))
///                        .AddAttribute(VK_FORMAT_R16G16_SNORM, offsetof(Vertex, normX))
///                        .AddAttribute(VK_FORMAT_B8G8R8A8_SRGB, offsetof(Vertex, color));
///             })
///             .GetCreateInfo();
///         return createInfo;
///     }
/// };
/// @endcode
class VertexBufferParamsBuilder {
private:
    class VertexInputBinding {
    public:
        VkVertexInputBindingDescription _bindingDescription{};

        VertexInputBinding& AddAttribute(VkFormat format, uint32_t offset)
        {
            VkVertexInputAttributeDescription description{};
            description.binding = _bindingDescription.binding;
            description.location = _parent._location++;
            description.format = format;
            description.offset = offset;
            _parent._attributes.emplace_back(description);
            return *this;
        }

    private:
        VertexBufferParamsBuilder& _parent;  // Reference to parent
        
        // Constructor that takes parent reference
        VertexInputBinding(VertexBufferParamsBuilder& parent, uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
            : _parent(parent)
        {
            _bindingDescription.binding = binding;
            _bindingDescription.stride = stride;
            _bindingDescription.inputRate = inputRate;
        }
        
        friend class VertexBufferParamsBuilder;  // Let parent access private constructor
    };

public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    VertexBufferParamsBuilder() = default;

    /// @brief Default deconstructor.
    ~VertexBufferParamsBuilder() = default;

    // Public Methods

    using BindingConfigurator = std::function<void(VertexInputBinding&)>;

    VertexBufferParamsBuilder& AddBinding
    (
        uint32_t stride,
        VkVertexInputRate inputRate,
        BindingConfigurator configurator
    );

    const VkPipelineVertexInputStateCreateInfo& GetCreateInfo() const;

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    std::vector<VkVertexInputBindingDescription> _bindings;
    std::vector<VkVertexInputAttributeDescription> _attributes;
    uint32_t _location{0U};

    // Mutable so we can modify it in const method
    mutable VkPipelineVertexInputStateCreateInfo _createInfo{};
    mutable bool _createInfoValid{false};

    // Private Methods
};

} // namespace velecs::graphics
