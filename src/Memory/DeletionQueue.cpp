/// @file    DeletionQueue.cpp
/// @author  Matthew Green
/// @date    2025-07-12 12:18:56
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Memory/DeletionQueue.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

void DeletionQueue::PushDeleter(std::function<void()>&& deleter)
{
    _deleters.push_back(std::move(deleter));
}

void DeletionQueue::Flush()
{
    // Reverse iterate the deletion queue to execute all the functions
    for (auto it = _deleters.rbegin(); it != _deleters.rend(); it++)
    {
        (*it)(); // Call the deleter
    }
    _deleters.clear();
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
