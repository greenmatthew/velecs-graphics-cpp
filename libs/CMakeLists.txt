cmake_minimum_required(VERSION 3.14)

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
add_subdirectory(vk-bootstrap)

# Create the main VULKAN interface that bundles everything
add_library(VULKAN INTERFACE)
target_link_libraries(VULKAN
    INTERFACE Vulkan::Vulkan
    INTERFACE vma
    INTERFACE volk
    INTERFACE vk-bootstrap::vk-bootstrap
)

# Future external dependencies:
# add_subdirectory(imgui)         # For UI later
# add_subdirectory(imgui_guizo)   # For UI later
# add_subdirectory(stb)           # Simple mesh loader
# add_subdirectory(assimp)        # Better mesh loader