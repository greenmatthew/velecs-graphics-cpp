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

if(NOT TARGET velecs-common)
   add_subdirectory(../velecs-common ${CMAKE_BINARY_DIR}/velecs-common)
endif()

if(NOT TARGET velecs-math)
   add_subdirectory(../velecs-math ${CMAKE_BINARY_DIR}/velecs-math)
endif()

if(NOT TARGET velecs-ecs)
   add_subdirectory(../velecs-ecs ${CMAKE_BINARY_DIR}/velecs-ecs)
endif()

# Add external dependencies (including Vulkan)
add_subdirectory(libs)

# Source files for the library
set(LIB_SOURCES
   src/Color32.cpp
   # Future Vulkan sources here
)

# Header files for the library (for IDE organization)  
set(LIB_HEADERS
   include/velecs/graphics/Common.hpp

   include/velecs/graphics/Graphics.hpp

   include/velecs/graphics/Color32.hpp
   # Future Vulkan headers here
)

# Always build the library
add_library(velecs-graphics ${LIB_SOURCES} ${LIB_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(velecs-graphics
   PUBLIC
       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(velecs-graphics
   PUBLIC velecs-common
   PUBLIC velecs-math
   PUBLIC velecs-ecs
   PUBLIC Vulkan
)

if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
   set(VELECS_GRAPHICS_LIBRARIES velecs-graphics PARENT_SCOPE)
endif()
