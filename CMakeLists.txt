cmake_minimum_required(VERSION 3.14)
project(velecs-graphics VERSION 0.1.0)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories to bin
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

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



# Future external dependencies can be added here:
# add_subdirectory(libs/imgui)         # For UI later
# add_subdirectory(libs/imgui_guizo)   # For UI later
# add_subdirectory(libs/stb)           # Simple mesh loader
# add_subdirectory(libs/assimp)        # Better mesh loader

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

# Source files for the library
set(LIB_SOURCES
   src/RenderEngine.cpp

   src/VulkanInitializers.cpp

   src/Memory/DeletionQueue.cpp

   src/Color32.cpp
)

# Header files for the library (for IDE organization)  
set(LIB_HEADERS
   include/velecs/graphics/Common.hpp

   include/velecs/graphics/RenderEngine.hpp
   include/velecs/graphics/VulkanInitializers.hpp

   include/velecs/graphics/Memory/AllocatedBuffer.hpp
   include/velecs/graphics/Memory/AllocatedImage.hpp
   include/velecs/graphics/Memory/DeletionQueue.hpp
   include/velecs/graphics/Memory/UploadContext.hpp

   include/velecs/graphics/Rect.hpp
   include/velecs/graphics/Color32.hpp
)

# Always build the library
add_library(velecs-graphics ${LIB_SOURCES} ${LIB_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(velecs-graphics
   PUBLIC
       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(velecs-graphics
   PUBLIC SDL3::SDL3
   PUBLIC Vulkan::Vulkan
   PUBLIC vma
   PUBLIC volk
   PUBLIC vk-bootstrap::vk-bootstrap
   PUBLIC velecs-common
   PUBLIC velecs-math
   PUBLIC velecs-ecs
)

if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
   set(VELECS_GRAPHICS_LIBRARIES velecs-graphics PARENT_SCOPE)
endif()
